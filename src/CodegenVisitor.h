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
    class ExpressionCodegenVisitor;
    class LValueVisitor;
    class StatementVisitor;
}


class qlow::ExpressionCodegenVisitor :
    public Visitor<
        llvm::Value*,
        llvm::IRBuilder<>,

        sem::LocalVariableExpression,
        sem::UnaryOperation,
        sem::BinaryOperation,
        sem::CastExpression,
        sem::NewArrayExpression,
        sem::MethodCallExpression,
        sem::FieldAccessExpression,
        sem::IntConst,
        sem::ThisExpression
    >
{
public:
    gen::FunctionGenerator& fg;
    inline ExpressionCodegenVisitor(gen::FunctionGenerator& fg) :
        fg{ fg }
    {
    }
    
    llvm::Value* visit(sem::LocalVariableExpression& node, llvm::IRBuilder<>&) override;
    llvm::Value* visit(sem::UnaryOperation& node, llvm::IRBuilder<>&) override;
    llvm::Value* visit(sem::BinaryOperation& node, llvm::IRBuilder<>&) override;
    llvm::Value* visit(sem::CastExpression& node, llvm::IRBuilder<>&) override;
    llvm::Value* visit(sem::NewArrayExpression& node, llvm::IRBuilder<>&) override;
    llvm::Value* visit(sem::MethodCallExpression& node, llvm::IRBuilder<>&) override;
    llvm::Value* visit(sem::FieldAccessExpression& node, llvm::IRBuilder<>&) override;
    llvm::Value* visit(sem::IntConst& node, llvm::IRBuilder<>&) override;
    llvm::Value* visit(sem::ThisExpression& node, llvm::IRBuilder<>&) override;
};


class qlow::LValueVisitor :
    public Visitor<
        llvm::Value*,
        llvm::IRBuilder<>,

        sem::Expression,
        sem::LocalVariableExpression,
        sem::FieldAccessExpression
    >
{
public:
    llvm::Value* visit(sem::Expression& node, llvm::IRBuilder<>&) override;
    llvm::Value* visit(sem::LocalVariableExpression& node, llvm::IRBuilder<>&) override;
    llvm::Value* visit(sem::FieldAccessExpression& node, llvm::IRBuilder<>&) override;
};


class qlow::StatementVisitor :
    public Visitor<
        llvm::Value*,
        gen::FunctionGenerator,

        sem::DoEndBlock,
        sem::IfElseBlock,
        sem::WhileBlock,
        sem::AssignmentStatement,
        sem::ReturnStatement,
        sem::FeatureCallStatement
    >
{
public:
    llvm::Value* visit(sem::DoEndBlock& node, gen::FunctionGenerator&) override;
    llvm::Value* visit(sem::IfElseBlock& node, gen::FunctionGenerator&) override;
    llvm::Value* visit(sem::WhileBlock& node, gen::FunctionGenerator&) override;
    llvm::Value* visit(sem::AssignmentStatement& node, gen::FunctionGenerator&) override;
    llvm::Value* visit(sem::ReturnStatement& node, gen::FunctionGenerator&) override;
    llvm::Value* visit(sem::FeatureCallStatement& node, gen::FunctionGenerator&) override;
};




#endif // QLOW_CODEGEN_VISITOR_H



