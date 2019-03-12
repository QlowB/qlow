#include <iostream>

#include <unistd.h>

#include "Ast.h"
#include "Semantic.h"
#include "Builtin.h"
#include "CodeGeneration.h"

#include "Driver.h"

int main(int argc, char** argv)
{
    std::cout << "Hi!";
    qlow::Driver driver(argc, argv);
    return driver.run();

    return 0;
}


