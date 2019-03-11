#ifndef QLOW_SEM_CONTEXT_H
#define QLOW_SEM_CONTEXT_H

#include <unordered_map>
#include <memory>
#include <vector>
#include <optional>

namespace qlow::sem
{
    class Type;
    class Context;
    
    using TypeId = size_t;
}

namespace std
{
    template<>
    struct std::hash<std::reference_wrapper<qlow::sem::Type>>
    {
        size_t operator() (const std::reference_wrapper<qlow::sem::Type>& t) const;
    };
}


class qlow::sem::Context
{
private:
    std::vector<std::unique_ptr<Type>> types;
    std::unordered_map<std::reference_wrapper<Type>, TypeId> typesMap;
    
public:
    
    TypeId addType(Type&& type);
    std::optional<std::reference_wrapper<Type>> getType(TypeId tid);
};

#endif // QLOW_SEM_CONTEXT_H


