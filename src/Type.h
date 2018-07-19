#ifndef QLOW_SEM_TYPE_H
#define QLOW_SEM_TYPE_H

#include <memory>
#include "Scope.h"

namespace llvm {
    class Value;
    class Type;
    class LLVMContext;
}

namespace qlow
{
    namespace sem
    {
        // forward declarations
        struct Class;
        
        class Scope;
    }


    namespace sem
    {
        struct SemanticObject;
        
        class Type;
        
        class ClassType;
        class ArrayType;
        class NativeType;
    }
}


struct qlow::sem::SemanticObject
{
    virtual ~SemanticObject(void);
    
    /**
     * \brief converts the object to a readable string for debugging purposes. 
     */
    virtual std::string toString(void) const;
};



class qlow::sem::Type : public SemanticObject,
                        protected std::enable_shared_from_this<Type>
{
public:
    virtual ~Type(void);

    virtual bool isClassType(void) const = 0;
    virtual bool isNativeType(void) const = 0;
    virtual bool isArrayType(void) const = 0;

    virtual Scope& getScope(void) = 0;
    
    virtual llvm::Type* getLlvmType(llvm::LLVMContext& context) const = 0;
    
    virtual bool equals(const Type& other) const;
    
//    static std::shared_ptr<Type> VOID;
//    static std::shared_ptr<Type> INTEGER;
//    static std::shared_ptr<Type> BOOLEAN;
};


class qlow::sem::ClassType : public Type
{
    sem::Class* classType;
    sem::TypeScope scope;
public:
    inline ClassType(sem::Class* classType) :
        classType{ classType },
        scope{ shared_from_this() }
    {
    }
    
    inline bool isClassType(void) const override { return true; }
    inline bool isNativeType(void) const override { return false; }
    inline bool isArrayType(void) const override { return false; }
    
    Scope& getScope(void);
    
    virtual llvm::Type* getLlvmType(llvm::LLVMContext& context) const override;
    inline sem::Class* getClassType(void) { return classType; }
    virtual bool equals(const Type& other) const;
};


class qlow::sem::ArrayType : public Type
{
    std::shared_ptr<sem::Type> arrayType;
    TypeScope scope;
public:
    
    inline ArrayType(std::shared_ptr<sem::Type> arrayType) :
        arrayType{ std::move(arrayType) },
        scope{ shared_from_this() }
    {
    }
    
    inline bool isClassType(void) const override { return false; }
    inline bool isNativeType(void) const override { return false; }
    inline bool isArrayType(void) const override { return true; }
    
    Scope& getScope(void);
    
    virtual llvm::Type* getLlvmType(llvm::LLVMContext& context) const override;
    inline std::shared_ptr<sem::Type> getArrayType(void) { return arrayType; }
    virtual bool equals(const Type& other) const;
};


class qlow::sem::NativeType : public Type
{
    NativeTypeScope scope;
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
        type{ type },
        scope{ shared_from_this() }
    {
    }
    
    inline bool isClassType(void) const override { return false; }
    inline bool isNativeType(void) const override { return true; }
    inline bool isArrayType(void) const override { return false; }
    
    Scope& getScope(void);
    
    bool isIntegerType(void) const;
    
    llvm::Type* getLlvmType(llvm::LLVMContext& context) const override;
    virtual bool equals(const sem::Type& other) const;
    
    /// cast an llvm::Value from another native type to this one
    llvm::Value* generateImplicitCast(llvm::Value* value);
};


#endif // QLOW_SEM_TYPE_H
