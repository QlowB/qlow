#include "CodegenVisitor.h"
#include "CodeGeneration.h"

#include "Type.h"
#include "Builtin.h"

#include "ErrorReporting.h"

#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/Support/raw_os_ostream.h>


using namespace qlow;

llvm::Value* ExpressionCodegenVisitor::visit(sem::LocalVariableExpression& lve, llvm::IRBuilder<>& builder)
{
    assert(lve.var->allocaInst != nullptr);
    // TODO improve handling of arrays and structs
    if (llvm::dyn_cast<llvm::AllocaInst>(lve.var->allocaInst) && !lve.type->isArrayType() && !lve.type->isStructType()) {
        llvm::Type* returnType = lve.type->getLlvmType(builder.getContext());
        llvm::Value* val = builder.CreateLoad(returnType, lve.var->allocaInst);
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
    
    if (type == nullptr)//(type->equals(sem::NativeType(sem::NativeType::Type::VOID)))
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
    return nullptr;
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
        // TODO rewrite
        if (sem::NativeMethod* nm = dynamic_cast<sem::NativeMethod*>(operation); nm) {
            return nm->generateCode(builder, {left, right});
        }
        else
            throw "only native operations supported at the moment";
    }
    else {
        throw "internal error: operation method null";
    }

    // unreachable
        
    if (left == nullptr) {
        printf("WOW: %s\n", binop.left->toString().c_str());
    }
    
    Value* implicitelyCastedRight = right;
    // TODO rewritten types
    //if (leftType != rightType))
    //    implicitelyCastedRight = dynamic_cast<sem::NativeType*>(leftType.get())->generateImplicitCast(right);
    
    /*
    if (dynamic_cast<sem::NativeType*>(leftType.get())->isIntegerType()) {
        // TODO allow integer operations
    }*/
    
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
    if (cast.isNativeCast) {
        return builder.CreateCast(
            llvm::Instruction::CastOps::SExt,
            cast.expression->accept(*this, builder),
            cast.targetType->getLlvmType(builder.getContext())
        );
    }
    return nullptr;
}


llvm::Value* ExpressionCodegenVisitor::visit(sem::NewExpression& nexpr, llvm::IRBuilder<>& builder)
{
    using llvm::Value;

    sem::Context& semCtxt = nexpr.context;
    sem::Type* type = nexpr.type;

    const llvm::DataLayout& layout = builder.GetInsertBlock()->getModule()->getDataLayout();
    llvm::Type* llvmTy = type->getLlvmType(builder.getContext())->getPointerElementType();
    auto allocSize = layout.getTypeAllocSize(llvmTy);

    auto size = llvm::ConstantInt::get(builder.getContext(), llvm::APInt(32, allocSize, false));
    auto mallocCall = llvm::CallInst::CreateMalloc(builder.GetInsertBlock(), size->getType(), llvmTy, size, nullptr, nullptr, "");
    //auto casted = builder.CreateBitCast(mallocCall, llvmTy);
    builder.GetInsertBlock()->getInstList().push_back(llvm::cast<llvm::Instruction>(mallocCall));
    return mallocCall;
}


llvm::Value* ExpressionCodegenVisitor::visit(sem::NewArrayExpression& naexpr, llvm::IRBuilder<>& builder)
{
    using llvm::Value;

    sem::Context& semCtxt = naexpr.context;
    llvm::LLVMContext& llvmCtxt = builder.getContext();

    const llvm::DataLayout& layout = builder.GetInsertBlock()->getModule()->getDataLayout();
    llvm::Type* llvmTy = naexpr.elementType->getLlvmType(llvmCtxt);
    llvm::Type* arrayStructType = naexpr.type->getLlvmType(llvmCtxt);
    auto elementSize = layout.getTypeAllocSize(llvmTy);

    llvm::Value* lengthExpr = naexpr.length->accept(*this, builder);
    llvm::Value* allocSize = builder.CreateMul(lengthExpr, llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvmCtxt), elementSize));

    auto mallocCall = llvm::CallInst::CreateMalloc(builder.GetInsertBlock(), allocSize->getType(), llvmTy, allocSize, nullptr, nullptr, "");
    //auto casted = builder.CreateBitCast(mallocCall, llvmTy);
    builder.GetInsertBlock()->getInstList().push_back(llvm::cast<llvm::Instruction>(mallocCall));

    llvm::Value* result = builder.CreateAlloca(arrayStructType);
    llvm::Value* arrRef = builder.CreateStructGEP(arrayStructType, result, 0);
    llvm::Value* lenRef = builder.CreateStructGEP(arrayStructType, result, 1);

    builder.CreateStore(mallocCall, arrRef);
    builder.CreateStore(lengthExpr, lenRef);

    return result; // builder.CreateGEP(result, llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvmCtxt), 0));
}


