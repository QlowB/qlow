#ifndef QLOW_SEM_BUILTIN_H
#define QLOW_SEM_BUILTIN_H


#include "Semantic.h"
#include "Scope.h"

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

namespace qlow
{
    namespace sem
    {
        NativeScope generateNativeScope(void);
        
        struct NativeMethod;
        struct UnaryNativeMethod;
        struct BinaryNativeMethod;
    }
}


struct qlow::sem::NativeMethod : public sem::Method
{
    inline NativeMethod(TypeId returnType) :
        Method{ NativeScope::getInstance(), returnType }
    {
    }
    
    virtual llvm::Value* generateCode(llvm::IRBuilder<>& builder,
                                      std::vector<llvm::Value*> arguments) = 0;
};


struct qlow::sem::UnaryNativeMethod : public sem::NativeMethod
{
    std::function<llvm::Value*(llvm::IRBuilder<>&, llvm::Value*)> generator;
    
    inline UnaryNativeMethod(TypeId returnType,
        const std::function
        <llvm::Value*(llvm::IRBuilder<>&, llvm::Value*)>& generator) :
        NativeMethod{ returnType },
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
    
    inline BinaryNativeMethod(TypeId returnType,
                              TypeId argumentType,
        
        Func&& generator) :
        NativeMethod{ returnType },
        generator{ generator },
        argument{ argumentType, "arg" }
    {
        Method::arguments = { &argument };
    }
    
    virtual llvm::Value* generateCode(llvm::IRBuilder<>& builder,
                                      std::vector<llvm::Value*> arguments);
};

#endif // QLOW_SEM_BUILTIN_H


