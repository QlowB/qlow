#ifndef QLOW_AST_VISITOR_H
#define QLOW_AST_VISITOR_H

#include "Visitor.h"
#include "Ast.h"
#include "Semantic.h"
#include "Builtin.h"
#include "Type.h"
#include "Scope.h"


#include <memory>


namespace qlow
{
    namespace ast
    {
        template<typename T>
        using List = std::vector<std::unique_ptr<T>>;
    }
}


namespace qlow
{
    class StructureVisitor;
}


class qlow::StructureVisitor :
    public Visitor<
        std::unique_ptr<sem::SemanticObject>,
        sem::Scope&,

        ast::Class,
        ast::FeatureDeclaration,
        ast::FieldDeclaration,
        ast::MethodDefinition,
        ast::VariableDeclaration,
        ast::Statement,
        ast::DoEndBlock,
        ast::IfElseBlock,
        ast::WhileBlock,
        ast::Expression,
        ast::FeatureCall,
        ast::AssignmentStatement,
        ast::ReturnStatement,
        ast::LocalVariableStatement,
        ast::AddressExpression,
        ast::ArrayAccessExpression,
        ast::IntConst,
        ast::StringConst,
        ast::UnaryOperation,
        ast::BinaryOperation,
        ast::NewExpression,
        ast::NewArrayExpression,
        ast::CastExpression
    >
{
public:
    using ReturnType = std::unique_ptr<sem::SemanticObject>;

    ReturnType visit(ast::Class& ast, sem::Scope& scope) override;
    ReturnType visit(ast::FeatureDeclaration& ast, sem::Scope& scope) override;
    ReturnType visit(ast::FieldDeclaration& ast, sem::Scope& scope) override;
    ReturnType visit(ast::MethodDefinition& ast, sem::Scope& scope) override;
    ReturnType visit(ast::VariableDeclaration& ast, sem::Scope& scope) override;
    ReturnType visit(ast::Statement& ast, sem::Scope& scope) override;
    ReturnType visit(ast::DoEndBlock& ast, sem::Scope& scope) override;
    ReturnType visit(ast::IfElseBlock& ast, sem::Scope& scope) override;
    ReturnType visit(ast::WhileBlock& ast, sem::Scope& scope) override;
    ReturnType visit(ast::Expression& ast, sem::Scope& scope) override;
    ReturnType visit(ast::FeatureCall& ast, sem::Scope& scope) override;
    ReturnType visit(ast::AssignmentStatement& ast, sem::Scope& scope) override;
    ReturnType visit(ast::ReturnStatement& ast, sem::Scope& scope) override;
    ReturnType visit(ast::LocalVariableStatement& ast, sem::Scope& scope) override;
    ReturnType visit(ast::AddressExpression& ast, sem::Scope& scope) override;
    ReturnType visit(ast::ArrayAccessExpression& ast, sem::Scope& scope) override;
    ReturnType visit(ast::IntConst& ast, sem::Scope& scope) override;
    ReturnType visit(ast::StringConst& ast, sem::Scope& scope) override;
    ReturnType visit(ast::UnaryOperation& ast, sem::Scope& scope) override;
    ReturnType visit(ast::BinaryOperation& ast, sem::Scope& scope) override;
    ReturnType visit(ast::NewExpression& ast, sem::Scope& scope) override;
    ReturnType visit(ast::NewArrayExpression& ast, sem::Scope& scope) override;
    ReturnType visit(ast::CastExpression& ast, sem::Scope& scope) override;

    std::unique_ptr<sem::Expression> createImplicitCast(std::unique_ptr<sem::Expression>, sem::Type* targetType, sem::Scope& scope);
};




#endif // QLOW_AST_VISITOR_H


