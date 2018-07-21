#include "CodegenVisitor.h"
#include "CodeGeneration.h"

#include "Type.h"
#include "Builtin.h"

#include "ErrorReporting.h"

#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>


using namespace qlow;

llvm::Value* ExpressionCodegenVisitor::visit(sem::LocalVariableExpression& lve, llvm::IRBuilder<>& builder)
{
    assert(lve.var->allocaInst != nullptr);
    if (llvm::dyn_cast<llvm::AllocaInst>(lve.var->allocaInst)) {
        llvm::Value* val = builder.CreateLoad(lve.var->allocaInst);
        return val;
    }
    else {
        return lve.var->allocaInst;
    }
}


llvm::Value* ExpressionCodegenVisitor::visit(sem::UnaryOperation& unop, llvm::IRBuilder<>& builder)
{
    using llvm::Value;
    auto value = unop.arg->accept(*this, builder);
    auto& type = unop.arg->type;
    
    if (type->equals(sem::NativeType(sem::NativeType::Type::VOID)))
        throw "invalid type to negate";

    /*
    switch (unop.op) {
        case ast::Operation::Operator::MINUS:
            return builder.CreateNeg(value, "negate");
        case ast::Operation::Operator::NOT:
            return builder.CreateNot(value, "not");
        default:
            throw "operator not supported";
    }*/
}


llvm::Value* ExpressionCodegenVisitor::visit(sem::BinaryOperation& binop, llvm::IRBuilder<>& builder)
{
    using llvm::Value;
    using sem::Type;
    auto left = binop.left->accept(*this, builder);
    auto right = binop.right->accept(*this, builder);
    
    auto& leftType = binop.left->type;
    auto& rightType = binop.right->type;
    
    sem::Method* operation = binop.operationMethod;
    
    if (operation != nullptr) {
        if (sem::NativeMethod* nm = dynamic_cast<sem::NativeMethod*>(operation); nm) {
            return nm->generateCode(builder, {left, right});
        }
        else
            throw "only native operations supported at the moment";
    }
    else {
        throw "internal error: operation method null";
    }
        
    if (left == nullptr) {
        printf("WOW: %s\n", binop.left->toString().c_str());
    }
    
    Value* implicitelyCastedRight = right;
    if (!leftType->equals(*rightType))
        implicitelyCastedRight = dynamic_cast<sem::NativeType*>(leftType.get())->generateImplicitCast(right);
    
    if (dynamic_cast<sem::NativeType*>(leftType.get())->isIntegerType()) {
        // TODO allow integer operations
    }
    
   /* 
    // TODO insert type checks
    switch (binop.op) {
        case ast::Operation::Operator::PLUS:
            return builder.CreateAdd(left, right, "add");
        case ast::Operation::Operator::MINUS:
            return builder.CreateSub(left, right, "sub");
        case ast::Operation::Operator::ASTERISK:
            return builder.CreateMul(left, right, "mul");
        case ast::Operation::Operator::SLASH:
            return builder.CreateSDiv(left, right, "sdiv");
            
        case ast::Operation::Operator::AND:
            return builder.CreateAnd(left, right, "and");
        case ast::Operation::Operator::OR:
            return builder.CreateOr(left, right, "or");
        case ast::Operation::Operator::XOR:
            return builder.CreateXor(left, right, "xor");
            
        case ast::Operation::Operator::EQUALS:
            return builder.CreateICmpEQ(left, right, "equals");
        case ast::Operation::Operator::NOT_EQUALS:
            return builder.CreateICmpNE(left, right, "not_equals");
    }*/
}


llvm::Value* ExpressionCodegenVisitor::visit(sem::CastExpression& cast, llvm::IRBuilder<>& builder)
{
    return builder.CreateCast(
        llvm::Instruction::CastOps::SExt,
        cast.expression->accept(*this, builder),
        cast.targetType->getLlvmType(builder.getContext())
    );
}


llvm::Value* ExpressionCodegenVisitor::visit(sem::NewArrayExpression& naexpr, llvm::IRBuilder<>& builder)
{
    using llvm::Value;
    // TODO implement
}

llvm::Value* ExpressionCodegenVisitor::visit(sem::FeatureCallExpression& call, llvm::IRBuilder<>& builder)
{
    using llvm::Value;
    std::vector<Value*> arguments;
    if (call.arguments.size() != call.callee->arguments.size()) {
        throw "wrong number of arguments";
    }
    for (size_t i = 0; i < call.arguments.size(); i++) {
        // : call.arguments) {
        auto& arg = call.arguments[i];
        auto value = arg->accept(*this, builder);
        
        if (!arg->type->equals(*call.callee->arguments[i]->type.get())) {
            throw "argument type mismatch";
        }
        
        arguments.push_back(value);
    }
    auto returnType = call.callee->returnType;
    llvm::CallInst* callInst = builder.CreateCall(call.callee->llvmNode, arguments);
    
    return callInst;
}

llvm::Value* ExpressionCodegenVisitor::visit(sem::IntConst& node, llvm::IRBuilder<>& builder)
{
    return llvm::ConstantInt::get(builder.getContext(),
        llvm::APInt(32, std::to_string(node.value), 10));
}


llvm::Value* StatementVisitor::visit(sem::DoEndBlock& assignment,
        qlow::gen::FunctionGenerator& fg)
{
    for (auto& statement : assignment.statements) {
        statement->accept(*this, fg);
    }
    return nullptr;
}


