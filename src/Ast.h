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
#include "ErrorReporting.h"

namespace qlow
{
    struct CodePosition;
    
    class StructureVisitor;
    namespace ast
    {
        // base class
        struct AstObject;

        struct Class;

        struct Type;
        struct ClassType;
        struct ArrayType;
        
        struct FeatureDeclaration;

        struct FieldDeclaration;
        struct MethodDefinition;

        struct VariableDeclaration;
        struct ArgumentDeclaration;

        struct Statement;
        
        struct DoEndBlock;
        struct IfElseBlock;
        struct WhileBlock;

        struct Expression;

        struct FeatureCall;
        struct AssignmentStatement;
        struct ReturnStatement;
        struct LocalVariableStatement;
        struct IntConst;

        struct Operation;
        struct UnaryOperation;
        struct BinaryOperation;
        
        struct NewArrayExpression;
        
        struct CastExpression;
    }

    namespace sem
    {
        struct SemanticObject;
        struct Class;
        
        class Scope;

//        template<typename T>
//        using SymbolTable = std::map<std::string, std::unique_ptr<T>>;
    }
}



struct qlow::ast::AstObject :
    public Visitable<std::unique_ptr<sem::SemanticObject>, sem::Scope&, StructureVisitor>
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

    virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&) override;
};


struct qlow::ast::Type : public AstObject
{
    inline Type(const CodePosition& cp) :
        AstObject{ cp }
    {
    }
    
    virtual std::string asString(void) const = 0;
    virtual inline std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&) override {}
};


struct qlow::ast::ClassType : public ast::Type
{
    std::string typeName;
    
    inline ClassType(const std::string& typeName, const CodePosition& cp) :
        Type{ cp },
        typeName{ typeName }
    {
    }
    
    inline ClassType(std::string&& typeName, const CodePosition& cp) :
        Type{ cp },
        typeName{ std::move(typeName) }
    {
    }
    
    inline std::string asString(void) const override { return typeName; }
};


struct qlow::ast::ArrayType : public ast::Type 
{
    std::unique_ptr<ast::Type> arrayType;
    
    inline ArrayType(std::unique_ptr<ast::Type> arrayType, const CodePosition& cp) :
        Type{ cp },
        arrayType{ std::move(arrayType) }
    {
    }
    
    inline std::string asString(void) const override {
        return std::string("[") + arrayType->asString() + "]";
    }
};


struct qlow::ast::FeatureDeclaration : public AstObject
{
    std::string name;
    std::unique_ptr<ast::Type> type;

    inline FeatureDeclaration(std::unique_ptr<ast::Type> type, const std::string& name, const CodePosition& cp) :
        AstObject{ cp },
        name{ name },
        type{ std::move(type) }
    {
    }

    virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&);
};


struct qlow::ast::FieldDeclaration : public FeatureDeclaration
{
    inline FieldDeclaration(std::unique_ptr<ast::Type> type, const std::string& name, const CodePosition& cp) :
        FeatureDeclaration{ std::move(type), name, cp }
    {
    }

    virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&);
};


struct qlow::ast::MethodDefinition : public FeatureDeclaration
{
    OwningList<ArgumentDeclaration> arguments;
    
    /// pointer to method body. If this is a null pointer, the method has only
    /// been declared and not defined (with extern)
    std::unique_ptr<DoEndBlock> body;

    inline MethodDefinition(std::unique_ptr<ast::Type> type, const std::string& name,
            std::unique_ptr<DoEndBlock> body, const CodePosition& cp) :
        FeatureDeclaration{ std::move(type), name, cp },
        body{ std::move(body) }
    {
    }


    inline MethodDefinition(std::unique_ptr<ast::Type> type, const std::string& name,
            OwningList<ArgumentDeclaration>&& arguments, std::unique_ptr<DoEndBlock> body, const CodePosition& cp) :
        FeatureDeclaration{ std::move(type), name, cp },
        arguments(std::move(arguments)),
        body{ std::move(body) }
    {
    }

    
    inline MethodDefinition(std::unique_ptr<ast::Type> type, const std::string& name,
            const CodePosition& cp) :
        FeatureDeclaration{ std::move(type), name, cp },
        body{ nullptr }
    {
    }
    

    inline MethodDefinition(std::unique_ptr<ast::Type> type, const std::string& name,
            OwningList<ArgumentDeclaration>&& arguments, const CodePosition& cp) :
        FeatureDeclaration{ std::move(type), name, cp },
        arguments(std::move(arguments)),
        body{ nullptr }
    {
    }

    virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&);
};


struct qlow::ast::VariableDeclaration : public AstObject
{
    std::unique_ptr<ast::Type> type;
    std::string name;
    inline VariableDeclaration(std::unique_ptr<ast::Type> type, std::string&& name, const CodePosition& cp) :
        AstObject{ cp },
        type{ std::move(type) },
        name{ std::move(name) }
    {
    }

    virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&);
};


struct qlow::ast::ArgumentDeclaration :
    public VariableDeclaration
{
    inline ArgumentDeclaration(std::unique_ptr<ast::Type> type, std::string&& name, const CodePosition& cp) :
        VariableDeclaration{ std::move(type), std::move(name), cp }
    {
    }

    //virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&);
};


struct qlow::ast::Statement : public virtual AstObject
{
    inline Statement(const CodePosition& cp) :
        AstObject{ cp } {}
        
