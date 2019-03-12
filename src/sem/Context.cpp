#include "Context.h"
#include "Type.h"

using qlow::sem::Context;

size_t std::hash<std::reference_wrapper<qlow::sem::Type>>::operator() (const std::reference_wrapper<qlow::sem::Type>& t) const
{
    return t.get().hash();
}


qlow::sem::TypeId Context::addType(Type&& type) {
    if (typesMap.find(type) != typesMap.end()) {
        return typesMap[type];
    }
    else {
        types.push_back(std::move(type));
        return types.size() - 1;
    }
}


std::optional<std::reference_wrapper<qlow::sem::Type>> Context::getType(TypeId tid)
{
    if (tid >= 0 && tid <= types.size()) {
        return std::make_optional<std::reference_wrapper<qlow::sem::Type>>(types[tid]);
    }
    else {
        return std::nullopt;
    }
}


qlow::sem::TypeId Context::getPointerTo(TypeId id)
{
    return addType(Type::createPointerType(*this, id));
}


qlow::sem::TypeId Context::getArrayOf(TypeId id)
{
    return addType(Type::createArrayType(*this, id));
}

