#include "Driver.h"

#include "Ast.h"
#include "Semantic.h"
#include "Builtin.h"
#include "CodeGeneration.h"
#include "Linking.h"

#include "Printer.h"
#include "ErrorReporting.h"
#include "Path.h"

#include <cstdio>
#include <set>
#include <functional>
#include <algorithm>

extern std::unique_ptr<std::vector<std::unique_ptr<qlow::ast::AstObject>>> parsedClasses;
extern FILE* qlow_parser_in;
extern int qlow_parser_parse(void);
extern const char* qlow_parser_filename;

using namespace qlow;

Options Options::parseOptions(int argc, char** argv)
{
    static const std::map<std::string, bool Options::*> boolArgs = 
    {
        {"-S",              &Options::emitAssembly},
        {"--emit-assembly", &Options::emitAssembly},
        {"-L",              &Options::emitLlvm},
        {"--emit-llvm",     &Options::emitLlvm},
    };
    
    Options options{};
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (boolArgs.find(arg) != boolArgs.end()) {
            bool Options::* attr = boolArgs.find(arg)->second;
            options.*attr = true;
        }
        else if (arg == "-o" || arg == "--out") {
            if (argc > i + 1) {
                options.outfile = argv[++i];
            }
            else {
                throw "Please specify a filename after '-o'";
            }
        }
        else if (arg.rfind("-O", 0) == 0) {
            if (arg.size() > 2) {
                options.optLevel = std::stoi(arg.substr(2));
            }
            else {
                options.optLevel = 2;
            }
        }
        else if (arg.rfind("-l", 0) == 0) {
            if (arg.size() > 2) {
                options.libs.push_back(arg.substr(2));
            }
        }
        else {
            if (options.outfile == "")
                options.outfile = arg + ".o";
            options.infiles.push_back(std::move(arg));
        }
    }
    if (options.outfile == "")
        options.outfile = "a.out";
    return options;
}


Driver::Driver(int argc, char** argv) :
    options{ Options::parseOptions(argc, argv) }
{
}


int Driver::run(void) try
{
    Printer& printer = Printer::getInstance();
    if (options.infiles.empty()) {
        qlow::printError(ErrorCode::NO_INFILES, printer);   
        return 1;     
    }

#ifdef DEBUGGING
    printer << "starting parser" << std::endl;
#endif
    //printer.logError("driver not yet implemented", {options.emitAssembly ? "asm" : "noasm", 10, 11, 12, 13});
    bool errorOccurred = parseStage();
    
    if (errorOccurred) {
        printer << "Aborting due to syntax errors." << std::endl;
        return 1;
    }
    
    errorOccurred = semanticStage();
    
    if (errorOccurred) {
        printer << "Aborting due to semantic errors." << std::endl;
        return 1;
    }
    

    for (auto& [a, b] : semClasses->classes) {
#ifdef DEBUGGING
        printer << a << ": " << b->toString() << std::endl;
#endif
    }
    
    

    /*auto main = semClasses->classes.find("Main");
    qlow::sem::Class* mainClass = nullptr;
    if (main == semClasses->classes.end()) {
        printer.logError("No Main class found");
        return 1;
    }
    else {
        mainClass = main->second.get();
    }*/
    
    auto* mainMethod = semClasses->getMethod("main");
    if (mainMethod == nullptr) {
        // TODO handle main ckeck well
        qlow::printError(printer, "no main method found");
        return 1;
    }
    
#ifdef DEBUGGING
    printer << "starting code generation!" << std::endl;
#endif

    std::unique_ptr<llvm::Module> mod = nullptr;
    
    try {
        mod = qlow::gen::generateModule(*semClasses);
    }
    catch (const char* err) {
        reportError(err);
        return 1;
    }
    catch (SemanticError& err) {
        err.print(printer);
        return 1;
    }
    catch (...) {
        reportError("unknown error during code generation");
        return 1;
    }
    
    // TODO create better tempfile
    tempObject = "/tmp/temp.o";

    try {
        qlow::gen::generateObjectFile(tempObject, std::move(mod), options.optLevel);
    }
    catch (const char* msg) {
        printError(printer, msg);
        return 1;
    }
    catch (...) {
        printError(printer, "unknown error during object file creation");
        reportError("unknown error during object file creation");
        return 1;
    }

#ifdef DEBUGGING
    printer << "object exported!" << std::endl;
#endif

    try {
        linkingStage();
    }
    catch (...) {
        reportError("unknown error during linking");
        return 1;
    }
    
    return 0;
}
catch (InternalError& e) {
    e.print(Printer::getInstance());
    return 1;
}


bool Driver::parseStage(void)
{
    using namespace std::literals;
    Printer& printer = Printer::getInstance();

    this->ast = std::make_unique<ast::Ast>();
    bool errorOccurred = false;

    std::set<qlow::util::Path> alreadyParsed = {};
    std::set<qlow::util::Path> toParse = {};

    toParse.insert(options.infiles.begin(), options.infiles.end());

    while(!toParse.empty()) {
        auto filename = toParse.extract(toParse.begin()).value();
        auto dirPath = filename.parentPath();
        std::FILE* file = std::fopen(filename.c_str(), "r");

        if (!file) {
            reportError("could not open file "s + filename.string() + ".");
            errorOccurred = true;
            continue;
        }

        try {
            // parse file content and add parsed objects to global ast
            ast::Parser parser(file, filename);
            this->ast->merge(parser.parse());
            for (auto& import : parser.getImports()) {
                auto importPath = dirPath;
                importPath.append(import->getRelativePath());
#ifdef DEBUGGING
                printer << "imported " << importPath.string() << std::endl;
#endif
                if (alreadyParsed.count(dirPath) == 0) {
                    toParse.insert(importPath);
                }
            }
        }
        catch (const CompileError& ce) {
            ce.print(printer);
            errorOccurred = true;
        }
        catch (const char* errMsg) {
            reportError(errMsg);
            errorOccurred = true;
        }
        catch (...) {
            reportError("an unknown error occurred.");
            errorOccurred = true;
            throw;
        }
        
        if (file)
            std::fclose(file);
    }

    return errorOccurred;
}


bool Driver::semanticStage(void)
{
    Printer& printer = Printer::getInstance();
    bool errorOccurred = false;

    try {
        std::tie(this->context, this->semClasses) = qlow::sem::createFromAst(*this->ast);
    }
    catch(SemanticError& se) {
        se.print(printer);
        errorOccurred = true;
    }
    catch(const char* err) {
        reportError(err);
        errorOccurred = true;
    }
    catch (...) {
        reportError("an unknown error occurred.");
        errorOccurred = true;
    }

    return errorOccurred;
}


bool Driver::linkingStage(void)
{
    using namespace std::literals;
    bool errorOccurred = false;
    auto linkerPath = qlow::getLinkerExecutable();

    std::vector<std::string> ldArgs = {
        tempObject.string(), "-e", "_qlow_start", "-o", options.outfile,
        "-lc",
#ifdef __linux__
        "-dynamic-linker", "/lib64/ld-linux-x86-64.so.2"
#endif
    };

    std::copy(options.libs.begin(), options.libs.end(), std::back_inserter(ldArgs));

    int linkerRun = qlow::invokeProgram(linkerPath, ldArgs);

    if (linkerRun != 0) {
        reportError("linker error");
        errorOccurred = true;
    }
    return errorOccurred;
}