llvm::Value* ExpressionCodegenVisitor::visit(sem::MethodCallExpression& call, llvm::IRBuilder<>& builder)
{
    using llvm::Value;

    sem::Context& semCtxt = call.context;

    if (call.arguments.size() != call.callee->arguments.size()) {
        throw "wrong number of arguments";
    }
    
    std::vector<Value*> arguments;
    
    if (call.target != nullptr) {
        auto* target = call.target->accept(*this, builder);

#ifdef DEBUGGING
        Printer::getInstance() << "creating 'this' argument";
#endif
        /*if (llvm::LoadInst* li = llvm::dyn_cast<llvm::LoadInst>(target); li) {
            llvm::Value* ptr = builder.CreateLoad(semCtxt.getLlvmType(call.target->type, builder.getContext()), li, "ptrload");
            arguments.push_back(ptr);
        } else*/
            arguments.push_back(target);
    }
    
    for (size_t i = 0; i < call.arguments.size(); i++) {
        // : call.arguments) {
        auto& arg = call.arguments[i];
        auto value = arg->accept(*this, builder);
        
        if (arg->type != call.callee->arguments[i]->type) {
            throw "argument type mismatch";
        }
        
        arguments.push_back(value);
    }
    //auto returnType = call.callee->returnType;
    llvm::CallInst* callInst = builder.CreateCall(call.callee->llvmNode, arguments);
    return callInst;
}


llvm::Value* ExpressionCodegenVisitor::visit(sem::FieldAccessExpression& access, llvm::IRBuilder<>& builder)
{
    using llvm::Value;
    using llvm::Type;

    sem::Context& semCtxt = access.context;
    
    Type* type = access.target->type->getLlvmType(builder.getContext());
    
    if (type == nullptr)
        throw "no access type";
    if (type->isPointerTy()) {
        type = type->getPointerElementType();
    }

    llvm::Value* target = access.target->accept(fg.expressionVisitor, builder);

    int structIndex = access.accessed->llvmStructIndex;
    /*llvm::ArrayRef<Value*> indexList = {
        llvm::ConstantInt::get(builder.getContext(), llvm::APInt(32, structIndex, false)),
        llvm::ConstantInt::get(builder.getContext(), llvm::APInt(32, 0, false))
    };*/

    //Value* ptr = builder.CreateGEP(type, target, indexList);
    Value* ptr = fg.builder.CreateStructGEP(type, target, structIndex);
    return builder.CreateLoad(ptr);
    
    
    //builder.CreateStructGEP(type,
    //                               llvm::ConstantInt::get(builder.getContext(),
    //                               llvm::APInt(32, 0, false)), 0);
    return llvm::ConstantInt::get(builder.getContext(),
                                   llvm::APInt(32, 0, false));
}


llvm::Value* ExpressionCodegenVisitor::visit(sem::AddressExpression& node, llvm::IRBuilder<>& builder)
{
    using llvm::Value;
    Value* lvalue = node.target->accept(fg.lvalueVisitor, fg);
    
    // this check is unnecessary
    if (auto* allocaInst = llvm::dyn_cast<llvm::AllocaInst>(lvalue)) {
        return lvalue;
    }
    else {
        return lvalue;
    }
}


