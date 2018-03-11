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

using namespace qlow::ast;

extern int qlow_parser_lex();

int qlow_parser_error(const char*)
{
    throw "syntax error";
}

using ClassList = std::vector<std::unique_ptr<qlow::ast::Class>>;
std::unique_ptr<ClassList> parsedClasses;

%}

%define api.prefix {qlow_parser_}

/*
%skeleton "lalr1.cc" // generate C++ parser
//%define api.namespace {uetli::parser}
//%define api.value.type {struct semantic_type}
//%define parser_class_name {Parser}

//%name-prefix "uetli_parser_"
*/
%union {
    //using qlow::ast::Class;
    std::vector<std::unique_ptr<qlow::ast::Class>>* classes;
    qlow::ast::Class* classDefinition;
    qlow::ast::FeatureDeclaration* featureDeclaration;
    std::vector<std::unique_ptr<qlow::ast::FeatureDeclaration>>* featureList;
    std::vector<std::unique_ptr<qlow::ast::ArgumentDeclaration>>* argumentList;
    std::vector<std::unique_ptr<qlow::ast::Statement>>* statements;
    std::vector<std::unique_ptr<qlow::ast::Expression>>* expressionList;
    qlow::ast::ArgumentDeclaration* argumentDeclaration;
    qlow::ast::DoEndBlock* doEndBlock;    
    qlow::ast::Statement* statement;
    qlow::ast::Expression* expression;
    qlow::ast::Operation::Operator op;

    qlow::ast::MethodDefinition* methodDefinition;

    qlow::ast::FeatureCall* featureCall;
    qlow::ast::AssignmentStatement* assignmentStatement;
    qlow::ast::NewVariableStatement* newVariableStatement;

    qlow::ast::UnaryOperation* unaryOperation;
    qlow::ast::BinaryOperation* binaryOperation;

    const char* cString;
    std::string* string;
    int token;
};


%token <string> IDENTIFIER
%token <token> CLASS DO END IF
%token <token> NEW_LINE
%token <token> COLON COMMA DOT ASSIGN OPERATOR
%token <token> ROUND_LEFT ROUND_RIGHT

%type <classes> classes
%type <classDefinition> classDefinition
%type <featureDeclaration> featureDeclaration fieldDeclaration methodDefinition
%type <featureList> featureList
%type <argumentList> argumentList
%type <statements> statements
%type <expressionList> expressionList
%type <argumentDeclaration> argumentDeclaration
%type <doEndBlock> doEndBlock
%type <statement> statement
%type <expression> expression operationExpression paranthesesExpression
%type <op> operator
%type <featureCall> featureCall
%type <assignmentStatement> assignmentStatement 
%type <newVariableStatement> newVariableStatement
%type <unaryOperation> unaryOperation
%type <binaryOperation> binaryOperation

%left ASTERISK SLASH
%left PLUS MINUS

%start classes

%%

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
    /* empty */ {
       parsedClasses = std::make_unique<ClassList>();
    }
    |
    classes classDefinition {
        parsedClasses->push_back(std::move(std::unique_ptr<Class>($2)));
    };


classDefinition:
    CLASS IDENTIFIER featureList END {
        $$ = new Class(*$2, *$3);
        delete $2; delete $3; $2 = 0; $3 = 0;
    };


featureList:
    /* empty */ {
        $$ = new std::vector<std::unique_ptr<FeatureDeclaration>>();
    }
    |
    featureList featureDeclaration {
        $$ = $1;
        $$->push_back(std::move(std::unique_ptr<FeatureDeclaration>($2)));
    };


featureDeclaration:
    fieldDeclaration {
        $$ = $1;
    }
    |
    methodDefinition {
        $$ = $1;
    };


fieldDeclaration:
    IDENTIFIER COLON IDENTIFIER {
        $$ = new FieldDeclaration(*$3, *$1);
        delete $3; delete $1; $1 = $3 = 0;
    };


