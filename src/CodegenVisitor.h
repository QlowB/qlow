#ifndef QLOW_CODEGEN_VISITOR_H
#define QLOW_CODEGEN_VISITOR_H

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>

#include "Visitor.h"
#include "Semantic.h"
#include "Scope.h"
#include "Type.h"


#include <memory>


namespace qlow
{
    namespace gen
    {
        class FunctionGenerator;        
    }
}


namespace qlow
{
    class ExpressionVisitor;
    class StatementVisitor;
}


class qlow::ExpressionVisitor :
    public Visitor<
        std::pair<llvm::Value*, sem::Type>,
        llvm::IRBuilder<>,

        sem::LocalVariableExpression,
        sem::BinaryOperation,
        sem::UnaryOperation,
        sem::FeatureCallExpression,
        sem::IntConst
    >
{
public:
    std::pair<llvm::Value*, sem::Type> visit(sem::LocalVariableExpression& node, llvm::IRBuilder<>&) override;
    std::pair<llvm::Value*, sem::Type> visit(sem::BinaryOperation& node, llvm::IRBuilder<>&) override;
    std::pair<llvm::Value*, sem::Type> visit(sem::UnaryOperation& node, llvm::IRBuilder<>&) override;
    std::pair<llvm::Value*, sem::Type> visit(sem::FeatureCallExpression& node, llvm::IRBuilder<>&) override;
    std::pair<llvm::Value*, sem::Type> visit(sem::IntConst& node, llvm::IRBuilder<>&) override;
};


class qlow::StatementVisitor :
    public Visitor<
        llvm::Value*,
        gen::FunctionGenerator,

        sem::AssignmentStatement,
        sem::FeatureCallStatement
    >
{
public:
    llvm::Value* visit(sem::AssignmentStatement& node, gen::FunctionGenerator&) override;
    llvm::Value* visit(sem::FeatureCallStatement& node, gen::FunctionGenerator&) override;
};

#endif // QLOW_CODEGEN_VISITOR_H



