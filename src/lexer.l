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

%{
#include "Ast.h"
#include "parser.hpp"


#define yylval qlow_parser_lval
#define SET_TOKEN(t) (yylval.token = t)
#define SET_STRING (yylval.string = new std::string(yytext, yyleng))

extern "C" int yywrap()
{
    return 1; /* do not continue on EOF */
}
/*  */
int commentDepth;

size_t offset;
extern QLOW_PARSER_LTYPE qlow_parser_lloc;
extern const char* qlow_parser_filename;

#define YY_USER_ACTION                          \
  do {                                          \
    qlow_parser_lloc.first_line = yylineno;     \
    qlow_parser_lloc.first_column = offset;     \
    offset += yyleng;                           \
    qlow_parser_lloc.last_line = yylineno;      \
    qlow_parser_lloc.last_column = offset;      \
    qlow_parser_lloc.filename = qlow_parser_filename; \
  } while(0);

%}


%option prefix="qlow_parser_"
%option yylineno
%option stack

%x COMMENT
%x LINE_COMMENT
%x STRING
%s METHOD

UTF8CHAR [\x00-\x7f]|[\xc2-\xdf][\x80-\xbf]|\xe0[\xa0-\xbf][\x80-\xbf]|[\xe1-\xec][\x80-\xbf][\x80-\xbf]|\xed[\x80-\x9f][\x80-\xbf]|[\xee\xef][\x80-\xbf][\x80-\xbf]|\xf0[\x90-\xbf][\x80-\xbf][\x80-\xbf]|[\xf1-\xf3][\x80-\xbf][\x80-\xbf][\x80-\xbf]|\xf4[\x80-\x8f][\x80-\xbf][\x80-\xbf]

/*UTF8CHAR [a-zA-Z0-9_]
*/

%%

<COMMENT>"/*"           commentDepth++;
<COMMENT>"*/"           if ((--commentDepth) == 0) { BEGIN(INITIAL); };
<COMMENT>\n             ;
<COMMENT>.              ; // inside comment, ignore everything

<LINE_COMMENT>\n        yy_pop_state(); //yy_push_state(INITIAL);
<LINE_COMMENT>.         ; // inside comment, ignore everything

<STRING>"\""            yy_pop_state();
<STRING>[^\"^\n]*          printf("%s\n", std::string(yytext, yyleng).c_str());
<STRING>\n              SET_STRING; return UNEXPECTED_SYMBOL; 

"/*"                    yy_push_state(COMMENT); commentDepth = 1;
"//"                    yy_push_state(LINE_COMMENT);
"\""                    yy_push_state(STRING);


[\t ]                   ; // Space or tab ignored
<METHOD>\n              offset = 0; return SET_TOKEN(NEW_LINE);
\n                      offset = 0; //return SET_TOKEN(NEW_LINE);

"class"                 return SET_TOKEN(CLASS);
"do"                    yy_push_state(METHOD); return SET_TOKEN(DO);
<METHOD>"end"           yy_pop_state(); return SET_TOKEN(END);
<INITIAL>"end"          return SET_TOKEN(END);
"if"                    return SET_TOKEN(IF);
"while"                 return SET_TOKEN(WHILE);
"else"                  return SET_TOKEN(ELSE);
"return"                return SET_TOKEN(RETURN);
"new"                   return SET_TOKEN(NEW);

":"                     return SET_TOKEN(COLON);
";"                     return SET_TOKEN(SEMICOLON);
","                     return SET_TOKEN(COMMA);
":="                    return SET_TOKEN(ASSIGN);
"."                     return SET_TOKEN(DOT);

"=="                    return SET_TOKEN(EQUALS);
"!="                    return SET_TOKEN(NOT_EQUALS);
"and"                   return SET_TOKEN(AND);
"or"                    return SET_TOKEN(OR);
"xor"                   return SET_TOKEN(XOR);
"not"                   return SET_TOKEN(NOT);

"+"                     return SET_TOKEN(PLUS);
"-"                     return SET_TOKEN(MINUS);
"*"                     return SET_TOKEN(ASTERISK);
"/"                     return SET_TOKEN(SLASH);

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
