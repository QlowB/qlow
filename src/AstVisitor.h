#ifndef QLOW_AST_VISITOR_H
#define QLOW_AST_VISITOR_H

#include "Visitor.h"
#include "Ast.h"
#include "Semantic.h"
#include "Scope.h"


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
        struct IntConst;

        struct Operation;
        struct UnaryOperation;
        struct BinaryOperation;
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
        ast::ArgumentDeclaration,
        ast::DoEndBlock,
        ast::Statement,
        ast::Expression,
        ast::FeatureCall,
        ast::AssignmentStatement,
        ast::NewVariableStatement,
        ast::IntConst,
        ast::UnaryOperation,
        ast::BinaryOperation
    >
{
public:
    using ReturnType = std::unique_ptr<sem::SemanticObject>;
    
    /*!
     * 
     * \returns <code>nullptr</code> if type is not found.
     */
    sem::Class* getType(const std::string& type, sem::Scope& scope);

    ReturnType visit(ast::Class& ast, sem::Scope& scope) override;
    ReturnType visit(ast::FeatureDeclaration& ast, sem::Scope& scope) override;
    ReturnType visit(ast::FieldDeclaration& ast, sem::Scope& scope) override;
    ReturnType visit(ast::MethodDefinition& ast, sem::Scope& scope) override;
    ReturnType visit(ast::VariableDeclaration& ast, sem::Scope& scope) override;
    ReturnType visit(ast::ArgumentDeclaration& ast, sem::Scope& scope) override;
    ReturnType visit(ast::DoEndBlock& ast, sem::Scope& scope) override;
    ReturnType visit(ast::Statement& ast, sem::Scope& scope) override;
    ReturnType visit(ast::Expression& ast, sem::Scope& scope) override;
    ReturnType visit(ast::FeatureCall& ast, sem::Scope& scope) override;
    ReturnType visit(ast::AssignmentStatement& ast, sem::Scope& scope) override;
    ReturnType visit(ast::NewVariableStatement& ast, sem::Scope& scope) override;
    ReturnType visit(ast::IntConst& ast, sem::Scope& scope) override;
    ReturnType visit(ast::UnaryOperation& ast, sem::Scope& scope) override;
    ReturnType visit(ast::BinaryOperation& ast, sem::Scope& scope) override;
};




#endif // QLOW_AST_VISITOR_H


