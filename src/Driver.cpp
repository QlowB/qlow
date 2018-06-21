#include "Driver.h"

#include "Ast.h"
#include "Semantic.h"
#include "CodeGeneration.h"

#include "Logging.h"

extern std::unique_ptr<std::vector<std::unique_ptr<qlow::ast::Class>>> parsedClasses;
extern FILE* qlow_parser_in;
extern int qlow_parser_parse(void);

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


void Driver::run(void)
{
    Logger& logger = Logger::getInstance();
    
    //logger.logError("driver not yet implemented", {options.emitAssembly ? "asm" : "noasm", 10, 11, 12, 13});
    
    std::vector<std::unique_ptr<qlow::ast::Class>> classes;
    for (auto& filename : options.infiles) {
        FILE* file = ::fopen(filename.c_str(), "r");
        
        try {
            classes = parseFile(file);
        } catch (const char* errMsg) {
            logger.logError(errMsg);
        }
        
        fclose(file);
    }
    
    return;
}


std::vector<std::unique_ptr<qlow::ast::Class>> Driver::parseFile(FILE* file)
{
    ::qlow_parser_in = file;
    if (!::qlow_parser_in)
        throw "Could not run parser: Invalid file";
    
    ::qlow_parser_parse();
    
    auto retval = std::move(*parsedClasses);
    parsedClasses.reset();
    return retval;
}











