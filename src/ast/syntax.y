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

%code requires {
#include "Ast.h"

using QLOW_PARSER_LTYPE = qlow::CodePosition;
using YYLTYPE = QLOW_PARSER_LTYPE;
#define QLOW_PARSER_LTYPE_IS_DECLARED

union QLOW_PARSER_STYPE;
using YYSTYPE = QLOW_PARSER_STYPE;

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
//#define qlow_parser_lex(a,b,c)  (({ auto v = qlow_parser_lex(a, b, c); v; }))
}

%{

#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include "Parser.h"
#include "ErrorReporting.h"
#include "syntax.hpp"
#include "lexer.h"

using namespace qlow::ast;

//extern int qlow_parser_lex();
//void yy_pop_state();

int qlow_parser_error(qlow::CodePosition* loc, yyscan_t scan,
    Ast& ast, const Parser& parser, const char* msg)
{
    //throw msg;
    //printf("error happened: %s\n", msg);
    // throw msg;
    return 0;
}


extern "C" int qlow_parser_wrap(yyscan_t s)
{
    return 1; /* do not continue on EOF */
}


# define YYLLOC_DEFAULT(Cur, Rhs, N)                      \
do                                                        \
  if (N)                                                  \
    {                                                     \
      (Cur).first_line   = YYRHSLOC(Rhs, 1).first_line;   \
      (Cur).first_column = YYRHSLOC(Rhs, 1).first_column; \
      (Cur).last_line    = YYRHSLOC(Rhs, N).last_line;    \
      (Cur).last_column  = YYRHSLOC(Rhs, N).last_column;  \
      (Cur).filename     = parser.getFilename().c_str();  \
    }                                                     \
  else                                                    \
    {                                                     \
      (Cur).first_line   = (Cur).last_line   =            \
        YYRHSLOC(Rhs, 0).last_line;                       \
      (Cur).first_column = (Cur).last_column =            \
        YYRHSLOC(Rhs, 0).last_column;                     \
      (Cur).filename = parser.getFilename().c_str();      \
    }                                                     \
while (0)
%}

%lex-param   { yyscan_t scanner }
%parse-param { yyscan_t scanner }
%parse-param { qlow::ast::Ast& ast }
%parse-param { const qlow::ast::Parser& parser }


%define api.prefix {qlow_parser_}
%define parse.error verbose
%define api.pure full
// %define parse.lac full

%locations
%defines


%initial-action
{
    // NOTE: the filename only lives as long as the parser.
    // Do not use after deletion of the parser.
    const auto* filename = parser.getFilename().c_str();
    @$.filename = filename;
    qlow_parser_set_extra(parser.getFilename(), scanner);
};


%union {
    qlow::ast::Ast* topLevel;
    qlow::ast::Class* classDefinition;
    
    qlow::ast::Type* type;
    qlow::ast::ClassType* classType;
    qlow::ast::ArrayType* arrayType;
    qlow::ast::PointerType* pointerType;
    
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

    qlow::ast::FieldDeclaration* fieldDeclaration;
    qlow::ast::MethodDefinition* methodDefinition;

    qlow::ast::FeatureCall* featureCall;
    qlow::ast::AssignmentStatement* assignmentStatement;
    qlow::ast::ReturnStatement* returnStatement;
    qlow::ast::LocalVariableStatement* localVariableStatement;
    qlow::ast::AddressExpression* addressExpression;

    qlow::ast::UnaryOperation* unaryOperation;
    qlow::ast::BinaryOperation* binaryOperation;

    qlow::ast::NewArrayExpression* newArrayExpression;
    qlow::ast::CastExpression* castExpression;

    const char* cString;
    std::string* string;
    int token;
}


%token <string> IDENTIFIER
%token <string> INT_LITERAL
%token <string> ASTERISK SLASH PLUS MINUS EQUALS NOT_EQUALS AND OR XOR CUSTOM_OPERATOR
%token <token> TRUE FALSE
%token <token> CLASS DO END IF ELSE WHILE RETURN NEW EXTERN AS
%token <token> NEW_LINE
%token <token> SEMICOLON COLON COMMA DOT ASSIGN AMPERSAND
%token <token> ROUND_LEFT ROUND_RIGHT SQUARE_LEFT SQUARE_RIGHT
%token <string> UNEXPECTED_SYMBOL

