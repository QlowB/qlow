#ifndef QLOW_DRIVER_H
#define QLOW_DRIVER_H

#include <vector>
#include <optional>
#include <memory>
#include <string>
#include <utility>

#include "Path.h"

#include "Parser.h"
#include "Scope.h"

namespace qlow
{
    struct Options;
    
    class Driver;
    
    
    namespace ast
    {
        class Ast;
        struct AstObject;
    }
}


struct qlow::Options
{
    bool emitAssembly;
    bool emitLlvm;
    std::string outfile = "a.out";
    std::vector<std::string> infiles;
    std::vector<std::string> libs;
    
    int optLevel = 0;
    
    static Options parseOptions(int argc, char** argv);
};


class qlow::Driver
{
    Options options;
    std::unique_ptr<ast::Ast> ast = nullptr;
    std::unique_ptr<sem::Context> context = nullptr;
    std::unique_ptr<sem::GlobalScope> semClasses = nullptr;

    qlow::util::Path tempObject = "";
public:
    Driver(void) = delete;
    Driver(int argc, char** argv);
    
    int run(void);

    bool parseStage(void);
    bool semanticStage(void);
    bool linkingStage(void);
};


#endif // QLOW_DRIVER_H

