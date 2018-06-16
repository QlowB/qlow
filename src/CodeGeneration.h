#ifndef QLOW_CODE_GENERATION_H
#define QLOW_CODE_GENERATION_H

#include "Semantic.h"
#include "CodegenVisitor.h"

#include <stack>

#include <llvm/IR/Module.h>

namespace qlow
{
namespace gen
{
    std::unique_ptr<llvm::Module> generateModule(const sem::SymbolTable<sem::Class>& classes);
    void generateObjectFile(const std::string& name, std::unique_ptr<llvm::Module> module);

    class FunctionGenerator;
}
}

class qlow::gen::FunctionGenerator
{
    const sem::Method& method;
    llvm::Module* module;

    std::stack<llvm::BasicBlock*> basicBlocks;

public:

    StatementVisitor statementVisitor;
    ExpressionVisitor expressionVisitor;

    inline FunctionGenerator(const sem::Method& m, llvm::Module* module) :
        method{ m }, module{ module }
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
