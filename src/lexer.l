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

%}


%option prefix="qlow_parser_"

%%

[\t ]                   ; // Space or tab ignored
\/\*[.\n]*\*\/          ; // comment
\n                      ; //return SET_TOKEN(NEW_LINE);

"class"                 return SET_TOKEN(CLASS);
"do"                    return SET_TOKEN(DO);
"end"                   return SET_TOKEN(END);
"if"                    return SET_TOKEN(IF);

":"                     return SET_TOKEN(COLON);
","                     return SET_TOKEN(COMMA);
":="                    return SET_TOKEN(ASSIGN);
"."                     return SET_TOKEN(DOT);

"("                     return SET_TOKEN(ROUND_LEFT);
")"                     return SET_TOKEN(ROUND_RIGHT);

"+"		        return SET_TOKEN(PLUS);
"-"		        return SET_TOKEN(MINUS);
"*"		        return SET_TOKEN(ASTERISK);
"/"		        return SET_TOKEN(SLASH);


[a-zA-Z_][a-zA-Z0-9_]*  SET_STRING; return IDENTIFIER;
.                       printf("Unknown token!\n"); yyterminate();

%%


/*

[\t ]                   ; // Space or tab ignored
\/\*[.\n]*\*\/          ; // comment
\n                      return SET_TOKEN(NEW_LINE);

":"                     return SET_TOKEN(COLON);
","                     return SET_TOKEN(COMMA);
":="                    return SET_TOKEN(ASSIGN);
"."                     return SET_TOKEN(DOT);

"+"		        return SET_TOKEN(PLUS);
"-"		        return SET_TOKEN(MINUS);
"*"		        return SET_TOKEN(ASTERISK);
"/"		        return SET_TOKEN(SLASH);

"("                     return SET_TOKEN(ROUND_LEFT);
")"                     return SET_TOKEN(ROUND_RIGHT);

"class"                 return SET_TOKEN(CLASS);
"do"                    return SET_TOKEN(DO);
"end"                   return SET_TOKEN(END);
"if"                    return SET_TOKEN(IF);
*/
