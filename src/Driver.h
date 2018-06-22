#ifndef QLOW_DRIVER_H
#define QLOW_DRIVER_H

#include <vector>
#include <optional>
#include <memory>
#include <string>
#include <utility>

namespace qlow
{
    struct Options;
    
    class Driver;
    
    
    namespace ast {
        struct Class;
    }
}


struct qlow::Options
{
    bool emitAssembly;
    bool emitLlvm;
    std::string outfile = "a.out";
    std::vector<std::string> infiles;
    
    static Options parseOptions(int argc, char** argv);
};


class qlow::Driver
{
    Options options;
public:
    Driver(void) = delete;
    Driver(int argc, char** argv);
    
    int run(void);
    
    /// \brief runs the parser over a given stream
    /// \warning Don't call concurrently. Not supported!
    std::vector<std::unique_ptr<qlow::ast::Class>> parseFile(FILE* file);
};


#endif // QLOW_DRIVER_H
