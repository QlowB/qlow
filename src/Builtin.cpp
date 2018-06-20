#include "Builtin.h"
#include "Scope.h"

using namespace qlow;

sem::Class sem::int32 = sem::Class{ "Integer", sem::NativeScope::getInstance() };

sem::NativeScope qlow::sem::generateNativeScope(void)
{
    using sem::Class;
    
    sem::NativeScope scope;
    
    std::unique_ptr<sem::Type> int32 =
        std::make_unique<sem::Type>(sem::Type::Kind::INTEGER, &sem::int32);
    
    scope.types.insert({"Integer", std::move(int32)});
    return scope;
}





