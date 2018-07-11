#include <iostream>

#include <unistd.h>

#include "Ast.h"
#include "Semantic.h"
#include "CodeGeneration.h"

#include "Driver.h"


extern std::unique_ptr<std::vector<std::unique_ptr<qlow::ast::Class>>> parsedClasses;
extern FILE* qlow_parser_in;
extern int qlow_parser_parse(void);


int main(int argc, char** argv)
{
    /*int c;
    while ((c = getopt(argc, argv, "c:")) != -1) {
        switch (c) {
            case 'c':
                printf("c: %s", optarg);
            break;
            default:
                printf("ay: %c\n", c);
        }
    }*/
    
    qlow::Driver driver(argc, argv);
    return driver.run();

    return 0;
    /*
    {
    const char* filename = argv[optind];
    
    try {
        ::qlow_parser_in = stdin;
        
        ::qlow_parser_in = fopen(filename, "r");
        if (!::qlow_parser_in)
            throw (std::string("File not found: ") + filename).c_str();
        
        ::qlow_parser_parse();
        std::cout << parsedClasses->size() << std::endl;

        std::cout << "parsing completed!" << std::endl;

        std::unique_ptr<qlow::sem::GlobalScope> semClasses =
            qlow::sem::createFromAst(*parsedClasses.get());

        for (auto& [a, b] : semClasses->classes) {
            std::cout << a << ": " << b->toString() << std::endl;
        }

        auto main = semClasses->classes.find("Main");
        qlow::sem::Class* mainClass = nullptr;
        if (main == semClasses->classes.end()) {
            throw "No Main class found!";
        }
        else {
            mainClass = main->second.get();
        }
        auto mainmain = mainClass->methods.find("main");
        qlow::sem::Method* mainMethod = nullptr;
        if (mainmain == mainClass->methods.end()) {
            //throw "No main method found inside Main class!";
        }
        else {
            mainMethod = mainmain->second.get();
        }
        
        std::cout << "starting code generation!" << std::endl;

        auto mod = qlow::gen::generateModule(semClasses->classes);
        qlow::gen::generateObjectFile("obj.o", std::move(mod));
        
        std::cout << "object exported!" << std::endl;
    }
    catch (qlow::sem::SemanticException& se)
    {
        std::cerr << se.getMessage() << std::endl;
    }
    catch (const std::string& err)
    {
        std::cerr << err << std::endl;
    }
    catch (const char* err)
    {
        std::cerr << err << std::endl;
    }
    catch (...)
    {
        std::cerr << "an unknown error occurred" << std::endl;
    }
    
    if (::qlow_parser_in != stdin)
        fclose(::qlow_parser_in);
    }

    for (auto&& c : *parsedClasses) {
        delete c.release();
    }*/
}


