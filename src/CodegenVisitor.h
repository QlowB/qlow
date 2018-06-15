#ifndef QLOW_CODEGEN_VISITOR_H
#define QLOW_CODEGEN_VISITOR_H

#include "llvm/IR/Value.h"
#include "llvm/IR/LLVMContext.h"

#include "Visitor.h"
#include "Semantic.h"
#include "Scope.h"


#include <memory>


namespace qlow
{
    namespace gen
    {
    }
}


namespace qlow
{
    class CodegenVisitor;
}


class qlow::CodegenVisitor :
    public Visitor<
        llvm::Value*,
        llvm::LLVMContext,

        sem::IntConst
    >
{
public:
    llvm::Value* visit(sem::IntConst& node, llvm::LLVMContext&) override;
};


#endif // QLOW_CODEGEN_VISITOR_H



