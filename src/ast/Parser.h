#ifndef QLOW_AST_PARSER_H
#define QLOW_AST_PARSER_H

#include <cstdio>
#include <string>
#include "Ast.h"

namespace qlow
{
    namespace ast
    {
        class Parser;
    }
}

class qlow::ast::Parser
{
    FILE* stream;
    std::string filename;
public:
    inline Parser(FILE* stream, std::string filename) :
        stream{ stream }, filename{ std::move(filename) } {}
    
    Ast parse(void);

    inline const std::string& getFilename(void) const { return filename; }
};

#endif // QLOW_AST_PARSER_H

