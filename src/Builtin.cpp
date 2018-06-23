#include "Builtin.h"
#include "Scope.h"

using namespace qlow;

sem::Class sem::int32 = sem::Class{ "Integer", sem::NativeScope::getInstance() };
sem::Class sem::boolean = sem::Class{ "Boolean", sem::NativeScope::getInstance() };

sem::NativeScope qlow::sem::generateNativeScope(void)
{
    using sem::Class;
    
    sem::NativeScope scope;
    
    /*
    std::unique_ptr<sem::Type> int32 =
        std::make_unique<sem::Type>(sem::Type::Kind::INTEGER, &sem::int32);
    std::unique_ptr<sem::Type> boolean =
        std::make_unique<sem::Type>(sem::Type::Kind::BOOLEAN, &sem::int32);
        
    scope.types.insert({"Integer", std::move(int32)});
    scope.types.insert({"Boolean", std::move(boolean)});
    return scope;
    */
}





