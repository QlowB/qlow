#ifndef QLOW_SEM_CONTEXT_H
#define QLOW_SEM_CONTEXT_H

#include <unordered_map>
#include <memory>
#include <vector>
#include <optional>

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
}


class qlow::sem::Context
{
private:
    std::vector<Type> types;
    std::unordered_map<std::reference_wrapper<Type>, TypeId, std::hash<std::reference_wrapper<Type>>, std::equal_to<Type>> typesMap;
    
    std::unique_ptr<NativeScope> nativeScope;

public:
    Context(void);
    
    TypeId addType(Type&& type);
    std::optional<std::reference_wrapper<Type>> getType(TypeId tid);

    TypeId getPointerTo(TypeId id);
    TypeId getArrayOf(TypeId id);


    TypeId getVoidTypeId(void);
    TypeId getNativeTypeId(Type::Native n);

    std::unique_ptr<TypeScope> getTypeScope(TypeId);
    NativeScope& getNativeScope(void);
};

#endif // QLOW_SEM_CONTEXT_H


