#include "Type.h"


using namespace qlow;

sem::Type::~Type(void)
{
}


bool sem::ClassType::isNative(void) const
{
    return false;
}


bool sem::NativeType::isNative(void) const
{
    return true;
}



