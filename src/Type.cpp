#include "Type.h"


using namespace qlow;

sem::Type::Type(sem::Class* classType) :
    kind{ Kind::CLASS }
{
    data.classType = classType;
}

bool sem::Type::isClassType(void) const
{
    return kind == Kind::CLASS;
}


bool sem::Type::isNative(void) const
{
    return kind != Kind::CLASS;
}


sem::Class* sem::Type::getClassType(void)
{
    if (kind != Kind::CLASS)
        throw "internal error";
    return data.classType;
}


const sem::Type sem::Type::INTEGER = sem::Type{ sem::Type::Kind::INTEGER };


