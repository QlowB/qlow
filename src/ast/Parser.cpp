#include "Parser.h"
#include "syntax.hpp"
#include "lexer.h"
using qlow::ast::Parser;

std::unique_ptr<std::vector<std::unique_ptr<qlow::ast::AstObject>>> Parser::parse(void)
{
    std::unique_ptr<std::vector<std::unique_ptr<qlow::ast::AstObject>>> result =
        std::make_unique<std::vector<std::unique_ptr<qlow::ast::AstObject>>>();
    yyscan_t scanner;
    qlow_parser_lex_init(&scanner);
    auto ret = qlow_parser_parse(scanner, *result);
    qlow_parser_lex_destroy(scanner);
    return result;
}
