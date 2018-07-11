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
#include "ErrorReporting.h"

using namespace qlow::ast;

extern int qlow_parser_lex();

void yy_pop_state(void);

int qlow_parser_error(const char* msg)
{
    //throw msg;
    //printf("error happened: %s\n", msg);
    // throw msg;
}

std::unique_ptr<std::vector<std::unique_ptr<qlow::ast::AstObject>>> parsedClasses;
const char* qlow_parser_filename = "";

# define YYLLOC_DEFAULT(Cur, Rhs, N)                      \
do                                                        \
  if (N)                                                  \
    {                                                     \
      (Cur).first_line   = YYRHSLOC(Rhs, 1).first_line;   \
      (Cur).first_column = YYRHSLOC(Rhs, 1).first_column; \
      (Cur).last_line    = YYRHSLOC(Rhs, N).last_line;    \
      (Cur).last_column  = YYRHSLOC(Rhs, N).last_column;  \
      (Cur).filename     = YYRHSLOC(Rhs, 1).filename;     \
    }                                                     \
  else                                                    \
    {                                                     \
      (Cur).first_line   = (Cur).last_line   =            \
        YYRHSLOC(Rhs, 0).last_line;                       \
      (Cur).first_column = (Cur).last_column =            \
        YYRHSLOC(Rhs, 0).last_column;                     \
    }                                                     \
while (0)

%}


%define api.prefix {qlow_parser_}
%define parse.error verbose
// %define parse.lac full

%locations
%code requires {
#include "Ast.h"
typedef qlow::CodePosition QLOW_PARSER_LTYPE;
#define QLOW_PARSER_LTYPE_IS_DECLARED
}

%initial-action
{
  @$.filename = qlow_parser_filename;
};

//%define api.location.type {qlow::CodePosition}

%union {
    std::vector<std::unique_ptr<qlow::ast::AstObject>>* topLevel;
    qlow::ast::Class* classDefinition;
    
    qlow::ast::Type* type;
    qlow::ast::ClassType* classType;
    qlow::ast::ArrayType* arrayType;
    
    qlow::ast::FeatureDeclaration* featureDeclaration;
    std::vector<std::unique_ptr<qlow::ast::FeatureDeclaration>>* featureList;
    std::vector<std::unique_ptr<qlow::ast::ArgumentDeclaration>>* argumentList;
    std::vector<std::unique_ptr<qlow::ast::Statement>>* statements;
    std::vector<std::unique_ptr<qlow::ast::Expression>>* expressionList;
    qlow::ast::ArgumentDeclaration* argumentDeclaration;
    qlow::ast::DoEndBlock* doEndBlock;    
    qlow::ast::IfElseBlock* ifElseBlock;    
    qlow::ast::WhileBlock* whileBlock;    
    qlow::ast::Statement* statement;
    qlow::ast::Expression* expression;
    qlow::ast::Operation::Operator op;

    qlow::ast::FieldDeclaration* fieldDeclaration;
    qlow::ast::MethodDefinition* methodDefinition;

    qlow::ast::FeatureCall* featureCall;
    qlow::ast::AssignmentStatement* assignmentStatement;
    qlow::ast::ReturnStatement* returnStatement;
    qlow::ast::LocalVariableStatement* localVariableStatement;

    qlow::ast::UnaryOperation* unaryOperation;
    qlow::ast::BinaryOperation* binaryOperation;
    
    qlow::ast::NewArrayExpression* newArrayExpression;

    const char* cString;
    std::string* string;
    int token;
}


%token <string> IDENTIFIER
%token <string> INT_LITERAL
%token <token> TRUE FALSE
%token <token> CLASS DO END IF ELSE WHILE RETURN NEW
%token <token> NEW_LINE
%token <token> SEMICOLON COLON COMMA DOT ASSIGN EQUALS NOT_EQUALS
%token <token> ROUND_LEFT ROUND_RIGHT SQUARE_LEFT SQUARE_RIGHT
%token <string> UNEXPECTED_SYMBOL

%type <topLevel> topLevel 
%type <classDefinition> classDefinition
%type <type> type 
%type <featureDeclaration> featureDeclaration
%type <fieldDeclaration> fieldDeclaration
%type <methodDefinition> methodDefinition
%type <featureList> featureList
%type <argumentList> argumentList
%type <statements> statements
%type <expressionList> expressionList
%type <argumentDeclaration> argumentDeclaration
%type <doEndBlock> doEndBlock
%type <ifElseBlock> ifElseBlock 
%type <whileBlock> whileBlock
%type <statement> statement
%type <expression> expression operationExpression paranthesesExpression
%type <op> operator
%type <featureCall> featureCall
%type <assignmentStatement> assignmentStatement 
%type <returnStatement> returnStatement 
%type <localVariableStatement> localVariableStatement
%type <unaryOperation> unaryOperation
%type <binaryOperation> binaryOperation
%type <newArrayExpression> newArrayExpression

%destructor { } <token>
%destructor { } <op>
%destructor { } <topLevel> // don't delete everything ;)
%destructor { if ($$) delete $$; } <*>

