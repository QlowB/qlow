#ifndef QLOW_SEM_CONTEXT_H
#define QLOW_SEM_CONTEXT_H

#include "Type.h"
#include "unordered_map"

namespace qlow::sem
{
    class Context;
    
    using TypeId = size_t;
}


class qlow::sem::Context
{
private:
    std::vector<std::unique_ptr<Type>> types;
    std::unordered_map<Type&, TypeId> typesMap;
    
public:
    
    TypeId addType(Type&& type) {
        if (typesMap.contains(type)) {
            return typesMap[type];
        }
        else {
            types.push_back(std::unique_ptr<Type>(type));
            return types.size() - 1;
        }
    }
};

#endif // QLOW_SEM_CONTEXT_H


