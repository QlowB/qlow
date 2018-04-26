#include <iostream>
#include "Ast.h"
#include "Semantic.h"

extern std::unique_ptr<std::vector<std::unique_ptr<qlow::ast::Class>>> parsedClasses;
extern FILE* qlow_parser_in;
extern int qlow_parser_parse(void);

int main()
{
    try {
        qlow_parser_in = stdin;
        ::qlow_parser_parse();
        std::cout << parsedClasses->size() << std::endl;

        qlow::sem::createFromAst(*parsedClasses.get());
    }
    catch (const char* err)
    {
        std::cerr << err << std::endl;
    }
    catch (...)
    {
    }
}


