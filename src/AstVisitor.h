#ifndef QLOW_AST_VISITOR_H
#define QLOW_AST_VISITOR_H

#include "Visitor.h"
#include "Ast.h"
#include "Semantic.h"

#include <memory>


namespace qlow
{
    namespace ast
    {
        template<typename T>
        using List = std::vector<std::unique_ptr<T>>;

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
}


namespace qlow
{
    class AstVisitor;
}


class qlow::AstVisitor :
    public Visitor<
        std::unique_ptr<sem::SemanticObject>,

        ast::Class,
        ast::FeatureDeclaration,
        ast::FieldDeclaration,
        ast::MethodDefinition,
        ast::VariableDeclaration,
        ast::ArgumentDeclaration,
        ast::DoEndBlock,
        ast::Statement,
        ast::Expression,
        ast::FeatureCall,
        ast::AssignmentStatement,
        ast::NewVariableStatement,
        ast::Operation,
        ast::UnaryOperation,
        ast::BinaryOperation
    >
{
public:
    using ReturnType = std::unique_ptr<sem::SemanticObject>;

    ReturnType visit(ast::Class& ast) override;
    ReturnType visit(ast::FeatureDeclaration& ast) override;
    ReturnType visit(ast::FieldDeclaration& ast) override;
    ReturnType visit(ast::MethodDefinition& ast) override;
    ReturnType visit(ast::VariableDeclaration& ast) override;
    ReturnType visit(ast::ArgumentDeclaration& ast) override;
    ReturnType visit(ast::DoEndBlock& ast) override;
    ReturnType visit(ast::Statement& ast) override;
    ReturnType visit(ast::Expression& ast) override;
    ReturnType visit(ast::FeatureCall& ast) override;
    ReturnType visit(ast::AssignmentStatement& ast) override;
    ReturnType visit(ast::NewVariableStatement& ast) override;
    ReturnType visit(ast::Operation& ast) override;
    ReturnType visit(ast::UnaryOperation& ast) override;
    ReturnType visit(ast::BinaryOperation& ast) override;
};




#endif // QLOW_AST_VISITOR_H


