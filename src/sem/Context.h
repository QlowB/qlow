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
    std::string test = "ayayay";
    std::vector<Type> types;
    std::unordered_map<std::reference_wrapper<Type>, TypeId, std::hash<std::reference_wrapper<Type>>, std::equal_to<Type>> typesMap;
    
public:
    
    TypeId addType(Type&& type);
    std::optional<std::reference_wrapper<Type>> getType(TypeId tid);

    TypeId getPointerTo(TypeId id);
    TypeId getArrayOf(TypeId id);
};

#endif // QLOW_SEM_CONTEXT_H


