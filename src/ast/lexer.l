/* =============================================================================
//
// This file is part of the qlow compiler.
//
// Copyright (C) 2014-2015 Nicolas Winkler
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// ===========================================================================*/

%option reentrant bison-bridge bison-locations
%option prefix="qlow_parser_"
%option yylineno nounput noinput
%option stack

%option 8bit
%option header-file="lexer.h"

%{
#include "Parser.h"
#include "syntax.hpp"


#define yylval qlow_parser_lval
#define YY_EXTRA_TYPE size_t
#define SET_TOKEN(t) (yylval_param->token = t)
#define SET_STRING (yylval_param->string = new std::string(yytext, yyleng))

extern "C" int qlow_parser_wrap(yyscan_t s);

// TODO rewrite
#define YY_USER_ACTION                          \
  do {                                          \
    yylloc_param->first_line = yylineno;     \
    yylloc_param->first_column = yyg->yyextra_r;     \
    yyg->yyextra_r += yyleng;                           \
    yylloc_param->last_line = yylineno;      \
    yylloc_param->last_column = yyg->yyextra_r;      \
    yylloc_param->filename = nullptr; \
  } while(0);
#undef YY_USER_ACTION
#define YY_USER_ACTION ;
%}


%x COMMENT
%x LINE_COMMENT
%x STRING
%s METHOD

UTF8CHAR [\x00-\x7f]|[\xc2-\xdf][\x80-\xbf]|\xe0[\xa0-\xbf][\x80-\xbf]|[\xe1-\xec][\x80-\xbf][\x80-\xbf]|\xed[\x80-\x9f][\x80-\xbf]|[\xee\xef][\x80-\xbf][\x80-\xbf]|\xf0[\x90-\xbf][\x80-\xbf][\x80-\xbf]|[\xf1-\xf3][\x80-\xbf][\x80-\xbf][\x80-\xbf]|\xf4[\x80-\x8f][\x80-\xbf][\x80-\xbf]

/*UTF8CHAR [a-zA-Z0-9_]
*/

%%

    int commentDepth = 0;

<COMMENT>"/*"           commentDepth++;
<COMMENT>"*/"           if ((--commentDepth) == 0) { BEGIN(INITIAL); };
<COMMENT>\n             yyg->yyextra_r = 0;
<COMMENT>.              ; // inside comment, ignore everything

<LINE_COMMENT>\n        yyg->yyextra_r = 0; yy_pop_state(yyscanner); //yy_push_state(INITIAL);
<LINE_COMMENT>.         ; // inside comment, ignore everything

<STRING>"\""            yy_pop_state(yyscanner);
<STRING>[^\"^\n]*          printf("%s\n", std::string(yytext, yyleng).c_str());
<STRING>\n              yyg->yyextra_r = 0; SET_STRING; return UNEXPECTED_SYMBOL; 

"/*"                    yy_push_state(COMMENT, yyscanner); commentDepth = 1;
"//"                    yy_push_state(LINE_COMMENT, yyscanner);
"\""                    yy_push_state(STRING, yyscanner);


[\t ]                   ; // Space or tab ignored
<METHOD>\n              yyg->yyextra_r = 0; return SET_TOKEN(NEW_LINE);
\n                      yyg->yyextra_r = 0; //return SET_TOKEN(NEW_LINE);

"class"                 return SET_TOKEN(CLASS);
"do"                    yy_push_state(METHOD, yyscanner); return SET_TOKEN(DO);
<METHOD>"end"           yy_pop_state(yyscanner); return SET_TOKEN(END);
<INITIAL>"end"          return SET_TOKEN(END);
"if"                    return SET_TOKEN(IF);
"while"                 return SET_TOKEN(WHILE);
"else"                  return SET_TOKEN(ELSE);
"return"                return SET_TOKEN(RETURN);
"new"                   return SET_TOKEN(NEW);
"extern"                return SET_TOKEN(EXTERN);

":"                     return SET_TOKEN(COLON);
";"                     return SET_TOKEN(SEMICOLON);
","                     return SET_TOKEN(COMMA);
"."                     return SET_TOKEN(DOT);
"&"                     return SET_TOKEN(AMPERSAND);

":="                    SET_STRING; return ASSIGN;

"=="                    SET_STRING; return EQUALS;
"!="                    SET_STRING; return NOT_EQUALS;
"and"                   SET_STRING; return AND;
"or"                    SET_STRING; return OR;
"xor"                   SET_STRING; return XOR;
"not"                   SET_STRING; return NOT;
"as"                    return SET_TOKEN(AS);

"+"                     SET_STRING; return PLUS;
"-"                     SET_STRING; return MINUS;
"*"                     SET_STRING; return ASTERISK;
"/"                     SET_STRING; return SLASH;
[\+\-\*\/=!<>]+         SET_STRING; return CUSTOM_OPERATOR;

"("                     return SET_TOKEN(ROUND_LEFT);
")"                     return SET_TOKEN(ROUND_RIGHT);
"["                     return SET_TOKEN(SQUARE_LEFT);
"]"                     return SET_TOKEN(SQUARE_RIGHT);

"false"                 return SET_TOKEN(FALSE);
"true"                  return SET_TOKEN(TRUE);

[0-9_]+                 SET_STRING; return INT_LITERAL;
0x[0-9A-Fa-f]+          SET_STRING; return INT_LITERAL;
[a-zA-Z_][a-zA-Z0-9_]*  SET_STRING; return IDENTIFIER;

.                       SET_STRING; return UNEXPECTED_SYMBOL; // printf("Unexpected symbol %s.\n", std::string(yytext, yyleng).c_str()); yyterminate();

%%

// [a-zA-Z_][a-zA-Z0-9_]*  SET_STRING; return IDENTIFIER;


/*
QLOW_PARSER_LTYPE qlow_parser_lloc;
const char* qlow_parser_filename;
QLOW_PARSER_STYPE qlow_parser_lval;

int main() {
    while (true) {
        yylex();
        auto& qpl = qlow_parser_lval;
        if (qpl.token > 0 && qpl.token < 1000) {
            printf("token %d\n", qpl.token);
        }
        else {
            printf("string %s\n", qpl.string->c_str());
        }
    }
}
*/
