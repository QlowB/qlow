#include "CodegenVisitor.h"
#include "CodeGeneration.h"

#include "Type.h"

#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>


using namespace qlow;

std::pair<llvm::Value*, sem::Type> ExpressionVisitor::visit(sem::LocalVariableExpression& lve, llvm::IRBuilder<>& builder)
{
    assert(lve.var->allocaInst != nullptr);
    if (llvm::dyn_cast<llvm::AllocaInst>(lve.var->allocaInst)) {
        llvm::Value* val = builder.CreateLoad(lve.var->allocaInst);
        return { val, lve.var->type };
    }
    else {
        return { lve.var->allocaInst, lve.var->type };
    }
}


std::pair<llvm::Value*, sem::Type> ExpressionVisitor::visit(sem::BinaryOperation& binop, llvm::IRBuilder<>& builder)
{
    using llvm::Value;
    using sem::Type;
    auto [left, leftType] = binop.left->accept(*this, builder);
    auto [right, rightType] = binop.right->accept(*this, builder);
    
    if (leftType != Type::INTEGER || rightType != Type::INTEGER)
        throw "invalid types in BinaryOperation";
    
    if (left == nullptr) {
        printf("WOW: %s\n", binop.left->toString().c_str());
    }
    
    
    switch (binop.op) {
        case ast::Operation::Operator::PLUS:
            return { builder.CreateAdd(left, right, "add"), Type::INTEGER };
        case ast::Operation::Operator::MINUS:
            return { builder.CreateSub(left, right, "sub"), Type::INTEGER };
        case ast::Operation::Operator::ASTERISK:
            return { builder.CreateMul(left, right, "mul"), Type::INTEGER };
        case ast::Operation::Operator::SLASH:
            return { builder.CreateSDiv(left, right, "add"), Type::INTEGER };
    }
}


std::pair<llvm::Value*, sem::Type> ExpressionVisitor::visit(sem::UnaryOperation& unop, llvm::IRBuilder<>& builder)
{
    using llvm::Value;
    auto [value, type] = unop.arg->accept(*this, builder);
    
    if (type != sem::Type::INTEGER)
        throw "invalid type to negate";

    switch (unop.op) {
        case ast::Operation::Operator::MINUS:
            return { builder.CreateNeg(value, "negate"), sem::Type::INTEGER };

        case ast::Operation::Operator::PLUS:
        [[fallthrough]];
        case ast::Operation::Operator::ASTERISK:
        [[fallthrough]];
        case ast::Operation::Operator::SLASH:
            throw "operator not supported";
    }
}

std::pair<llvm::Value*, sem::Type> ExpressionVisitor::visit(sem::FeatureCallExpression& call, llvm::IRBuilder<>& builder)
{
    using llvm::Value;
    std::vector<Value*> arguments;
    if (call.arguments.size() != call.callee->arguments.size()) {
        throw "wrong number of arguments";
    }
    for (size_t i = 0; i < call.arguments.size(); i++) {
        // : call.arguments) {
        auto& arg = call.arguments[i];
        auto [value, type] = arg->accept(*this, builder);
        
        if (type != call.callee->arguments[i]->type) {
            throw "argument type mismatch";
        }
        
        arguments.push_back(value);
    }
    auto returnType = call.callee->returnType;
    llvm::CallInst* callInst = builder.CreateCall(call.callee->llvmNode, arguments);
    
    return { callInst, returnType };
    //return { nullptr, sem::Type::NULL_TYPE };
}

std::pair<llvm::Value*, sem::Type> ExpressionVisitor::visit(sem::IntConst& node, llvm::IRBuilder<>& builder)
{
    return {
        llvm::ConstantInt::get(builder.getContext(), llvm::APInt(32, std::to_string(node.value), 10)),
        sem::Type::INTEGER
    };
}


llvm::Value* StatementVisitor::visit(sem::AssignmentStatement& assignment,
        qlow::gen::FunctionGenerator& fg)
{
    llvm::IRBuilder<> builder(fg.getContext());
    builder.SetInsertPoint(fg.getCurrentBlock());
    auto [val, type] = assignment.value->accept(fg.expressionVisitor, builder);
    if (auto* targetVar =
        dynamic_cast<sem::LocalVariableExpression*>(assignment.target.get()); targetVar) {
        builder.CreateStore(val, targetVar->var->allocaInst);
    }
    else
        throw "only local variables are assignable at the moment";
    return llvm::ConstantFP::get(fg.getContext(), llvm::APFloat(5123.0));
}


llvm::Value* StatementVisitor::visit(sem::ReturnStatement& returnStatement,
        qlow::gen::FunctionGenerator& fg)
{
    llvm::IRBuilder<> builder(fg.getContext());
    builder.SetInsertPoint(fg.getCurrentBlock());
    auto [val, type] = returnStatement.value->accept(fg.expressionVisitor, builder);
    builder.CreateRet(val);
    return val;
}


llvm::Value* StatementVisitor::visit(sem::FeatureCallStatement& fc, gen::FunctionGenerator& fg)
{
    llvm::Module* module = fg.getModule();
    llvm::IRBuilder<> builder(fg.getContext());
    builder.SetInsertPoint(fg.getCurrentBlock());
    //llvm::Constant* c = module->getOrInsertFunction(fc.expr->callee->name, {});
    llvm::Function* f = fc.expr->callee->llvmNode;
    builder.CreateCall(f, {});
    return llvm::ConstantFP::get(fg.getContext(), llvm::APFloat(5.0));
}




