#ifndef QLOW_SEM_TYPE_H
#define QLOW_SEM_TYPE_H

#include <memory>
#include "Semantic.h"

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


class qlow::sem::Type : public SemanticObject
{
public:
    virtual ~Type(void);

    virtual bool isClassType(void) const = 0;
    virtual bool isNativeType(void) const = 0;
    virtual bool isArrayType(void) const = 0;

    virtual llvm::Type* getLlvmType(llvm::LLVMContext& context) const = 0;
    
    virtual bool equals(const Type* other) const;
    
    static Type* VOID;
    static Type* INTEGER;
    static Type* BOOLEAN;
};


class qlow::sem::ClassType : public Type
{
    sem::Class* classType;
public:
    inline ClassType(sem::Class* classType) :
        classType{ classType }
    {
    }
    
    inline bool isClassType(void) const override { return true; }
    inline bool isNativeType(void) const override { return false; }
    inline bool isArrayType(void) const override { return false; }
    
    virtual llvm::Type* getLlvmType(llvm::LLVMContext& context) const override;
    inline sem::Class* getClassType(void) { return classType; }
    virtual bool equals(const Type* other) const;
};


class qlow::sem::ArrayType : public Type
{
    sem::Type* arrayType;
public:
    
    inline ArrayType(sem::Type* arrayType) :
        arrayType{ arrayType }
    {
    }
    
    inline bool isClassType(void) const override { return false; }
    inline bool isNativeType(void) const override { return false; }
    inline bool isArrayType(void) const override { return true; }
    
    virtual llvm::Type* getLlvmType(llvm::LLVMContext& context) const override;
    inline sem::Type* getArrayType(void) { return arrayType; }
    virtual bool equals(const Type* other) const;
};


class qlow::sem::NativeType : public Type
{
public:
    enum Type {
        VOID,
        INTEGER,
        BOOLEAN,
        CHAR,
        STRING,
        INT8, INT16, INT32, INT64, INT128,
        UINT8, UINT16, UINT32, UINT64, UINT128,
        FLOAT32, FLOAT64, FLOAT128,
    };
    
    Type type;
    
    inline NativeType(Type type) :
        type{ type }
    {
    }
    
    inline bool isClassType(void) const override { return false; }
    inline bool isNativeType(void) const override { return true; }
    inline bool isArrayType(void) const override { return false; }
    
    llvm::Type* getLlvmType(llvm::LLVMContext& context) const override;
    virtual bool equals(const sem::Type* other) const;
};


#endif // QLOW_SEM_TYPE_H
