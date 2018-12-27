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
        
        struct NativeMethod;
    }


    namespace sem
    {
        struct SemanticObject;
        class Semantic;
        
        class Type;
        
        class PointerType;
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


class qlow::sem::Type : public SemanticObject
{
protected:
    const Semantic& semantic;
public:

    inline Type(const Semantic& semantic) :
        semantic{ semantic }
    {
    }

    virtual ~Type(void);

    /// \returns false by default
    virtual inline bool isPointerType(void) const { return false; }
    
    /// \returns false by default
    virtual inline bool isClassType(void) const { return false; }
    
    /// \returns false by default
    virtual inline bool isNativeType(void) const { return false; }
    
    /// \returns false by default
    virtual inline bool isArrayType(void) const { return false; }

    virtual std::string asString(void) const = 0;
    virtual Scope& getScope(void) = 0;
    
    virtual llvm::Type* getLlvmType(llvm::LLVMContext& context) const = 0;
    
    virtual bool equals(const Type& other) const;
    
//    static std::shared_ptr<Type> VOID;
//    static std::shared_ptr<Type> INTEGER;
//    static std::shared_ptr<Type> BOOLEAN;
};


class qlow::sem::PointerType : public Type
{
    //std::shared_ptr<Type> derefType;
    TypeId derefType;
    sem::TypeScope scope;
public:
    inline PointerType(const Semantic& semantic) :
        Type{ semantic },
        scope{ semantic, *this }
    {
    }

    TypeId getDerefType(void) const { return derefType; }

    inline bool isPointerType(void) const override { return true; }

    virtual std::string asString(void) const override;
    virtual Scope& getScope(void) override;

    virtual llvm::Type* getLlvmType(llvm::LLVMContext& context) const override;

    virtual bool equals(const Type& other) const override;
};


class qlow::sem::ClassType : public Type
{
    sem::Class* classType;
    sem::TypeScope scope;
public:
    inline ClassType(const Semantic& semantic, sem::Class* classType) :
        Type{ semantic },
        classType{ classType },
        scope{ semantic, *this }
    {
    }

    inline bool isClassType(void) const override { return true; }

    std::string asString(void) const;
    Scope& getScope(void);

    virtual llvm::Type* getLlvmType(llvm::LLVMContext& context) const override;
    inline sem::Class* getClassType(void) { return classType; }
    virtual bool equals(const Type& other) const override;
};


class qlow::sem::ArrayType : public Type
{
    TypeId arrayType;
    TypeScope scope;
public:
    
    inline ArrayType(const Semantic& semantic, TypeId arrayType) :
        Type{ semantic },
        arrayType{ arrayType },
        scope{semantic, *this }
    {
    }
    
    inline bool isArrayType(void) const override { return true; }
    
    std::string asString(void) const;
    Scope& getScope(void);
    
    virtual llvm::Type* getLlvmType(llvm::LLVMContext& context) const override;
    inline TypeId getArrayType(void) { return arrayType; }
    virtual bool equals(const Type& other) const override;
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
    
    SymbolTable<NativeMethod> nativeMethods;
    
    inline NativeType(const Semantic& semantic, Type type) :
        sem::Type{ semantic },
        type{ type },
        scope{ semantic, *this }
    {
    }
    
    inline bool isNativeType(void) const override { return true; }
    
    std::string asString(void) const;
    Scope& getScope(void);
    
    bool isIntegerType(void) const;
    
    llvm::Type* getLlvmType(llvm::LLVMContext& context) const override;
    virtual bool equals(const sem::Type& other) const override;
    
    /// cast an llvm::Value from another native type to this one
    llvm::Value* generateImplicitCast(llvm::Value* value);
};


#endif // QLOW_SEM_TYPE_H

