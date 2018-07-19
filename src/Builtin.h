#ifndef QLOW_SEM_BUILTIN_H
#define QLOW_SEM_BUILTIN_H


#include "Semantic.h"
#include "Scope.h"


namespace qlow
{
    namespace sem
    {
        NativeScope generateNativeScope(void);
        
        struct NativeMethod;
        class IntegerNativeScope;
    }
}


struct qlow::sem::NativeMethod : public sem::Method
{
    llvm::Value* generateCode(std::vector<llvm::Value*> arguments);
}


class qlow::sem::IntegerNativeScope : public NativeScope
{
public:
    void generateAdd();
};











#endif // QLOW_SEM_BUILTIN_H


