#include <iostream>

#include <unistd.h>

#include "Ast.h"
#include "Semantic.h"

extern std::unique_ptr<std::vector<std::unique_ptr<qlow::ast::Class>>> parsedClasses;
extern FILE* qlow_parser_in;
extern int qlow_parser_parse(void);


int main(int argc, char** argv)
{
    int c;
    while ((c = getopt(argc, argv, "c:")) != -1) {
        switch (c) {
            case 'c':
                printf("c: %s", optarg);
            break;
            default:
                printf("ay: %c\n", c);
        }
    }
    
    const char* filename = argv[optind];
    
    try {
        ::qlow_parser_in = stdin;
        
        ::qlow_parser_in = fopen(filename, "r");
        if (!::qlow_parser_in)
            throw (std::string("File not found: ") + filename).c_str();
        
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
    
    if (::qlow_parser_in != stdin)
        fclose(::qlow_parser_in);
}


