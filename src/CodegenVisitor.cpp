#include "CodegenVisitor.h"

#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>


using namespace qlow;

llvm::Value* CodegenVisitor::visit(sem::BinaryOperation& binop, llvm::IRBuilder<>& builder)
{
    using llvm::Value;
    Value* left = binop.left->accept(*this, builder);
    Value* right = binop.right->accept(*this, builder);
    switch (binop.op) {
        case ast::Operation::Operator::PLUS:
            return builder.CreateAdd(left, right, "add");
        case ast::Operation::Operator::MINUS:
            return builder.CreateSub(left, right, "sub");
        case ast::Operation::Operator::ASTERISK:
            return builder.CreateMul(left, right, "mul");
        case ast::Operation::Operator::SLASH:
            return builder.CreateSDiv(left, right, "add");
    }
}


llvm::Value* CodegenVisitor::visit(sem::UnaryOperation& unop, llvm::IRBuilder<>& builder)
{
    using llvm::Value;
}

llvm::Value* CodegenVisitor::visit(sem::FeatureCallExpression& binop, llvm::IRBuilder<>& builder)
{
    using llvm::Value;
}

llvm::Value* CodegenVisitor::visit(sem::IntConst& node, llvm::IRBuilder<>& builder)
{
    return llvm::ConstantInt::get(builder.getContext(), llvm::APInt(32, std::to_string(node.value), 10));
}

