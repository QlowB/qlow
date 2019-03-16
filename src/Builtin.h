#ifndef QLOW_SEM_BUILTIN_H
#define QLOW_SEM_BUILTIN_H


#include "Semantic.h"
#include "Scope.h"
#include "Type.h"

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

namespace qlow
{
    namespace sem
    {
        class Context;
        
        NativeScope generateNativeScope(Context& context);
        void fillNativeScope(NativeScope& scope);

        NativeTypeScope generateNativeTypeScope(Context& context, Type::Native native);
        
        struct NativeMethod;
        struct UnaryNativeMethod;
        struct BinaryNativeMethod;
    }
}


struct qlow::sem::NativeMethod : public sem::Method
{
    inline NativeMethod(NativeScope& scope, TypeId returnType) :
        Method{ scope, returnType }
    {
    }
    
    virtual llvm::Value* generateCode(llvm::IRBuilder<>& builder,
                                      std::vector<llvm::Value*> arguments) = 0;
};


struct qlow::sem::UnaryNativeMethod : public sem::NativeMethod
{
    std::function<llvm::Value*(llvm::IRBuilder<>&, llvm::Value*)> generator;
    
    inline UnaryNativeMethod(NativeScope& scope,
                             TypeId returnType,
                             const std::function
                             <llvm::Value*(llvm::IRBuilder<>&, llvm::Value*)>& generator) :
        NativeMethod{ scope, returnType },
        generator{ generator }
    {
    }
    
    virtual llvm::Value* generateCode(llvm::IRBuilder<>& builder,
                                      std::vector<llvm::Value*> arguments);
};


struct qlow::sem::BinaryNativeMethod : public sem::NativeMethod
{
    using Func = 
        std::function<llvm::Value*(llvm::IRBuilder<>&, llvm::Value*, llvm::Value*)>;
    
    Func generator;
    Variable argument;
    
    inline BinaryNativeMethod(NativeScope& scope,
                              TypeId returnType,
                              TypeId argumentType,
                              Func&& generator) :
        NativeMethod{ scope, returnType },
        generator{ generator },
        argument{ context, argumentType, "arg" }
    {
        Method::arguments = { &argument };
    }
    
    virtual llvm::Value* generateCode(llvm::IRBuilder<>& builder,
                                      std::vector<llvm::Value*> arguments);
};

#endif // QLOW_SEM_BUILTIN_H


