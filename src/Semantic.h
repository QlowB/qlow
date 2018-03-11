#ifndef QLOW_SEMANTIC_H
#define QLOW_SEMANTIC_H

#include <string>
#include "Util.h"

namespace qlow
{
    namespace sem
    {

        struct Class;

        struct Field;
        struct Method;
    }
}


struct Class
{
    std::string name;
    util::OwningList<Field> fields;
    util::OwningList<Method> methods;
};


struct Field
{
    Class* type;
}








#endif // QLOW_SEMANTIC_H