%type <topLevel> topLevel 
%type <classDefinition> classDefinition
%type <type> type 
%type <featureDeclaration> featureDeclaration
%type <fieldDeclaration> fieldDeclaration
%type <methodDefinition> methodDefinition
%type <methodDefinition> externMethodDeclaration
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
%type <featureCall> featureCall
%type <assignmentStatement> assignmentStatement 
%type <returnStatement> returnStatement 
%type <localVariableStatement> localVariableStatement
%type <addressExpression> addressExpression 
%type <string> operator
%type <unaryOperation> unaryOperation
%type <binaryOperation> binaryOperation
%type <newArrayExpression> newArrayExpression
%type <castExpression> castExpression

%destructor { } <token>
//%destructor { if ($$) delete $$ } <op>
//%destructor { } <topLevel> // don't delete everything ;)
//%destructor { if ($$) delete $$; } <*>

%left ASSIGN
%left CUSTOM_OPERATOR
%left OR XOR
%left AND
%left NOT
%left EQUALS
%left PLUS MINUS
%left ASTERISK SLASH
%left AS
%left DOT

%start topLevel

%%

/* list of class definitions */
topLevel:
    /* empty */ {
       $$ = &ast;
    }
    |
    topLevel classDefinition {
        $$ = $1;
        $$->getObjects().push_back(std::move(std::unique_ptr<qlow::ast::Class>($2)));
        $2 = nullptr;
    }
    |
    topLevel methodDefinition {
        $$ = $1;
        $$->getObjects().push_back(std::move(std::unique_ptr<qlow::ast::MethodDefinition>($2)));
        $2 = nullptr;
    }
    |
    topLevel externMethodDeclaration {
        $$ = $1;
        $$->getObjects().push_back(std::move(std::unique_ptr<qlow::ast::MethodDefinition>($2)));
        $2 = nullptr;
    }
    |
    topLevel error methodDefinition {
        $$ = $1;
        reportError(qlow::SyntaxError(@2));
        yyerrok;
        $$->getObjects().push_back(std::move(std::unique_ptr<qlow::ast::MethodDefinition>($3)));
        $3 = nullptr;
    }
    |
    topLevel error classDefinition {
        reportError(qlow::SyntaxError(@2));
        yyerrok;
        $$ = $1;
        $$->getObjects().push_back(std::move(std::unique_ptr<qlow::ast::Class>($3)));
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
    type ASTERISK {
        $$ = new qlow::ast::PointerType(std::unique_ptr<Type>($1), @$);
        $1 = nullptr;
    }
    |
    SQUARE_LEFT type SQUARE_RIGHT {
        $$ = new qlow::ast::ArrayType(std::unique_ptr<qlow::ast::Type>($2), @$);
        $2 = nullptr;
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


externMethodDeclaration:
    EXTERN IDENTIFIER COLON type {
        $$ = new MethodDefinition(std::unique_ptr<qlow::ast::Type>($4), *$2, @$);
        delete $2; $2 = nullptr;
    }
    |
    EXTERN IDENTIFIER {
        $$ = new MethodDefinition(nullptr, *$2, @$);
        delete $2; $2 = nullptr;
    }
    |
    EXTERN IDENTIFIER ROUND_LEFT argumentList ROUND_RIGHT {
        $$ = new MethodDefinition(nullptr, *$2, std::move(*$4), @$);
        delete $2; delete $4; $2 = nullptr; $4 = nullptr;
    }
    |
    EXTERN IDENTIFIER ROUND_LEFT argumentList ROUND_RIGHT COLON type {
        $$ = new MethodDefinition(std::unique_ptr<qlow::ast::Type>($7),
                                 *$2, std::move(*$4), @$);
        delete $2; delete $4; $2 = nullptr; $4 = nullptr;
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
    castExpression {
        $$ = $1;
    }
    |
    addressExpression {
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
    expression PLUS expression {
        $$ = new BinaryOperation(std::unique_ptr<Expression>($1), 
            std::unique_ptr<Expression>($3), *$2, @$, @2);
        delete $2; $2 = nullptr;
    }
    |
    expression MINUS expression {
        $$ = new BinaryOperation(std::unique_ptr<Expression>($1), 
            std::unique_ptr<Expression>($3), *$2, @$, @2);
        delete $2; $2 = nullptr;
    }
    |
    expression ASTERISK expression {
        $$ = new BinaryOperation(std::unique_ptr<Expression>($1), 
            std::unique_ptr<Expression>($3), *$2, @$, @2);
        delete $2; $2 = nullptr;
    }
    |
    expression SLASH expression {
        $$ = new BinaryOperation(std::unique_ptr<Expression>($1), 
            std::unique_ptr<Expression>($3), *$2, @$, @2);
        delete $2; $2 = nullptr;
    }
    |
    expression EQUALS expression {
        $$ = new BinaryOperation(std::unique_ptr<Expression>($1), 
            std::unique_ptr<Expression>($3), *$2, @$, @2);
        delete $2; $2 = nullptr;
    }
    |
    expression NOT_EQUALS expression {
        $$ = new BinaryOperation(std::unique_ptr<Expression>($1), 
            std::unique_ptr<Expression>($3), *$2, @$, @2);
        delete $2; $2 = nullptr;
    }
    |
    expression AND expression {
        $$ = new BinaryOperation(std::unique_ptr<Expression>($1), 
            std::unique_ptr<Expression>($3), *$2, @$, @2);
        delete $2; $2 = nullptr;
    }
    |
    expression OR expression {
        $$ = new BinaryOperation(std::unique_ptr<Expression>($1), 
            std::unique_ptr<Expression>($3), *$2, @$, @2);
        delete $2; $2 = nullptr;
    }
    |
    expression XOR expression {
        $$ = new BinaryOperation(std::unique_ptr<Expression>($1), 
            std::unique_ptr<Expression>($3), *$2, @$, @2);
        delete $2; $2 = nullptr;
    }
    |
    expression CUSTOM_OPERATOR expression {
        $$ = new BinaryOperation(std::unique_ptr<Expression>($1), 
            std::unique_ptr<Expression>($3), *$2, @$, @2);
        delete $2; $2 = nullptr;
    }
    ;


unaryOperation:
    expression operator {
        $$ = new UnaryOperation(std::unique_ptr<Expression>($1),
            UnaryOperation::SUFFIX, *$2, @$, @2);
        delete $2; $2 = nullptr;
    }
    |
    operator expression {
        $$ = new UnaryOperation(std::unique_ptr<Expression>($2),
            UnaryOperation::PREFIX, *$1, @$, @2);
        delete $1; $1 = nullptr;
    };


operator:
    PLUS { $$ = $1; }
    |
    MINUS { $$ = $1; }
    |
    ASTERISK { $$ = $1; }
    |
    SLASH { $$ = $1; }
    |
    EQUALS { $$ = $1; }
    |
    NOT_EQUALS { $$ = $1; }
    |
    AND { $$ = $1; }
    |
    OR { $$ = $1; }
    |
    XOR { $$ = $1; }
    |
    CUSTOM_OPERATOR { $$ = $1; };

addressExpression:
    AMPERSAND expression {
        $$ = new AddressExpression(std::unique_ptr<Expression>($2), @$);
        $2 = nullptr;
    };

paranthesesExpression:
    ROUND_LEFT expression ROUND_RIGHT {
        $$ = $2;
    };

newArrayExpression:
    NEW SQUARE_LEFT type SEMICOLON expression SQUARE_RIGHT {
        $$ = nullptr;
    };
    
castExpression:
    expression AS type {
        $$ = new CastExpression(std::unique_ptr<Expression>($1),
            std::unique_ptr<qlow::ast::Type>($3), @$);
    };

assignmentStatement:
    expression ASSIGN expression {
        $$ = new AssignmentStatement(std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3), @$);
    };

returnStatement:
    RETURN expression {
        $$ = new ReturnStatement(std::unique_ptr<Expression>($2), @$);
    }
    |
    RETURN {
        $$ = new ReturnStatement(nullptr, @$);
    };

localVariableStatement:
    IDENTIFIER COLON type {
        $$ = new LocalVariableStatement(std::move(*$1), std::unique_ptr<qlow::ast::Type>($3), @$);
        delete $1; $3 = nullptr; $1 = nullptr;
    };


%%


