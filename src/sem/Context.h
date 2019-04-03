#ifndef QLOW_SEM_CONTEXT_H
#define QLOW_SEM_CONTEXT_H

#include <unordered_map>
#include <memory>
#include <vector>
#include <optional>

#include <llvm/IR/Type.h>
#include <llvm/IR/LLVMContext.h>

#include "Type.h"
#include "Util.h"

namespace qlow::sem
{
    class Type;
    class Context;

    class TypeScope;
    class NativeScope;
}

namespace std
{
    template<>
    struct hash<qlow::sem::Type*>
    {
        size_t operator() (qlow::sem::Type* t) const;
    };


    template<>
    struct equal_to<qlow::sem::Type*>
    {
        inline bool operator() (qlow::sem::Type* a, qlow::sem::Type* b) const
        {
            return a->equals(*b);
        }
    };
}


/**
 * @brief owning context for all types
 * 
 * This class ensures that there is only ever one
 */
class qlow::sem::Context
{
private:
    OwningList<Type> types;
    std::unordered_map<Type*, size_t> typeMap;

    std::unique_ptr<NativeScope> nativeScope;
public:
    Context(void);

    Type* getVoidType(void);
    NativeScope& getNativeScope(void);

    Type* getNativeType(NativeType::NType type);
    Type* getClassType(Class* c);
    Type* getArrayType(Type* pointsTo);

    //llvm::Type* getLlvmType(TypeId id, llvm::LLVMContext& llvmCtxt);

    void createLlvmTypes(llvm::LLVMContext& llvmCtxt);
};

#endif // QLOW_SEM_CONTEXT_H


