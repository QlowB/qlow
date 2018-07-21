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
        { "Boolean",    NativeType::BOOLEAN },
        { "Char",       NativeType::CHAR },
        { "String",     NativeType::STRING },
        
        { "Float32",    NativeType::FLOAT32 },
        { "Float64",    NativeType::FLOAT64 },
    };
    
    std::map<std::string, NativeType::Type> integers = {
        { "Integer",    NativeType::INTEGER },
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
    };
    
    for (auto [name, type] : natives) {
        scope.types.insert({ name, std::make_unique
            <std::shared_ptr<NativeType>>(std::make_shared<NativeType>(type)) });
    }
    
    for (auto [name, type] : integers) {
        auto native = std::make_shared<NativeType>(type);
        
        native->nativeMethods.insert(
            { "+",
                std::make_unique<BinaryNativeMethod>(native, native,
                [] (llvm::IRBuilder<>& builder, llvm::Value* a, llvm::Value* b) {
                    return builder.CreateAdd(a, b);
                })
            }
        );
        native->nativeMethods.insert(
            { "-",
                std::make_unique<BinaryNativeMethod>(native, native,
                [] (llvm::IRBuilder<>& builder, llvm::Value* a, llvm::Value* b) {
                    return builder.CreateSub(a, b);
                })
            }
        );
        native->nativeMethods.insert(
            { "*",
                std::make_unique<BinaryNativeMethod>(native, native,
                [] (llvm::IRBuilder<>& builder, llvm::Value* a, llvm::Value* b) {
                    return builder.CreateMul(a, b);
                })
            }
        );
        if (name[0] == 'U') {
            native->nativeMethods.insert(
                { "/",
                    std::make_unique<BinaryNativeMethod>(native, native,
                    [] (llvm::IRBuilder<>& builder, llvm::Value* a, llvm::Value* b) {
                        return builder.CreateUDiv(a, b);
                    })
                }
            );
        }
        else {
            native->nativeMethods.insert(
                { "/",
                    std::make_unique<BinaryNativeMethod>(native, native,
                    [] (llvm::IRBuilder<>& builder, llvm::Value* a, llvm::Value* b) {
                        return builder.CreateSDiv(a, b);
                    })
                }
            );
        }
        
        native->nativeMethods.insert(
            { "==",
                std::make_unique<BinaryNativeMethod>(std::make_shared<NativeType>(NativeType::BOOLEAN), native,
                [] (llvm::IRBuilder<>& builder, llvm::Value* a, llvm::Value* b) {
                    return builder.CreateICmpEQ(a, b);
                })
            }
        );
        native->nativeMethods.insert(
            { "!=",
                std::make_unique<BinaryNativeMethod>(std::make_shared<NativeType>(NativeType::BOOLEAN), native,
                [] (llvm::IRBuilder<>& builder, llvm::Value* a, llvm::Value* b) {
                    return builder.CreateICmpNE(a, b);
                })
            }
        );
        
        scope.types.insert({ name, std::make_unique
            <std::shared_ptr<NativeType>>(std::move(native)) });
    }
    
    return scope;
}


llvm::Value* qlow::sem::UnaryNativeMethod::generateCode(llvm::IRBuilder<>& builder,
    std::vector<llvm::Value*> arguments)
{
    if (arguments.size() != 1)
        throw "invalid unary operation";
    return generator(builder, arguments[0]);
}


llvm::Value* qlow::sem::BinaryNativeMethod::generateCode(llvm::IRBuilder<>& builder,
    std::vector<llvm::Value*> arguments)
{
    if (arguments.size() != 2)
        throw "invalid binary operation";
    return generator(builder, arguments[0], arguments[1]);
}







