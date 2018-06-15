#ifndef QLOW_CODE_GENERATION_H
#define QLOW_CODE_GENERATION_H

#include "Semantic.h"
#include <llvm/IR/Module.h>

namespace qlow
{
    namespace gen
    {
        void generateObjectFile(const std::string& name, std::unique_ptr<llvm::Module> module,
            const sem::SymbolTable<sem::Class>& classes);
    }
}


#endif // QLOW_CODE_GENERATION_H
