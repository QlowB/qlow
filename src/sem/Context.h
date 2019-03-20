#ifndef QLOW_SEM_CONTEXT_H
#define QLOW_SEM_CONTEXT_H

#include <unordered_map>
#include <memory>
#include <vector>
#include <optional>

#include <llvm/IR/Type.h>
#include <llvm/IR/LLVMContext.h>

#include "Type.h"

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
    struct hash<std::reference_wrapper<qlow::sem::Type>>
    {
        size_t operator() (const std::reference_wrapper<qlow::sem::Type>& t) const;
    };


    template<>
    struct equal_to<std::reference_wrapper<qlow::sem::Type>>
    {
        inline bool operator() (const std::reference_wrapper<qlow::sem::Type>& a, const std::reference_wrapper<qlow::sem::Type>& b) const
        {
            return a.get() == b.get();
        }
    };
}


class qlow::sem::Context
{
private:
    std::vector<std::pair<Type, llvm::Type*>> types;
    std::unordered_map<std::reference_wrapper<Type>, TypeId> typesMap;

    std::unique_ptr<NativeScope> nativeScope;
public:
    Context(void);
    
    TypeId addType(Type&& type);
    Type& getType(TypeId tid);
    std::optional<std::reference_wrapper<Type>> getMaybeType(TypeId tid);

    /**
     * @brief get a string denoting the type
     * 
     * @return the name of the type if the given id is valid,
     *         an empty string otherwise.
     */
    std::string getTypeString(TypeId tid);

    TypeId getVoidTypeId(void);
    TypeId getNativeTypeId(Type::Native n);
    NativeScope& getNativeScope(void);

    TypeId createNativeType(std::string name, Type::Native type);
    TypeId createClassType(Class* classType);
    TypeId createPointerType(TypeId pointsTo);
    TypeId createArrayType(TypeId pointsTo);

    llvm::Type* getLlvmType(TypeId id, llvm::LLVMContext& llvmCtxt);

    void createLlvmTypes(llvm::LLVMContext& llvmCtxt);
};

#endif // QLOW_SEM_CONTEXT_H


