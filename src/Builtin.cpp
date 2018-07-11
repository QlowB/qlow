#include "Builtin.h"
#include "Scope.h"
#include "Type.h"

using namespace qlow;


sem::NativeScope qlow::sem::generateNativeScope(void)
{
    using sem::Class;
    using sem::NativeType;
    using sem::NativeScope;
    
    NativeScope scope;
    
    scope.types.insert(
        {"Integer", std::make_unique<NativeType>(NativeType::INTEGER)}
    );
    
    scope.types.insert(
        {"Boolean", std::make_unique<NativeType>(NativeType::BOOLEAN)}
    );
    
    return scope;
    
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





