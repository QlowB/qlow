#ifndef QLOW_DRIVER_H
#define QLOW_DRIVER_H

#include <vector>
#include <optional>
#include <memory>
#include <string>
#include <utility>
#include "Parser.h"
#include "Scope.h"

namespace qlow
{
    struct Options;
    
    class Driver;
    
    
    namespace ast {
        struct Ast;
        struct AstObject;
    }
}


struct qlow::Options
{
    bool emitAssembly;
    bool emitLlvm;
    std::string outfile = "a.out";
    std::vector<std::string> infiles;
    
    int optLevel = 0;
    
    static Options parseOptions(int argc, char** argv);
};


class qlow::Driver
{
    Options options;
    std::unique_ptr<ast::Ast> ast = nullptr;
    std::unique_ptr<sem::Context> context = nullptr;
    std::unique_ptr<sem::GlobalScope> semClasses = nullptr;
public:
    Driver(void) = delete;
    Driver(int argc, char** argv);
    
    int run(void);

    bool parseStage(void);
    bool semanticStage(void);
    
    qlow::ast::Ast parseFile(FILE* file, const std::string& filename);
};


#endif // QLOW_DRIVER_H

