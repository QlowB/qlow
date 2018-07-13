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
    
    std::map<std::string, NativeType::Type> natives = {
        { "Integer",    NativeType::INTEGER },
        { "Boolean",    NativeType::BOOLEAN },
        { "Char",       NativeType::CHAR },
        { "String",     NativeType::STRING },
        
        { "Int8",       NativeType::INT8 },
        { "Int16",      NativeType::INT16 },
        { "Int32",      NativeType::INT32 },
        { "Int64",      NativeType::INT64 },
        { "Int128",     NativeType::INT128 },
        
        { "UInt8",      NativeType::UINT8 },
        { "UInt16",     NativeType::UINT16 },
        { "UInt32",     NativeType::UINT32 },
        { "UInt64",     NativeType::UINT64 },
        { "UInt128",    NativeType::UINT128 },
        
        { "Float32",    NativeType::FLOAT32 },
        { "Float64",    NativeType::FLOAT64 },
    };
    
    for (auto [name, type] : natives) {
        scope.types.insert({ name, std::make_unique<NativeType>(type) });
    }
    
    return scope;
}





