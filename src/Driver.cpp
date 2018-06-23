#include "Driver.h"

#include "Ast.h"
#include "Semantic.h"
#include "CodeGeneration.h"

#include "Logging.h"

#include <cstdio>

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
        else {
            options.infiles.push_back(std::move(arg));
        }
    }
    return options;
}


Driver::Driver(int argc, char** argv) :
    options{ Options::parseOptions(argc, argv) }
{
}


int Driver::run(void)
{
    Logger& logger = Logger::getInstance();
    
    //logger.logError("driver not yet implemented", {options.emitAssembly ? "asm" : "noasm", 10, 11, 12, 13});
    
    std::vector<std::unique_ptr<qlow::ast::AstObject>> objects;
    for (auto& filename : options.infiles) {
        std::FILE* file = std::fopen(filename.c_str(), "r");
        ::qlow_parser_filename = filename.c_str();
        
        try {
            auto newObjects = parseFile(file);
            objects.insert(objects.end(),
                           std::make_move_iterator(newObjects.begin()),
                           std::make_move_iterator(newObjects.end()));
        }
        catch (const CompileError& ce) {
            ce.print(logger);
            return 1;
        }
        catch (const char* errMsg) {
            logger.logError(errMsg);
            return 1;
        }
        
        if (file)
            std::fclose(file);
    }
    
    
    std::unique_ptr<qlow::sem::GlobalScope> semClasses = nullptr;
    try {
        semClasses =
            qlow::sem::createFromAst(objects);
    }
    catch(SemanticError& se) {
        se.print(logger);
        return 1;
    }
    catch(const char* err) {
        logger.logError(err);
    }

    for (auto& [a, b] : semClasses->classes) {
        logger.debug() << a << ": " << b->toString() << std::endl;
    }

    auto main = semClasses->classes.find("Main");
    qlow::sem::Class* mainClass = nullptr;
    if (main == semClasses->classes.end()) {
        logger.logError("No Main class found");
        return 1;
    }
    else {
        mainClass = main->second.get();
    }
    auto mainmain = mainClass->methods.find("main");
    qlow::sem::Method* mainMethod = nullptr;
    if (mainmain == mainClass->methods.end()) {
        //throw "No main method found inside Main class!";
    }
    else {
        mainMethod = mainmain->second.get();
    }
    
    logger.debug() << "starting code generation!" << std::endl;

    std::unique_ptr<llvm::Module> mod = nullptr;
    
    try {
        mod = qlow::gen::generateModule(semClasses->classes);
    }
    catch (const char* err) {
        logger.logError(err);
    }
    qlow::gen::generateObjectFile("obj.o", std::move(mod));
    
    logger.debug() << "object exported!" << std::endl;
    
    return 0;
}


std::vector<std::unique_ptr<qlow::ast::AstObject>> Driver::parseFile(FILE* file)
{
    ::qlow_parser_in = file;
    if (!::qlow_parser_in)
        throw "Could not run parser: Invalid file";
    
    ::qlow_parser_parse();
    
    auto retval = std::move(*parsedClasses);
    parsedClasses.reset();
    return retval;
}











