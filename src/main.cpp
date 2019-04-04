#include <iostream>

#include <exception>
#include <cstdlib>

#include "Ast.h"
#include "Semantic.h"
#include "ErrorReporting.h"
#include "Builtin.h"
#include "CodeGeneration.h"

#include "Driver.h"
#include "Path.h"

int main(int argc, char** argv) try
{
#ifndef DEBUGGING
    std::set_terminate ([] () {
        qlow::printError(qlow::Printer::getInstance(), "severe internal compiler error");
        exit(1);
    });
#endif

    qlow::Driver driver(argc, argv);
    return driver.run();

    return 0;
}
catch (float f) {
    std::cerr << "uncaught float" << std::endl;
}
catch (const std::bad_alloc& ba) {
    std::cerr << "out of memory" << std::endl;
}
/*catch(...) {
    std::cerr << "uncaught exception" << std::endl;
}*/