%left ASTERISK SLASH
%left PLUS MINUS
%left EQUALS
%left NOT
%left AND
%left OR XOR

%start topLevel

%%

/* list of class definitions */
topLevel:
    /* empty */ {
       parsedClasses = std::make_unique<std::vector<std::unique_ptr<qlow::ast::AstObject>>>();
    }
    |
    topLevel classDefinition {
        parsedClasses->push_back(std::move(std::unique_ptr<qlow::ast::Class>($2)));
        $2 = nullptr;
    }
    |
    topLevel methodDefinition {
        parsedClasses->push_back(std::move(std::unique_ptr<qlow::ast::MethodDefinition>($2)));
        $2 = nullptr;
    }
    |
    topLevel error methodDefinition {
        reportError(qlow::SyntaxError(@2));
        yyerrok;
        parsedClasses->push_back(std::move(std::unique_ptr<qlow::ast::MethodDefinition>($3)));
        $3 = nullptr;
    }
    |
    topLevel error classDefinition {
        reportError(qlow::SyntaxError(@2));
        yyerrok;
        parsedClasses->push_back(std::move(std::unique_ptr<qlow::ast::Class>($3)));
        $3 = nullptr;
    };


classDefinition:
    CLASS IDENTIFIER featureList END {
        $$ = new Class(*$2, *$3, @$);
        delete $2; delete $3; $2 = 0; $3 = 0;
    }
    |
    CLASS error END {
        reportError(qlow::SyntaxError(@2));
        yyerrok;
        $$ = nullptr;
    };


type:
    IDENTIFIER {
        $$ = new qlow::ast::ClassType(std::move(*$1), @$);
        delete $1; $1 = nullptr;
    }
    |
    SQUARE_LEFT type SQUARE_RIGHT {
        $$ = new qlow::ast::ArrayType(std::unique_ptr<qlow::ast::Type>($2), @$);
    }
    |
    SQUARE_LEFT error SQUARE_RIGHT {
        reportError(qlow::SyntaxError("invalid type", @2));
    };
    

