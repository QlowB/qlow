#include "Parser.h"
#include "syntax.hpp"
#include "lexer.h"
using qlow::ast::Parser;

qlow::ast::Ast Parser::parse(void)
{
    qlow::ast::Ast result;
    yyscan_t scanner;
    int lexInit = qlow_parser_lex_init(&scanner);

    if (lexInit != 0)
        throw qlow::InternalError(InternalError::PARSER_INIT_FAILED);

    qlow_parser_set_in(this->stream, scanner);
    auto error = qlow_parser_parse(scanner, result, *this);

    if (error != 0)
        throw qlow::InternalError(InternalError::PARSER_FAILED);

    int lexDest = qlow_parser_lex_destroy(scanner);

    if (lexDest != 0)
        throw qlow::InternalError(InternalError::PARSER_DEST_FAILED);

    return result;
}


void Parser::addImports(std::vector<std::unique_ptr<ImportDeclaration>> toAdd)
{
    std::move(toAdd.begin(), toAdd.end(), std::back_inserter(this->imports));
}


const std::vector<std::unique_ptr<qlow::ast::ImportDeclaration>>& Parser::getImports(void) const
{
    return imports;
}
