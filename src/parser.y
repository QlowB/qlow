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

#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include "Ast.h"
using namespace qlow::parser;

extern int qlow_parser_lex();

int qlow_parser_error(const char*)
{
    throw "syntax error";
}

using ClassList = std::vector<std::unique_ptr<qlow::ast::Class>>;
std::unique_ptr<ClassList> parsedClasses;

%}

%define api.prefix {uetli_parser_}

/*
%skeleton "lalr1.cc" // generate C++ parser
//%define api.namespace {uetli::parser}
//%define api.value.type {struct semantic_type}
//%define parser_class_name {Parser}

//%name-prefix "uetli_parser_"
*/
%union {
    std::vector<std::unique_ptr<uetli::parser::Class>>* classes;
    ClassDefinition* classDefinition;
    FeatureDeclaration* featureDeclaration;
    std::vector<std::unique_ptr<uetli::parser::FeatureDeclaration>>* featureList;

    const char* cString;
    int token;
};


%token <string> IDENTIFIER
%token <token> CLASS DO END IF
%token <token> NEW_LINE
%token <token> COLON COMMA DOT ASSIGN OPERATOR
%token <token> ROUND_LEFT ROUND_RIGHT

%type <classes> classes
%type <classDefinition> classDefinition
%type <featureDeclaration> featureDeclaration fieldDeclaration methodDeclaration
%type <featureList> featureList

%left ASTERISK SLASH
%left PLUS MINUS

%start compilationUnit

%%


compilationUnit:
    /* empty */ {
        parsedClasses = std::make_unique<ClassList>();
    }
    |
    compilationUnit classes {
        parsedClasses = $2;
    };


/* possible newline characters
pnl:
    {
    }
    |
    pnl NEW_LINE {
    };
*/

/* list of class definitions */
classes:
    classDefinition {
        $$ = new std::vector<std::unique_ptr<ClassDefinition>>();
        $$->push_back($1);
    }
    |
    classes classDefinition {
        $$->push_back($3);
    };


classDefinition:
    CLASS IDENTIFIER featureList END {
        $$ = new ClassDefinition(*$2, *$3);
        delete $2; $2 = 0; $3 = 0;
    };


featureList:
    /* empty */ {
        $$ = new std::vector<std::unique_ptr<FeatureDeclaration>>();
    }
    |
    featureList featureDeclaration {
        $$ = $1;
        $$->push_back($2);
    };


featureDeclaration:
    fieldDeclaration {
        $$ = $1;
    }
    |
    methodDeclaration {
        $$ = $1;
    };


fieldDeclaration:
    IDENTIFIER COLON IDENTIFIER {
        $$ = new FieldDeclaration(*$3, *$1);
        delete $3; delete $1; $1 = $3 = 0;
    };


methodDefinition:
    IDENTIFIER COLON IDENTIFIER doEndBlock {
        $$ = new MethodDefinition(*$3, *$1, $4);
        delete $3; delete $1; $1 = $3 = 0;
    }
    |
    IDENTIFIER doEndBlock {
        $$ = new MethodDefinition("", *$1, $2);
        delete $1; $1 = 0;
    }
    |
    IDENTIFIER
        ROUND_LEFT argumentList ROUND_RIGHT COLON IDENTIFIER doEndBlock {
        $$ = new MethodDefinition(*$6, *$1, $7);
        delete $6; delete $1; $1 = $6 = 0;
    }
    |
    IDENTIFIER ROUND_LEFT argumentList ROUND_RIGHT doEndBlock {
        $$ = new MethodDefinition("", *$1, $5);
        delete $1; $1 = 0;
    };


argumentList:
    argumentDeclaration {
        $$ = new std::vector<std::unique_ptr<ArgumentDeclaration>>();
        $$->push_back($1);
    }
    |
    argumentList COMMA argumentDeclaration {
        $$ = $1;
        $$->push_back($3);
    };


argumentDeclaration:
    IDENTIFIER COLON IDENTIFIER {
        $$ = new ArgumentDeclaration(*$3, *$1);
        delete $3; delete $1; $1 = $3 = 0;
    };


doEndBlock:
    DO statements END {
        $$ = new DoEndBlock(*$2);
        delete $2; $2 = 0;
    };


statements:
    /* empty */ {
        $$ = new std::vector<std::unique_ptr<Statement>>();
    }
    |
    statements statement {
        $$ = $1;
        $$->push_back($2);
    };


statement:
    callOrVariableStatement {
        $$ = $1;
    }
    |
    assignmentStatement {
        $$ = $1;
    }
    |
    newVariableStatement {
        $$ = $1;
    };


callOrVariableStatement:
    IDENTIFIER {
        $$ = new CallOrVariableStatement(0, *$1);
        delete $1; $1 = 0;
    }
    |
    IDENTIFIER ROUND_LEFT expressionList ROUND_RIGHT {
        $$ = new CallOrVariableStatement(0, *$1, *$3);
        delete $1; delete $3; $1 = 0; $3 = 0;
    }
    |
    expression DOT IDENTIFIER {
        $$ = new CallOrVariableStatement($1, *$3);
        delete $3; $3 = 0;
    }
    |
    expression DOT IDENTIFIER ROUND_LEFT expressionList ROUND_RIGHT {
        $$ = new CallOrVariableStatement($1, *$3, *$5);
        delete $3; $3 = 0; delete $5; $5 = 0;
    };


/* list of effective arguments */
expressionList:
    expression {
        $$ = new std::vector<std::unique_ptr<Expression>>();
        $$->push_back($1);
    }
    |
    expressionList COMMA expression {
        $$ = $1;
        $$->push_back($3);
    };


expression:
    callOrVariableStatement {
        $$ = $1;
    }
    |
    operationExpression {
        $$ = $1;
    }
    |
    paranthesesExpression {
        $$ = $1;
    };


operationExpression:
    binaryOperationExpression {
        $$ = $1;
    }
    |
    unaryOperationExpression {
        $$ = $1;
    };


binaryOperationExpression:
    expression operator expression {
        $$ = new BinaryOperationExpression($1, $3, $2);
    };


unaryOperationExpression:
    expression operator {
        $$ = new UnaryOperationExpression($1,
            UnaryOperationExpression::SUFFIX, $2);
    }
    |
    operator expression {
        $$ = new UnaryOperationExpression($2,
            UnaryOperationExpression::PREFIX, $1);
    };


operator:
    PLUS { $$ = "+"; }
    |
    MINUS { $$ = "-"; }
    |
    ASTERISK { $$ = "*"; }
    |
    SLASH { $$ = "/"; };


paranthesesExpression:
    ROUND_LEFT expression ROUND_RIGHT {
        $$ = $2;
    };


assignmentStatement:
    callOrVariableStatement ASSIGN expression {
        $$ = new AssignmentStatement($1, $3);
    };


newVariableStatement:
    IDENTIFIER COLON IDENTIFIER {
        $$ = new NewVariableStatement(*$3, *$1);
        delete $3; delete $1; $3 = 0; $1 = 0;
    };


%%