llvm::Value* ExpressionCodegenVisitor::visit(sem::ArrayAccessExpression& node, llvm::IRBuilder<>& builder)
{
    auto array = node.array->accept(*this, builder);
    auto index = node.index->accept(*this, builder);

    auto arrType = node.array->type;
    if (!arrType->isArrayType()) {
        throw "trying to access non-array type as array.";
    }

    sem::ArrayType* at = static_cast<sem::ArrayType*>(arrType);
    auto elemType = at->getArrayOf();

    auto arrPtr = builder.CreateStructGEP(at->getLlvmType(builder.getContext()), array, 0);
    // TODO implement range checks
    //auto length = builder.CreateStructGEP(at->getLlvmType(builder.getContext()), array, 1);

    auto arr = builder.CreateLoad(arrPtr);
    auto accessVal = builder.CreateGEP(arr, index);

    return builder.CreateLoad(accessVal);
}


llvm::Value* ExpressionCodegenVisitor::visit(sem::IntConst& node, llvm::IRBuilder<>& builder)
{
    return llvm::ConstantInt::get(builder.getContext(),
        llvm::APInt(64, std::to_string(node.value), 10));
}


llvm::Value* ExpressionCodegenVisitor::visit(sem::ThisExpression& thisExpr, llvm::IRBuilder<>& builder)
{
    return thisExpr.allocaInst;
}


llvm::Value* LValueVisitor::visit(sem::Expression& e, qlow::gen::FunctionGenerator& fg)
{
    throw "cannot construct lvalue from expression";
}


llvm::Value* LValueVisitor::visit(sem::LocalVariableExpression& lve, qlow::gen::FunctionGenerator& fg)
{
    assert(lve.var->allocaInst != nullptr);
    
    if (llvm::dyn_cast<llvm::AllocaInst>(lve.var->allocaInst)) {
        return lve.var->allocaInst;
        
        /*llvm::Value* val = builder.CreateLoad(
            lve.type->getLlvmType(builder.getContext())->getPointerTo(),
            lve.var->allocaInst
        );
        return val;*/
    }
    else if (llvm::dyn_cast<llvm::PointerType> (lve.var->allocaInst->getType())) {
        return lve.var->allocaInst;
    }
    else {
        throw "unable to find alloca instance of local variable";
        //return lve.var->allocaInst;
    }
}


llvm::Value* LValueVisitor::visit(sem::FieldAccessExpression& access, qlow::gen::FunctionGenerator& fg)
{
    /*
    using llvm::Value;
    using llvm::Type;
    
    auto& fieldType = fae.accessed->type;
    Type* ptr = fieldType->getLlvmType(builder.getContext())->getPointerTo();
    
    llvm::Value* allocaInst = fae.target->accept(*this, builder);
    
    if (ptr == nullptr)
        throw "no access type";
    
    llvm::ArrayRef<Value*> indices = {
        llvm::ConstantInt::get(builder.getContext(), llvm::APInt(32, 0, false)),
        llvm::ConstantInt::get(builder.getContext(), llvm::APInt(32, 0, false))
    };
    */
    
    using llvm::Value;
    using llvm::Type;
    sem::Context& semCtxt = access.context;
    
    Type* type = access.target->type->getLlvmType(fg.builder.getContext());
    
    if (type == nullptr)
        throw "no access type";
    if (type->isPointerTy()) {
        type = type->getPointerElementType();
    }
    
    llvm::Value* target = access.target->accept(fg.expressionVisitor, fg.builder);
    
    int structIndex = access.accessed->llvmStructIndex;
    /*llvm::ArrayRef<Value*> indexList = {
        llvm::ConstantInt::get(fg.builder.getContext(), llvm::APInt(32, structIndex, false)),
        llvm::ConstantInt::get(fg.builder.getContext(), llvm::APInt(32, 0, false))
    };*/
    //Value* ptr = fg.builder.CreateGEP(type, target, indexList);
    Value* ptr = fg.builder.CreateStructGEP(type, target, structIndex);
    return ptr;
}