llvm::Value* StatementVisitor::visit(sem::IfElseBlock& ifElseBlock,
        qlow::gen::FunctionGenerator& fg)
{
    using llvm::Value;
    using llvm::BasicBlock;
    
    llvm::IRBuilder<> builder(fg.getContext());
    builder.SetInsertPoint(fg.getCurrentBlock());
    auto condition = ifElseBlock.condition->accept(fg.expressionVisitor, builder);
    
    llvm::Function* function = fg.getCurrentBlock()->getParent();
    
    BasicBlock* thenB = BasicBlock::Create(fg.getContext(), "then", function);
    BasicBlock* elseB = BasicBlock::Create(fg.getContext(), "else", function);
    BasicBlock* merge = BasicBlock::Create(fg.getContext(), "merge", function);
    
    Value* boolCond = builder.CreateIntCast(condition, llvm::Type::getInt1Ty(fg.getContext()), false);
    
    builder.CreateCondBr(boolCond, thenB, elseB);  
    
    fg.pushBlock(thenB);
    ifElseBlock.ifBlock->accept(*this, fg);
    builder.SetInsertPoint(thenB);
    if (!thenB->getTerminator())
        builder.CreateBr(merge);
    fg.popBlock();
    fg.pushBlock(elseB);
    ifElseBlock.elseBlock->accept(*this, fg);
    builder.SetInsertPoint(elseB);
    if (!elseB->getTerminator())
        builder.CreateBr(merge);
    fg.popBlock();
    fg.popBlock();
    fg.pushBlock(merge);
}


llvm::Value* StatementVisitor::visit(sem::WhileBlock& whileBlock,
        qlow::gen::FunctionGenerator& fg)
{
    using llvm::Value;
    using llvm::BasicBlock;
    
    llvm::IRBuilder<> builder(fg.getContext());
    builder.SetInsertPoint(fg.getCurrentBlock());
    
    llvm::Function* function = fg.getCurrentBlock()->getParent();
    
    BasicBlock* startloop = BasicBlock::Create(fg.getContext(), "startloop", function);
    BasicBlock* body = BasicBlock::Create(fg.getContext(), "loopbody", function);
    BasicBlock* merge = BasicBlock::Create(fg.getContext(), "merge", function);
    
    
    //builder.CreateCondBr(boolCond, body, merge);
    builder.CreateBr(startloop);
    fg.pushBlock(startloop);
    builder.SetInsertPoint(startloop);
    auto condition = whileBlock.condition->accept(fg.expressionVisitor, builder);
    Value* boolCond = builder.CreateIntCast(condition, llvm::Type::getInt1Ty(fg.getContext()), false);
    builder.CreateCondBr(condition, body, merge);
    fg.popBlock();
    
    fg.pushBlock(body);
    whileBlock.body->accept(*this, fg);
    builder.SetInsertPoint(body);
    builder.CreateBr(startloop);
    fg.popBlock();
    fg.pushBlock(merge);
}


llvm::Value* StatementVisitor::visit(sem::AssignmentStatement& assignment,
        qlow::gen::FunctionGenerator& fg)
{
    Logger& logger = Logger::getInstance();
    llvm::IRBuilder<> builder(fg.getContext());
    builder.SetInsertPoint(fg.getCurrentBlock());
    auto val = assignment.value->accept(fg.expressionVisitor, builder);
    if (auto* targetVar =
        dynamic_cast<sem::LocalVariableExpression*>(assignment.target.get()); targetVar) {
        logger.debug() << "assigning to LocalVariableExpression" << std::endl;
        builder.CreateStore(val, targetVar->var->allocaInst);
    }
    else if (auto* targetVar =
        dynamic_cast<sem::FeatureCallExpression*>(assignment.target.get()); targetVar) {
        
        logger.debug() << "assigning to FeatureCallExpression" << std::endl;
    }
    else {
        
        logger.debug() << "assigning to instance of " << assignment.target->toString() << std::endl;
        throw "only local variables are assignable at the moment";
    }
    
    return nullptr;
    //return llvm::ConstantFP::get(fg.getContext(), llvm::APFloat(5123.0));
}


llvm::Value* StatementVisitor::visit(sem::ReturnStatement& returnStatement,
        qlow::gen::FunctionGenerator& fg)
{
    llvm::IRBuilder<> builder(fg.getContext());
    builder.SetInsertPoint(fg.getCurrentBlock());
    auto val = returnStatement.value->accept(fg.expressionVisitor, builder);
    builder.CreateRet(val);
    return val;
}


llvm::Value* StatementVisitor::visit(sem::FeatureCallStatement& fc, gen::FunctionGenerator& fg)
{
    llvm::Module* module = fg.getModule();
    llvm::IRBuilder<> builder(fg.getContext());
    builder.SetInsertPoint(fg.getCurrentBlock());
    //llvm::Constant* c = module->getOrInsertFunction(fc.expr->callee->name, {});
    
    return fc.expr->accept(fg.expressionVisitor, builder);
    
    /*
    llvm::Function* f = fc.expr->callee->llvmNode;
    std::vector<llvm::Value*> arguments;
    for (auto& arg : fc.expr->arguments) {
        arguments
    }
    builder.CreateCall(f, {});
    */
    // return llvm::ConstantFP::get(fg.getContext(), llvm::APFloat(5.0));
}