    virtual ~Statement(void);

    virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&);
};


struct qlow::ast::DoEndBlock : public Statement 
{
    OwningList<Statement> statements;
    
    inline DoEndBlock(OwningList<Statement>&& statements, const CodePosition& cp) :
        AstObject{ cp },
        Statement{ cp },
        statements(std::move(statements))
    {
    }

    virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&);
};


struct qlow::ast::IfElseBlock : public Statement
{
    std::unique_ptr<Expression> condition;
    std::unique_ptr<DoEndBlock> ifBlock;
    std::unique_ptr<DoEndBlock> elseBlock;
    
    inline IfElseBlock(std::unique_ptr<Expression> condition,
                       std::unique_ptr<DoEndBlock> ifBlock,
                       std::unique_ptr<DoEndBlock> elseBlock,
                       const CodePosition& cp) :
        AstObject{ cp },
        Statement{ cp },
        condition{ std::move(condition) },
        ifBlock{ std::move(ifBlock) },
        elseBlock{ std::move(elseBlock) }
    {
    }

    virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&);
};


struct qlow::ast::WhileBlock : public Statement
{
    std::unique_ptr<Expression> condition;
    std::unique_ptr<DoEndBlock> body;
    
    inline WhileBlock(std::unique_ptr<Expression> condition,
                      std::unique_ptr<DoEndBlock> body,
                      const CodePosition& cp) :
        AstObject{ cp },
        Statement{ cp },
        condition{ std::move(condition) },
        body{ std::move(body) }
    {
    }

    virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&);
};


struct qlow::ast::Expression : public virtual AstObject
{
    inline Expression(const CodePosition& cp) :
        AstObject{ cp } {}
        
    virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&);
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

    virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&);
};


struct qlow::ast::ReturnStatement : public Statement
{
    std::unique_ptr<Expression> expr;

    inline ReturnStatement(std::unique_ptr<Expression>&& expr, const CodePosition& cp) :
        AstObject{ cp },
        Statement{ cp },
        expr{ std::move(expr) }
    {
    }

    virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&);
};


struct qlow::ast::AssignmentStatement : public Statement
{
    std::unique_ptr<Expression> target;
    std::unique_ptr<Expression> expr;

    inline AssignmentStatement(std::unique_ptr<Expression>&& target, std::unique_ptr<Expression>&& expr, const CodePosition& cp) :
        AstObject{ cp },
        Statement{ cp },
        target{ std::move(target) }, expr{ std::move(expr) }
    {
    }

    virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&);
};


struct qlow::ast::LocalVariableStatement : public Statement
{
    std::string name;
    std::unique_ptr<ast::Type> type;
    inline LocalVariableStatement(std::string&& name, std::unique_ptr<Type> type, const CodePosition& cp) :
        AstObject{ cp },
        Statement{ cp },
       name{ name },
       type{ std::move(type) }
    {
    } 

    virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&);
};


struct qlow::ast::IntConst : public Expression
{
    unsigned long long value;
    
    IntConst(unsigned long long v, const CodePosition& p) :
        AstObject(p),
        Expression(p),
        value{ v } {}
        
    IntConst(std::string&& val, const CodePosition& p);
    virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&);
};


struct qlow::ast::Operation : public Expression
{
    std::string opString;
    CodePosition opPos;

    inline Operation(const std::string& opString, const CodePosition& cp,
                     const CodePosition& opPos) :
        AstObject{ cp },
        Expression{ cp },
        opString{ opString },
        opPos{ opPos }
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

    inline UnaryOperation(std::unique_ptr<Expression> expr, Side side,
                          const std::string& op, const CodePosition& cp,
                          const CodePosition& opPos
                         ) :
        AstObject{ cp },
        Operation{ op, cp, opPos },
        side{ side },
        expr{ std::move(expr) }
    {
    }

    virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&);
};


struct qlow::ast::BinaryOperation : public Operation
{
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    inline BinaryOperation(std::unique_ptr<Expression> left,
                           std::unique_ptr<Expression> right,
                           const std::string& op,
                           const CodePosition& cp,
                           const CodePosition& opPos
                          ) :
        AstObject{ cp },
        Operation{ op, cp, opPos },
        left{ std::move(left) },
        right{ std::move(right) }
    {
    }

    virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&);
};


struct qlow::ast::NewArrayExpression : public Expression
{
    std::unique_ptr<ast::Type> type;
    std::unique_ptr<Expression> length;
    inline NewArrayExpression(std::unique_ptr<ast::Type> type,
                              std::unique_ptr<Expression> length,
                              const CodePosition& cp) :
        AstObject{ cp },
        Expression{ cp },
        type{ std::move(type) },
        length{ std::move(length) }
    {
    }
    
    virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&);
};


struct qlow::ast::CastExpression : public Expression
{
    std::unique_ptr<ast::Expression> expression;
    std::unique_ptr<ast::Type> targetType;
    
    inline CastExpression(std::unique_ptr<ast::Expression> expression,
                          std::unique_ptr<ast::Type> targetType,
                          const CodePosition& cp) :
        AstObject{ cp },
        Expression{ cp },
        expression{ std::move(expression) },
        targetType{ std::move(targetType) }
    {
    }
    
    virtual std::unique_ptr<sem::SemanticObject> accept(StructureVisitor& v, sem::Scope&);
};


#endif // QLOW_AST_H


