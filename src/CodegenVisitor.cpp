#include "CodegenVisitor.h"

#include <llvm/IR/Constants.h>


using namespace qlow;

llvm::Value* CodegenVisitor::visit(sem::IntConst& node, llvm::LLVMContext& context)
{
    return llvm::ConstantInt::get(context, llvm::APInt(32, std::to_string(node.value), 10));
}