llvm::Value* LValueVisitor::visit(sem::ArrayAccessExpression& node, qlow::gen::FunctionGenerator& fg)
{
    auto& builder = fg.builder;
    auto array = node.array->accept(fg.expressionVisitor, builder);
    auto index = node.index->accept(fg.expressionVisitor, builder);

    auto arrType = node.array->type;
    if (!arrType->isArrayType()) {
        throw "trying to access non-array type as array.";
    }

    //auto ostr = llvm::raw_os_ostream(Printer::getInstance());
    //fg.getModule()->print(ostr, nullptr);

    auto elemType = arrType->getArrayOf();

    auto arrPtr = builder.CreateStructGEP(arrType->getLlvmType(builder.getContext()), array, 0);
    // TODO implement range check
    //auto length = builder.CreateStructGEP(arrType->getLlvmType(builder.getContext()), array, 1);

    auto arr = builder.CreateLoad(arrPtr);

    auto accessVal = builder.CreateGEP(arr, index);

    return accessVal;
}


llvm::Value* StatementVisitor::visit(sem::DoEndBlock& block,
        qlow::gen::FunctionGenerator& fg)
{
    for (auto& statement : block.statements) {
        statement->accept(*this, fg);
    }
    return nullptr;
}


llvm::Value* StatementVisitor::visit(sem::IfElseBlock& ifElseBlock,
        qlow::gen::FunctionGenerator& fg)
{
    using llvm::Value;
    using llvm::BasicBlock;
    

    fg.builder.SetInsertPoint(fg.getCurrentBlock());
    auto condition = ifElseBlock.condition->accept(fg.expressionVisitor, fg.builder);
    
    llvm::Function* function = fg.getCurrentBlock()->getParent();
    
    BasicBlock* thenB = BasicBlock::Create(fg.getContext(), "then", function);
    BasicBlock* elseB = BasicBlock::Create(fg.getContext(), "else", function);
    BasicBlock* merge = BasicBlock::Create(fg.getContext(), "merge", function);
    
    Value* boolCond = fg.builder.CreateIntCast(condition, llvm::Type::getInt1Ty(fg.getContext()), false);
    
    fg.builder.CreateCondBr(boolCond, thenB, elseB);  
    
    fg.pushBlock(thenB);
    ifElseBlock.ifBlock->accept(*this, fg);
    fg.builder.SetInsertPoint(thenB);
    if (!thenB->getTerminator())
        fg.builder.CreateBr(merge);
    fg.popBlock();
    fg.pushBlock(elseB);
    ifElseBlock.elseBlock->accept(*this, fg);
    fg.builder.SetInsertPoint(elseB);
    if (!elseB->getTerminator())
        fg.builder.CreateBr(merge);
    fg.popBlock();
    fg.popBlock();
    fg.pushBlock(merge);
    return nullptr;
}


llvm::Value* StatementVisitor::visit(sem::WhileBlock& whileBlock,
        qlow::gen::FunctionGenerator& fg)
{
    using llvm::Value;
    using llvm::BasicBlock;
    
    fg.builder.SetInsertPoint(fg.getCurrentBlock());
    
    llvm::Function* function = fg.getCurrentBlock()->getParent();
    
    BasicBlock* startloop = BasicBlock::Create(fg.getContext(), "startloop", function);
    BasicBlock* body = BasicBlock::Create(fg.getContext(), "loopbody", function);
    BasicBlock* merge = BasicBlock::Create(fg.getContext(), "merge", function);
    
    
    //builder.CreateCondBr(boolCond, body, merge);
    fg.builder.CreateBr(startloop);
    fg.pushBlock(startloop);
    fg.builder.SetInsertPoint(startloop);
    auto condition = whileBlock.condition->accept(fg.expressionVisitor, fg.builder);
    Value* boolCond = fg.builder.CreateIntCast(condition, llvm::Type::getInt1Ty(fg.getContext()), false);
    fg.builder.CreateCondBr(condition, body, merge);
    fg.popBlock();
    
    fg.pushBlock(body);
    whileBlock.body->accept(*this, fg);
    fg.builder.SetInsertPoint(body);
    fg.builder.CreateBr(startloop);
    fg.popBlock();
    fg.pushBlock(merge);
    return nullptr;
}


