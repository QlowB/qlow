// =============================================================================
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
// =============================================================================

#include <string>
#include <vector>
#include <memory>
#include <utility>


namespace qlow
{
    namespace ast
    {
        template<typename T>
        using List = std::vector<std::unique_ptr<T>>;


        struct Class;

        struct FeatureDeclaration;

        struct FieldDeclaration;
        struct MethodDefinition;

        struct VariableDeclaration;
        struct ArgumentDeclaration;

        struct DoEndBlock;

        struct Statement;
        struct Expression;

        struct FeatureCall;
        struct AssignmentStatement;
        struct NewVariableStatement;

        struct Operation;
        struct UnaryOperation;
        struct BinaryOperation;
    }
}



struct qlow::ast::Class
{
    std::string name;
    List<FeatureDeclaration> features;
    
    inline Class(const std::string& name, List<FeatureDeclaration>& features) :
        name(name), features(std::move(features))
    {
    }
};


struct qlow::ast::FeatureDeclaration
{
    std::string name;
    std::string type;

    inline FeatureDeclaration(const std::string& type, const std::string& name) :
        name(name), type(type)
    {
    }
};


struct qlow::ast::FieldDeclaration : public FeatureDeclaration
{
    inline FieldDeclaration(const std::string& type, const std::string& name) :
        FeatureDeclaration(type, name)
    {
    }
};


struct qlow::ast::MethodDefinition : public FeatureDeclaration
{
    List<ArgumentDeclaration> arguments;
    std::unique_ptr<DoEndBlock> body;

    inline MethodDefinition(const std::string& type, const std::string& name,
            std::unique_ptr<DoEndBlock> body) :
        FeatureDeclaration(type, name),
        body(std::move(body))
    {
    }


    inline MethodDefinition(const std::string& type, const std::string& name,
            List<ArgumentDeclaration>&& arguments, std::unique_ptr<DoEndBlock> body) :
        FeatureDeclaration(type, name),
        arguments(std::move(arguments)),
        body(std::move(body))
    {
    }
};


struct qlow::ast::VariableDeclaration 
{
    std::string type;
    std::string name;
    inline VariableDeclaration(const std::string& type, const std::string& name) :
        type(type), name(name)
    {
    }
};


struct qlow::ast::ArgumentDeclaration :
    public VariableDeclaration
{
    inline ArgumentDeclaration(const std::string& type, const std::string& name) :
        VariableDeclaration(type, name)
    {
    }
};


struct qlow::ast::DoEndBlock
{
    List<Statement> statements;
    
    inline DoEndBlock(List<Statement>&& statements) :
        statements(std::move(statements))
    {
    }
};


struct qlow::ast::Statement
{
    virtual ~Statement(void);
};


struct qlow::ast::Expression
{
};


struct qlow::ast::FeatureCall : public Expression, public Statement
{
    std::unique_ptr<Expression> target;
    std::string name;
    List<Expression> arguments;

    inline FeatureCall(std::unique_ptr<Expression> target, const std::string& name) :
        target(std::move(target)), name(name)
    {
    }


    inline FeatureCall(std::unique_ptr<Expression> target, const std::string& name,
            List<Expression>&& arguments) :
        target(std::move(target)), name(name), arguments(std::move(arguments))
    {
    }
};


struct qlow::ast::AssignmentStatement : public Statement
{
    std::string target;
    std::unique_ptr<Expression> expr;

    inline AssignmentStatement(const std::string& target, std::unique_ptr<Expression> expr) :
        target(target), expr(std::move(expr))
    {
    }
};


struct qlow::ast::NewVariableStatement : public Statement
{
    std::string name;
    std::string type;
    inline NewVariableStatement(const std::string& name, const std::string& type) :
       name(name), type(type)
    {
    } 
};


struct qlow::ast::Operation : public Expression
{
    enum Operator {
        PLUS, MINUS, ASTERISK, SLASH
    };
    Operator op;

    inline Operation(Operator op) :
        op(op)
    {
    }
};


struct qlow::ast::UnaryOperation : public Operation
{
    enum Side
    {
        PREFIX,
        SUFFIX,
    };

    Side side;
    std::unique_ptr<Expression> expr;

    inline UnaryOperation(std::unique_ptr<Expression> expr, Side side, Operator op) :
        Operation(op),
        side(side),
        expr(std::move(expr))
    {
    }
};


struct qlow::ast::BinaryOperation : public Operation
{
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    inline BinaryOperation(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, Operator op) :
        Operation(op),
        left(std::move(left)), right(std::move(right))
    {
    }
};




