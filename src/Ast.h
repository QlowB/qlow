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

#ifndef QLOW_AST_H
#define QLOW_AST_H

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <map>

#include "Visitor.h"
#include "Util.h"
#include "Util.h"

namespace qlow
{
    struct CodePosition;
    
    class StructureVisitor;
    namespace ast
    {
        // base class
        struct AstObject;

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

    namespace sem
    {
        struct SemanticObject;
        struct Class;
        
        template<typename T>
        using SymbolTable = std::map<std::string, std::unique_ptr<T>>;
    }
}


/*!
 * \brief bison-compatible location struct
 */
struct qlow::CodePosition
{
    const char* filename;
    int first_line;
    int last_line;
    int first_column;
    int last_column;
};


struct qlow::ast::AstObject :
    public Visitable<std::unique_ptr<sem::SemanticObject>, const sem::SymbolTable<sem::Class>, StructureVisitor>
{
    CodePosition pos;
    
    inline AstObject(const CodePosition& cp) :
        pos{ cp } {}
        
    virtual ~AstObject(void);
};


struct qlow::ast::Class : public AstObject
{
    std::string name;
    OwningList<FeatureDeclaration> features;
    
    inline Class(const std::string& name, OwningList<FeatureDeclaration>& features, const CodePosition& cp) :
        AstObject{ cp },
        name{ name }, features(std::move(features))
    {
    }

    std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, const sem::SymbolTable<sem::Class>&) override;
};


struct qlow::ast::FeatureDeclaration : public AstObject
{
    std::string name;
    std::string type;

    inline FeatureDeclaration(const std::string& type, const std::string& name, const CodePosition& cp) :
        AstObject{ cp },
        name(name), type(type)
    {
    }

    std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, const sem::SymbolTable<sem::Class>&);
};


struct qlow::ast::FieldDeclaration : public FeatureDeclaration
{
    inline FieldDeclaration(const std::string& type, const std::string& name, const CodePosition& cp) :
        FeatureDeclaration(type, name, cp)
    {
    }

    std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, const sem::SymbolTable<sem::Class>&);
};


struct qlow::ast::MethodDefinition : public FeatureDeclaration
{
    OwningList<ArgumentDeclaration> arguments;
    std::unique_ptr<DoEndBlock> body;

    inline MethodDefinition(const std::string& type, const std::string& name,
            std::unique_ptr<DoEndBlock> body, const CodePosition& cp) :
        FeatureDeclaration(type, name, cp),
        body(std::move(body))
    {
    }


    inline MethodDefinition(const std::string& type, const std::string& name,
            OwningList<ArgumentDeclaration>&& arguments, std::unique_ptr<DoEndBlock> body, const CodePosition& cp) :
        FeatureDeclaration(type, name, cp),
        arguments(std::move(arguments)),
        body(std::move(body))
    {
    }

    std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, const sem::SymbolTable<sem::Class>&);
};


struct qlow::ast::VariableDeclaration  : public AstObject
{
    std::string type;
    std::string name;
    inline VariableDeclaration(const std::string& type, const std::string& name, const CodePosition& cp) :
        AstObject{ cp },
        type(type), name(name)
    {
    }

    std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, const sem::SymbolTable<sem::Class>&);
};


struct qlow::ast::ArgumentDeclaration :
    public VariableDeclaration
{
    inline ArgumentDeclaration(const std::string& type, const std::string& name, const CodePosition& cp) :
        VariableDeclaration(type, name, cp)
    {
    }

    std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, const sem::SymbolTable<sem::Class>&);
};


struct qlow::ast::DoEndBlock : public AstObject
{
    OwningList<Statement> statements;
    
    inline DoEndBlock(OwningList<Statement>&& statements, const CodePosition& cp) :
        AstObject{ cp },
        statements(std::move(statements))
    {
    }

    std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, const sem::SymbolTable<sem::Class>&);
};


struct qlow::ast::Statement : public virtual AstObject
{
    inline Statement(const CodePosition& cp) :
        AstObject{ cp } {}
        
    virtual ~Statement(void);

    std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, const sem::SymbolTable<sem::Class>&);
};


struct qlow::ast::Expression : public virtual AstObject
{
    inline Expression(const CodePosition& cp) :
        AstObject{ cp } {}
        
    std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, const sem::SymbolTable<sem::Class>&);
};


struct qlow::ast::FeatureCall : public Expression, public Statement
{
    std::unique_ptr<Expression> target;
    std::string name;
    OwningList<Expression> arguments;

    inline FeatureCall(std::unique_ptr<Expression> target, const std::string& name, const CodePosition& cp) :
        AstObject{ cp },
        Expression{ cp }, Statement{ cp },
        target(std::move(target)), name(name)
    {
    }


    inline FeatureCall(std::unique_ptr<Expression> target, const std::string& name,
            OwningList<Expression>&& arguments, const CodePosition& cp) :
        AstObject{ cp },
        Expression{ cp }, Statement{ cp },
        target(std::move(target)), name(name), arguments(std::move(arguments))
    {
    }

    std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, const sem::SymbolTable<sem::Class>&);
};


struct qlow::ast::AssignmentStatement : public Statement
{
    std::string target;
    std::unique_ptr<Expression> expr;

    inline AssignmentStatement(const std::string& target, std::unique_ptr<Expression> expr, const CodePosition& cp) :
        AstObject{ cp },
        Statement{ cp },
        target(target), expr(std::move(expr))
    {
    }

    std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, const sem::SymbolTable<sem::Class>&);
};


struct qlow::ast::NewVariableStatement : public Statement
{
    std::string name;
    std::string type;
    inline NewVariableStatement(const std::string& name, const std::string& type, const CodePosition& cp) :
        AstObject{ cp },
        Statement{ cp },
       name(name), type(type)
    {
    } 

    std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, const sem::SymbolTable<sem::Class>&);
};


struct qlow::ast::Operation : public Expression
{
    enum Operator {
        PLUS, MINUS, ASTERISK, SLASH
    };
    Operator op;

    inline Operation(Operator op, const CodePosition& cp) :
        AstObject{ cp },
        Expression{ cp },
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

    inline UnaryOperation(std::unique_ptr<Expression> expr, Side side, Operator op, const CodePosition& cp) :
        AstObject{ cp },
        Operation(op, cp),
        side(side),
        expr(std::move(expr))
    {
    }

    std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, const sem::SymbolTable<sem::Class>&);
};


struct qlow::ast::BinaryOperation : public Operation
{
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    inline BinaryOperation(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, Operator op, const CodePosition& cp) :
        AstObject{ cp },
        Operation(op, cp),
        left(std::move(left)), right(std::move(right))
    {
    }

    std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, const sem::SymbolTable<sem::Class>&);
};


#endif // QLOW_AST_H


