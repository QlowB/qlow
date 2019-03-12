#include "Parser.h"
#include "syntax.hpp"
#include "lexer.h"
using qlow::ast::Parser;

qlow::ast::Ast Parser::parse(void)
{
    qlow::ast::Ast result;
    yyscan_t scanner;
    qlow_parser_lex_init(&scanner);
    qlow_parser_set_in(this->stream, scanner);
    auto error = qlow_parser_parse(scanner, result, *this);
    qlow_parser_lex_destroy(scanner);
    return result;
}
