#include "Builtin.h"

using namespace qlow;

sem::Class initInt32(void)
{
    sem::Class c{ nullptr, sem::nativeGlobalScope };
    return c;
}


sem::GlobalScope sem::nativeGlobalScope{};
sem::Class sem::Int32 = initInt32();



