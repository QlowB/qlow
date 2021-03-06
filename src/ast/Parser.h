#ifndef QLOW_AST_PARSER_H
#define QLOW_AST_PARSER_H

#include <cstdio>
#include <string>
#include "Path.h"
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
    qlow::util::Path filename;
    std::vector<std::unique_ptr<ImportDeclaration>> imports;
public:
    inline Parser(FILE* stream, qlow::util::Path filename) :
        stream{ stream }, filename{ std::move(filename) } {}
    
    Ast parse(void);

    void addImports(std::vector<std::unique_ptr<ImportDeclaration>> toAdd);
    const std::vector<std::unique_ptr<ImportDeclaration>>& getImports(void) const;

    inline std::string getFilename(void) const { return filename; }
};

#endif // QLOW_AST_PARSER_H