methodDefinition:
    IDENTIFIER COLON IDENTIFIER doEndBlock {
        $$ = new MethodDefinition(*$3, *$1, std::move(std::unique_ptr<DoEndBlock>($4)));
        delete $3; delete $1; $1 = $3 = 0;
    }
    |
    IDENTIFIER doEndBlock {
        $$ = new MethodDefinition("", *$1, std::move(std::unique_ptr<DoEndBlock>($2)));
        delete $1; $1 = 0;
    }
    |
    IDENTIFIER
        ROUND_LEFT argumentList ROUND_RIGHT COLON IDENTIFIER doEndBlock {
        $$ = new MethodDefinition(*$6, *$1, std::move(*$3), std::move(std::unique_ptr<DoEndBlock>($7)));
        delete $6; delete $1; delete $3; $1 = $6 = nullptr; $3 = nullptr;
    }
    |
    IDENTIFIER ROUND_LEFT argumentList ROUND_RIGHT doEndBlock {
        $$ = new MethodDefinition("", *$1, std::move(*$3), std::move(std::unique_ptr<DoEndBlock>($5)));
        delete $1; delete $3; $1 = nullptr; $3 = nullptr;
    };


argumentList:
    argumentDeclaration {
        $$ = new std::vector<std::unique_ptr<ArgumentDeclaration>>();
        $$->push_back(std::unique_ptr<ArgumentDeclaration>($1));
    }
    |
    argumentList COMMA argumentDeclaration {
        $$ = $1;
        $$->push_back(std::unique_ptr<ArgumentDeclaration>($3));
    };


argumentDeclaration:
    IDENTIFIER COLON IDENTIFIER {
        $$ = new ArgumentDeclaration(*$3, *$1);
        delete $3; delete $1; $1 = $3 = 0;
    };


doEndBlock:
    DO statements END {
        $$ = new DoEndBlock(std::move(*$2));
        delete $2; $2 = 0;
    };


statements:
    /* empty */ {
        $$ = new std::vector<std::unique_ptr<Statement>>();
    }
    |
    statements statement {
        $$ = $1;
        $$->push_back(std::move(std::unique_ptr<Statement>($2)));
    };


statement:
    featureCall {
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


featureCall:
    IDENTIFIER {
        $$ = new FeatureCall(nullptr, *$1);
        delete $1; $1 = 0;
    }
    |
    IDENTIFIER ROUND_LEFT expressionList ROUND_RIGHT {
        $$ = new FeatureCall(nullptr, *$1, std::move(*$3));
        delete $1; delete $3; $1 = 0; $3 = 0;
    }
    |
    expression DOT IDENTIFIER {
        $$ = new FeatureCall(std::move(std::unique_ptr<Expression>($1)), *$3);
        delete $3; $3 = 0;
    }
    |
    expression DOT IDENTIFIER ROUND_LEFT expressionList ROUND_RIGHT {
        $$ = new FeatureCall(std::move(std::unique_ptr<Expression>($1)), *$3,
            std::move(*$5));
        delete $3; $3 = 0; delete $5; $5 = 0;
    };


/* list of effective arguments */
expressionList:
    expression {
        $$ = new std::vector<std::unique_ptr<Expression>>();
        $$->push_back(std::move(std::unique_ptr<Expression>($1)));
    }
    |
    expressionList COMMA expression {
        $$ = $1;
        $$->push_back(std::move(std::unique_ptr<Expression>($3)));
    };


expression:
    featureCall {
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
    binaryOperation {
        $$ = $1;
    }
    |
    unaryOperation {
        $$ = $1;
    };


binaryOperation:
    expression operator expression {
        $$ = new BinaryOperation(std::unique_ptr<Expression>($1), 
            std::unique_ptr<Expression>($3), $2);
    };


unaryOperation:
    expression operator {
        $$ = new UnaryOperation(std::unique_ptr<Expression>($1),
            UnaryOperation::SUFFIX, $2);
    }
    |
    operator expression {
        $$ = new UnaryOperation(std::unique_ptr<Expression>($2),
            UnaryOperation::PREFIX, $1);
    };


operator:
    PLUS { $$ = qlow::ast::Operation::Operator::PLUS; }
    |
    MINUS { $$ = qlow::ast::Operation::Operator::MINUS; }
    |
    ASTERISK { $$ = qlow::ast::Operation::Operator::ASTERISK; }
    |
    SLASH { $$ = qlow::ast::Operation::Operator::SLASH; };


paranthesesExpression:
    ROUND_LEFT expression ROUND_RIGHT {
        $$ = $2;
    };


assignmentStatement:
    IDENTIFIER ASSIGN expression {
        $$ = new AssignmentStatement(std::move(*$1), std::unique_ptr<Expression>($3));
        delete $1; $1 = 0;
    };


newVariableStatement:
    IDENTIFIER COLON IDENTIFIER {
        $$ = new NewVariableStatement(*$3, *$1);
        delete $3; delete $1; $3 = 0; $1 = 0;
    };


%%


