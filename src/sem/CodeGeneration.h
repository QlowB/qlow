#ifndef QLOW_CODE_GENERATION_H
#define QLOW_CODE_GENERATION_H

#include "Semantic.h"
#include "Builtin.h"
#include "CodegenVisitor.h"

#include <stack>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

namespace qlow
{
namespace gen
{
    std::unique_ptr<llvm::Module> generateModule(sem::GlobalScope& objects);
    llvm::Function* generateFunction (llvm::Module* module, sem::Method* method);
    void generateObjectFile(const std::string& name, std::unique_ptr<llvm::Module> module, int optLevel);

    class FunctionGenerator;
}
}

class qlow::gen::FunctionGenerator
{
    const sem::Method& method;
    llvm::Module* module;
    llvm::AttributeSet& attributes;

    std::stack<llvm::BasicBlock*> basicBlocks;

public:

    StatementVisitor statementVisitor;
    ExpressionCodegenVisitor expressionVisitor;
    LValueVisitor lvalueVisitor;
    llvm::IRBuilder<> builder;

    inline FunctionGenerator(const sem::Method& m, llvm::Module* module,
        llvm::AttributeSet& attributes) :
        method{ m },
        module{ module },
        attributes{ attributes },
        expressionVisitor{ *this },
        builder{ module->getContext() }
    {
    }

    llvm::Function* generate(void);

    inline llvm::Module* getModule(void) const { return module; }
    inline llvm::LLVMContext& getContext(void) const { return module->getContext(); }
    inline llvm::BasicBlock* getCurrentBlock(void) const { return basicBlocks.top(); }
    inline void pushBlock(llvm::BasicBlock* bb) { basicBlocks.push(bb); }
    inline llvm::BasicBlock* popBlock(void) { auto* bb = basicBlocks.top(); basicBlocks.pop(); return bb; }
};


#endif // QLOW_CODE_GENERATION_H
