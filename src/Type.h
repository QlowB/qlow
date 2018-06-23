#ifndef QLOW_SEM_TYPE_H
#define QLOW_SEM_TYPE_H

#include <memory>

namespace llvm {
    class Type;
    class LLVMContext;
}

namespace qlow
{
    namespace sem
    {
        // forward declarations
        struct Class;
    }


    namespace sem
    {
        class Type;
        
        class ClassType;
        class ArrayType;
        class NativeType;
    }
}


class qlow::sem::Type
{
public:
    virtual ~Type(void);

    virtual bool isClassType(void) const = 0;
    virtual bool isNativeType(void) const = 0;
    virtual bool isArrayType(void) const = 0;

    Class* getClassType(void);
    virtual llvm::Type* getLlvmType(llvm::LLVMContext& context) const = 0;
    
    virtual bool operator == (const Type& other) const;
    virtual bool operator != (const Type& other) const;
    
    static Type* INTEGER;
    static Type* BOOLEAN;
};


class qlow::sem::ClassType : public Type
{
    sem::Class* classType;
public:
    inline bool isClassType(void) const override { return true; }
    inline bool isNativeType(void) const override { return false; }
    inline bool isArrayType(void) const override { return false; }
    
    virtual llvm::Type* getLlvmType(llvm::LLVMContext& context) const override;
    inline sem::Class* getClassType(void) { return classType; }
};


class qlow::sem::ArrayType : public Type
{
    sem::Type* arrayType;
public:
    inline bool isClassType(void) const override { return false; }
    inline bool isNativeType(void) const override { return false; }
    inline bool isArrayType(void) const override { return true; }
    
    inline sem::Type* getArrayType(void) { return arrayType; }
};


class qlow::sem::NativeType : public Type
{
public:
    enum Type {
        INTEGER,
        BOOLEAN
    };
    
    Type type;
    
    inline NativeType(Type type) :
        type{ type }
    {
    }
    
    inline bool isClassType(void) const override { return false; }
    inline bool isNativeType(void) const override { return true; }
    inline bool isArrayType(void) const override { return false; }
    
    virtual llvm::Type* getLlvmType(llvm::LLVMContext& context) const;
};


#endif // QLOW_SEM_TYPE_H
