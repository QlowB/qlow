#ifndef QLOW_PARSER_H
#define QLOW_PARSER_H

#include <cstdio>
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
public:
    inline Parser(FILE* stream) :
        stream{ stream } {}
    
    std::unique_ptr<std::vector<std::unique_ptr<qlow::ast::AstObject>>> parse(void);
};

#endif // QLOW_PARSER_H
