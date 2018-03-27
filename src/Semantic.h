#ifndef QLOW_SEMANTIC_H
#define QLOW_SEMANTIC_H

#include <string>
#include <map>
#include "Util.h"

namespace qlow
{
    namespace sem
    {

        template<typename T>
        using SymbolTable = std::map<std::string, std::unique_ptr<T>>;

        struct SemanticObject;
        struct Class;

        struct Field;
        struct Method;
    }
}


struct Class
{
    std::string name;
    SymbolTable<Field> fields;
    SymbolTable<Method> methods;
};


struct Field
{
    Class* type;
    std::string name;
}








#endif // QLOW_SEMANTIC_H

