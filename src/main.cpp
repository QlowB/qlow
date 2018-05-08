#include <iostream>
#include "Ast.h"
#include "Semantic.h"

extern std::unique_ptr<std::vector<std::unique_ptr<qlow::ast::Class>>> parsedClasses;
extern FILE* qlow_parser_in;
extern int qlow_parser_parse(void);

int main(int argc, char** argv)
{
    try {
        qlow_parser_in = stdin;
        
        if (argc > 1)
            qlow_parser_in = ::fopen(argv[1], "r");
        
        ::qlow_parser_parse();
        std::cout << parsedClasses->size() << std::endl;

        std::cout << "parsing completed!" << std::endl;

        qlow::sem::SymbolTable<qlow::sem::Class> semClasses = qlow::sem::createFromAst(*parsedClasses.get());

        for (auto& [a, b] : semClasses) {
            std::cout << a << ": " << b->toString() << std::endl;
        }
    }
    catch (qlow::sem::SemanticException& se)
    {
        std::cerr << se.getMessage() << std::endl;
    }
    catch (const char* err)
    {
        std::cerr << err << std::endl;
    }
    catch (...)
    {
    }
}


