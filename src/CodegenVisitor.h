#ifndef QLOW_CODEGEN_VISITOR_H
#define QLOW_CODEGEN_VISITOR_H

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

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
        llvm::IRBuilder<>,

        sem::BinaryOperation,
        sem::UnaryOperation,
        sem::FeatureCallExpression,
        sem::IntConst
    >
{
public:
    llvm::Value* visit(sem::BinaryOperation& node, llvm::IRBuilder<>&) override;
    llvm::Value* visit(sem::UnaryOperation& node, llvm::IRBuilder<>&) override;
    llvm::Value* visit(sem::FeatureCallExpression& node, llvm::IRBuilder<>&) override;
    llvm::Value* visit(sem::IntConst& node, llvm::IRBuilder<>&) override;
};


#endif // QLOW_CODEGEN_VISITOR_H



