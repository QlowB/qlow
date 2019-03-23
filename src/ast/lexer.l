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
%option extra-type="std::string"

%{


#include "Parser.h"
#include "syntax.hpp"

#define register

#define CREATE_TOKEN(t) (yylval_param->token = t)
#define CREATE_STRING (yylval_param->string = new std::string(yytext, yyleng))

extern "C" int qlow_parser_wrap(yyscan_t s);

#ifdef _WIN32
#define YY_NO_UNISTD_H
#endif

#define YY_USER_ACTION                                \
  do {                                                \
    yylloc_param->first_line = yylineno;              \
    yylloc_param->first_column = yycolumn;            \
    yycolumn += yyleng;                               \
    yylloc_param->last_line = yylineno;               \
    yylloc_param->last_column = yycolumn;             \
    yylloc_param->filename = yyextra;                 \
  } while(0);
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
<COMMENT>"*/"           if ((--commentDepth) <= 0) { yy_pop_state(yyscanner); };
<COMMENT>\n             yycolumn = 0;
<COMMENT>.              ; // inside comment, ignore everything

<LINE_COMMENT>"\n"      yycolumn = 0; yy_pop_state(yyscanner); return CREATE_TOKEN(NEW_LINE); //yy_push_state(INITIAL);
<LINE_COMMENT>.         ; // inside comment, ignore everything

<STRING>"\""            yy_pop_state(yyscanner);
<STRING>[^\"^\n]*          printf("%s\n", std::string(yytext, yyleng).c_str());
<STRING>\n              yycolumn = 0; CREATE_STRING; return UNEXPECTED_SYMBOL; 

"/*"                    yy_push_state(COMMENT, yyscanner); commentDepth = 1;
"//"                    yy_push_state(LINE_COMMENT, yyscanner);
"\""                    yy_push_state(STRING, yyscanner);


[\t ]                   ; // Space or tab ignored
<METHOD>\n              yycolumn = 0; return CREATE_TOKEN(NEW_LINE);
\n                      yycolumn = 0; //return CREATE_TOKEN(NEW_LINE);

"class"                 return CREATE_TOKEN(CLASS);
"struct"                return CREATE_TOKEN(STRUCT);
"do"                    yy_push_state(METHOD, yyscanner); return CREATE_TOKEN(DO);
<METHOD>"end"           yy_pop_state(yyscanner); return CREATE_TOKEN(END);
<INITIAL>"end"          return CREATE_TOKEN(END);
"if"                    return CREATE_TOKEN(IF);
"while"                 return CREATE_TOKEN(WHILE);
"else"                  return CREATE_TOKEN(ELSE);
"return"                return CREATE_TOKEN(RETURN);
"new"                   return CREATE_TOKEN(NEW);
"extern"                return CREATE_TOKEN(EXTERN);
"import"                return CREATE_TOKEN(IMPORT);

":"                     return CREATE_TOKEN(COLON);
";"                     return CREATE_TOKEN(SEMICOLON);
","                     return CREATE_TOKEN(COMMA);
"."                     return CREATE_TOKEN(DOT);
"&"                     return CREATE_TOKEN(AMPERSAND);

":="                    CREATE_STRING; return ASSIGN;

"=="                    CREATE_STRING; return EQUALS;
"!="                    CREATE_STRING; return NOT_EQUALS;
"and"                   CREATE_STRING; return AND;
"or"                    CREATE_STRING; return OR;
"xor"                   CREATE_STRING; return XOR;
"not"                   CREATE_STRING; return NOT;
"as"                    return CREATE_TOKEN(AS);

"+"                     CREATE_STRING; return PLUS;
"-"                     CREATE_STRING; return MINUS;
"*"                     CREATE_STRING; return ASTERISK;
"/"                     CREATE_STRING; return SLASH;
[\+\-\*\/=!<>]+         CREATE_STRING; return CUSTOM_OPERATOR;

"("                     return CREATE_TOKEN(ROUND_LEFT);
")"                     return CREATE_TOKEN(ROUND_RIGHT);
"["                     return CREATE_TOKEN(SQUARE_LEFT);
"]"                     return CREATE_TOKEN(SQUARE_RIGHT);

"false"                 return CREATE_TOKEN(FALSE);
"true"                  return CREATE_TOKEN(TRUE);

[0-9_]+                 CREATE_STRING; return INT_LITERAL;
0x[0-9A-Fa-f]+          CREATE_STRING; return INT_LITERAL;
[a-zA-Z_][a-zA-Z0-9_]*  CREATE_STRING; return IDENTIFIER;

.                       CREATE_STRING; return UNEXPECTED_SYMBOL; // printf("Unexpected symbol %s.\n", std::string(yytext, yyleng).c_str()); yyterminate();

%%

// [a-zA-Z_][a-zA-Z0-9_]*  CREATE_STRING; return IDENTIFIER;


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