featureList:
    /* empty */ {
        $$ = new std::vector<std::unique_ptr<FeatureDeclaration>>();
    }
    |
    featureList featureDeclaration {
        $$ = $1;
        $$->push_back(std::move(std::unique_ptr<FeatureDeclaration>($2)));
        $2 = nullptr;
    }
    |
    featureList error featureDeclaration {
        $$ = $1;
        yyerrok;
        reportError(qlow::SyntaxError(@2));
        $$->push_back(std::move(std::unique_ptr<FeatureDeclaration>($3)));
        $3 = nullptr;
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
    IDENTIFIER COLON type {
        $$ = new FieldDeclaration(std::unique_ptr<qlow::ast::Type>($3), std::move(*$1), @$);
        delete $1; $1 = nullptr;
    };


methodDefinition:
    IDENTIFIER COLON type doEndBlock {
        $$ = new MethodDefinition(std::unique_ptr<qlow::ast::Type>($3), *$1, std::unique_ptr<DoEndBlock>($4), @$);
        delete $1; $1 = nullptr;
    }
    |
    IDENTIFIER doEndBlock {
        $$ = new MethodDefinition(nullptr, *$1, std::move(std::unique_ptr<DoEndBlock>($2)), @$);
        delete $1; $1 = nullptr;
    }
    |
    IDENTIFIER
        ROUND_LEFT argumentList ROUND_RIGHT COLON type doEndBlock {
        $$ = new MethodDefinition(std::unique_ptr<qlow::ast::Type>($6),
                                 *$1, std::move(*$3),
                                 std::unique_ptr<DoEndBlock>($7),
                                 @$);
        delete $1; delete $3; $1 = nullptr; $3 = nullptr;
    }
    |
    IDENTIFIER ROUND_LEFT argumentList ROUND_RIGHT doEndBlock {
        $$ = new MethodDefinition(nullptr, *$1, std::move(*$3), std::move(std::unique_ptr<DoEndBlock>($5)), @$);
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
    IDENTIFIER COLON type {
        $$ = new ArgumentDeclaration(std::unique_ptr<qlow::ast::Type>($3), std::move(*$1), @$);
        delete $1; $1 = nullptr; $3 = nullptr;
    };


doEndBlock:
    DO statements END {
        $$ = new DoEndBlock(std::move(*$2), @$);
        delete $2; $2 = nullptr;
    };
    
    
ifElseBlock:
    IF expression doEndBlock {
        $$ = new IfElseBlock(std::unique_ptr<Expression>($2),
                             std::unique_ptr<DoEndBlock>($3),
                             std::make_unique<DoEndBlock>(qlow::OwningList<qlow::ast::Statement>{}, @3), @$);
        $2 = nullptr; $3 = nullptr;
    }
    |
    IF expression DO statements ELSE statements END {
        $$ = new IfElseBlock(std::unique_ptr<Expression>($2),
                             std::make_unique<DoEndBlock>(std::move(*$4), @4),
                             std::make_unique<DoEndBlock>(std::move(*$6), @6), @$);
        $2 = nullptr;
        delete $4;
        delete $6;
    };
    
whileBlock:
    WHILE expression doEndBlock {
        $$ = new WhileBlock(std::unique_ptr<Expression>($2),
                            std::unique_ptr<DoEndBlock>($3), @$);
        $2 = nullptr; $3 = nullptr;
    };


statements:
    pnl {
        $$ = new std::vector<std::unique_ptr<Statement>>();
    }
    |
    statements statement {
        $$ = $1;
        // statements can be null on errors
        if ($1 != nullptr)
            $$->push_back(std::unique_ptr<Statement>($2));
    };

/*!
 * hacky way to allow for multiple empty lines between statements
 */
pnl:
    /* empty */ {
    }
    |
    pnl NEW_LINE {
    }
    ;

statement:
    featureCall statementEnd {
        $$ = $1;
    }
    |
    assignmentStatement statementEnd {
        $$ = $1;
    }
    |
    returnStatement statementEnd {
        $$ = $1;
    }
    |
    localVariableStatement statementEnd {
        $$ = $1;
    }
    |
    ifElseBlock statementEnd {
        $$ = $1;
    }
    |
    whileBlock statementEnd {
        $$ = $1;
    }
    |
    error statementEnd {
        $$ = nullptr;
        //printf("error happened here (%s): %d\n", qlow_parser_filename, @1.first_line);
        //throw qlow::SyntaxError(@1);
        reportError(qlow::SyntaxError(@1));
        printf("unexpected token: %d\n", $<token>1);
    }
    ;
    
statementEnd:
    SEMICOLON pnl {}
    |
    NEW_LINE pnl {}
    ;


featureCall:
    IDENTIFIER {
        $$ = new FeatureCall(nullptr, *$1, @$);
        delete $1; $1 = 0;
    }
    |
    IDENTIFIER ROUND_LEFT expressionList ROUND_RIGHT {
        $$ = new FeatureCall(nullptr, *$1, std::move(*$3), @$);
        delete $1; delete $3; $1 = 0; $3 = 0;
    }
    |
    expression DOT IDENTIFIER {
        $$ = new FeatureCall(std::unique_ptr<Expression>($1), *$3, @$);
        delete $3; $3 = 0;
    }
    |
    expression DOT IDENTIFIER ROUND_LEFT expressionList ROUND_RIGHT {
        $$ = new FeatureCall(std::unique_ptr<Expression>($1), *$3,
            std::move(*$5), @$);
        delete $3; $3 = 0; delete $5; $5 = 0;
    };


/* list of effective arguments */
expressionList:
    expression {
        $$ = new std::vector<std::unique_ptr<Expression>>();
        $$->push_back(std::unique_ptr<Expression>($1));
    }
    |
    expressionList COMMA expression {
        $$ = $1;
        $$->push_back(std::unique_ptr<Expression>($3));
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
    }
    |
    newArrayExpression {
        $$ = $1;
    }
    |
    INT_LITERAL {
        $$ = new IntConst(std::move(*$1), @$);
        delete $1;
    };/*
    |
    error {
        $$ = nullptr;
        reportError(qlow::SyntaxError(@1));
        //throw qlow::SyntaxError(@1);
    }
    ;
*/

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
            std::unique_ptr<Expression>($3), $2, @$);
    };


unaryOperation:
    expression operator {
        $$ = new UnaryOperation(std::unique_ptr<Expression>($1),
            UnaryOperation::SUFFIX, $2, @$);
    }
    |
    operator expression {
        $$ = new UnaryOperation(std::unique_ptr<Expression>($2),
            UnaryOperation::PREFIX, $1, @$);
    };


operator:
    PLUS { $$ = qlow::ast::Operation::Operator::PLUS; }
    |
    MINUS { $$ = qlow::ast::Operation::Operator::MINUS; }
    |
    ASTERISK { $$ = qlow::ast::Operation::Operator::ASTERISK; }
    |
    SLASH { $$ = qlow::ast::Operation::Operator::SLASH; }
    |
    EQUALS { $$ = qlow::ast::Operation::Operator::EQUALS; }
    |
    NOT_EQUALS { $$ = qlow::ast::Operation::Operator::NOT_EQUALS; }
    |
    AND { $$ = qlow::ast::Operation::Operator::AND; }
    |
    OR { $$ = qlow::ast::Operation::Operator::OR; }
    |
    XOR { $$ = qlow::ast::Operation::Operator::XOR; };


paranthesesExpression:
    ROUND_LEFT expression ROUND_RIGHT {
        $$ = $2;
    };

newArrayExpression:
    NEW SQUARE_LEFT type SEMICOLON expression SQUARE_RIGHT {
        
    };


assignmentStatement:
    expression ASSIGN expression {
        $$ = new AssignmentStatement(std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3), @$);
    };

returnStatement:
    RETURN expression {
        $$ = new ReturnStatement(std::unique_ptr<Expression>($2), @$);
    };

localVariableStatement:
    IDENTIFIER COLON type {
        $$ = new LocalVariableStatement(std::move(*$1), std::unique_ptr<qlow::ast::Type>($3), @$);
        delete $1; $3 = nullptr; $1 = nullptr;
    };


%%


