#include "CodegenVisitor.h"
#include "CodeGeneration.h"

#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>


using namespace qlow;

llvm::Value* ExpressionVisitor::visit(sem::BinaryOperation& binop, llvm::IRBuilder<>& builder)
{
    using llvm::Value;
    Value* left = binop.left->accept(*this, builder);
    Value* right = binop.right->accept(*this, builder);
    if (left == nullptr) {
        printf("WOW: %s\n", binop.left->toString().c_str());
    }
    
    
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


llvm::Value* ExpressionVisitor::visit(sem::UnaryOperation& unop, llvm::IRBuilder<>& builder)
{
    using llvm::Value;
    Value* value = unop.arg->accept(*this, builder);

    switch (unop.op) {
        case ast::Operation::Operator::MINUS:
            return builder.CreateNeg(value, "negate");

        case ast::Operation::Operator::PLUS:
        [[fallthrough]]
        case ast::Operation::Operator::ASTERISK:
        [[fallthrough]]
        case ast::Operation::Operator::SLASH:
            throw "operator not supported";
    }
}

llvm::Value* ExpressionVisitor::visit(sem::FeatureCallExpression& call, llvm::IRBuilder<>& builder)
{
    using llvm::Value;
    std::vector<Value*> arguments;
    for (auto& arg : call.arguments) {
        arguments.push_back(arg->accept(*this, builder));
    }
    //return builder.CreateCall(nullptr, arguments);
    return nullptr;
}

llvm::Value* ExpressionVisitor::visit(sem::IntConst& node, llvm::IRBuilder<>& builder)
{
    return llvm::ConstantInt::get(builder.getContext(), llvm::APInt(32, std::to_string(node.value), 10));
}


llvm::Value* StatementVisitor::visit(sem::AssignmentStatement& assignment,
        qlow::gen::FunctionGenerator& fg)
{
    llvm::IRBuilder<> builder(fg.getContext());
    builder.SetInsertPoint(fg.getCurrentBlock());
    llvm::Value* val = assignment.value->accept(fg.expressionVisitor, builder);
    builder.CreateRet(val);
    return llvm::ConstantFP::get(fg.getContext(), llvm::APFloat(5.0));
}


llvm::Value* StatementVisitor::visit(sem::FeatureCallStatement& fc, gen::FunctionGenerator& fg)
{
    llvm::Module* module = fg.getModule();
    llvm::IRBuilder<> builder(fg.getContext());
    builder.SetInsertPoint(fg.getCurrentBlock());
    llvm::Constant* c = module->getOrInsertFunction(fc.expr->callee->name, {});
    builder.CreateCall(c, {});
    return llvm::ConstantFP::get(fg.getContext(), llvm::APFloat(5.0));
}