llvm::Value* StatementVisitor::visit(sem::AssignmentStatement& assignment,
        qlow::gen::FunctionGenerator& fg)
{
    Printer& printer = Printer::getInstance();
    fg.builder.SetInsertPoint(fg.getCurrentBlock());
    
    auto val = assignment.value->accept(fg.expressionVisitor, fg.builder);
    auto target = assignment.target->accept(fg.lvalueVisitor, fg);
    
    if (!assignment.value->type->isNativeType() && !assignment.value->type->isReferenceType()) {
        const llvm::DataLayout& layout = fg.builder.GetInsertBlock()->getModule()->getDataLayout();
#if LLVM_VERSION_MAJOR >= 7
        return fg.builder.CreateMemCpy(target, llvm::MaybeAlign(), val, llvm::MaybeAlign(), layout.getTypeAllocSize(val->getType()->getPointerElementType()), false);
#else
        return fg.builder.CreateMemCpy(target, val, layout.getTypeAllocSize(val->getType()->getPointerElementType()), 1);
#endif
    }
    else {
        return fg.builder.CreateStore(val, target);
    }
    
    /*
    if (auto* targetVar =
        dynamic_cast<sem::LocalVariableExpression*>(assignment.target.get()); targetVar) {
#ifdef DEBUGGING
        printer << "assigning to LocalVariableExpression" << std::endl;
#endif
        builder.CreateStore(val, targetVar->var->allocaInst);
    }
    else if (auto* targetVar =
        dynamic_cast<sem::FieldAccessExpression*>(assignment.target.get()); targetVar) {
        
#ifdef DEBUGGING
        printer << "assigning to FieldAccessExpression" << std::endl;
#endif
        if (targetVar->target) {
            llvm::Value* target = targetVar->target->accept(fg.expressionVisitor, builder);
            
            //auto elementPtr = builder.CreateGEP(targetVar->target->type->getLlvmType(fg.getContext()), target, llvm::ConstantInt::get(builder.getContext(), llvm::APInt(32, 0)));
            auto elementPtr = llvm::ConstantPointerNull::get(val->getType()->getPointerTo());
            
            builder.CreateStore(val, elementPtr);
        }
        else {
            throw "field access without target";
        }
    }
    else {
#ifdef DEBUGGING
        printer << "assigning to instance of " << assignment.target->toString() << std::endl;
#endif
        throw "only local variables are assignable at the moment";
    }
    
    return nullptr;
    //return llvm::ConstantFP::get(fg.getContext(), llvm::APFloat(5123.0));*/
}


llvm::Value* StatementVisitor::visit(sem::ReturnStatement& returnStatement,
        qlow::gen::FunctionGenerator& fg)
{
    fg.builder.SetInsertPoint(fg.getCurrentBlock());
    auto val = returnStatement.value->accept(fg.expressionVisitor, fg.builder);
    if (returnStatement.value != nullptr && val == nullptr) {
        throw "internal error: returned type is invalid";
    }
    fg.builder.CreateRet(val);
    return val;
}


llvm::Value* StatementVisitor::visit(sem::FeatureCallStatement& fc, gen::FunctionGenerator& fg)
{
    llvm::Module* module = fg.getModule();
    fg.builder.SetInsertPoint(fg.getCurrentBlock());
    //llvm::Constant* c = module->getOrInsertFunction(fc.expr->callee->name, {});
    
    return fc.expr->accept(fg.expressionVisitor, fg.builder);
    
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


llvm::Value* CastGenerator::generateCast(llvm::Value* toCast,
                                          llvm::IRBuilder<>& b)
{
    /*return b.CreateCast(
        llvm::Instruction::CastOps::BitCast, toCast,
        cast.to->getLlvmType(b.getContext()));*/
    return nullptr;
}




