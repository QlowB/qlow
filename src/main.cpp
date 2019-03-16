#include <iostream>

#include <exception>
#include <cstdlib>

#include "Ast.h"
#include "Semantic.h"
#include "ErrorReporting.h"
#include "Builtin.h"
#include "CodeGeneration.h"

#include "Driver.h"

int main(int argc, char** argv) try
{
    std::set_terminate ([] () {
        qlow::printError(qlow::Printer::getInstance(), "severe internal compiler error");
        exit(1);
    });

    qlow::Driver driver(argc, argv);
    return driver.run();

    return 0;
}
catch (float f) {
    std::cerr << "uncaught float" << std::endl;
}
catch(...) {
    std::cerr << "uncaught exception" << std::endl;
}


