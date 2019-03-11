#ifndef QLOW_SEM_TYPE_H
#define QLOW_SEM_TYPE_H

#include <variant>
#include <string>

namespace qlow::sem
{
    struct SemanticObject;
    class Type;
    
    struct TypeId;

    // forward declarations to other files
    struct Class;
    struct Context;
}


struct qlow::sem::SemanticObject
{
    virtual ~SemanticObject(void);
    
    /**
     * \brief converts the object to a readable string for debugging purposes. 
     */
    virtual std::string toString(void) const;
};


class qlow::sem::TypeId
{
    Context& context;
    size_t id;
public:
    inline TypeId(Context& context, size_t id) :
        context{ context }, id{ id } {}

    inline Context& getContext(void) const { return context; }
    inline size_t getId(void) const { return id; }
};


class qlow::sem::Type
{
public:
    enum class Kind
    {
        NATIVE,
        CLASS,
        POINTER,
        ARRAY
    };
private:
    std::string name;

    struct NativeType
    {
    };

    struct ClassType
    {
        Class* classType;
    };

    struct PointerType
    {
        TypeId targetType;
    };

    struct ArrayType 
    {
        TypeId targetType;
    };

    std::variant<NativeType, ClassType, PointerType, ArrayType> type;
public:
    Kind getKind(void) const;
    std::string asString(void) const;
    size_t hash(void) const;

    bool operator == (const Type& other) const;
};

 
#if 0
#include "Scope.h"

#include <memory>
#include <string>

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
        
        class Type;
        
        class PointerType;
        class ClassType;
        class ArrayType;
        class NativeType;
    }
}

class qlow::sem::Type : public SemanticObject
{
public:
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

    virtual size_t hash(void) const;
    
//    static std::shared_ptr<Type> VOID;
//    static std::shared_ptr<Type> INTEGER;
//    static std::shared_ptr<Type> BOOLEAN;
};


class qlow::sem::PointerType : public Type
{
    std::shared_ptr<Type> derefType;
    sem::TypeScope scope;
public:
    inline PointerType(std::shared_ptr<Type> derefType) :
        derefType{ derefType },
        scope{ *this }
    {
    }
    
    const std::shared_ptr<Type>& getDerefType(void) const { return derefType; }
    
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
    inline ClassType(sem::Class* classType) :
        classType{ classType },
        scope{ *this }
    {
    }
    
    inline bool isClassType(void) const override { return true; }
    
    std::string asString(void) const override;
    Scope& getScope(void) override;
    
    virtual llvm::Type* getLlvmType(llvm::LLVMContext& context) const override;
    inline sem::Class* getClassType(void) { return classType; }
    virtual bool equals(const Type& other) const override;
};


class qlow::sem::ArrayType : public Type
{
    std::shared_ptr<sem::Type> arrayType;
    TypeScope scope;
public:
    
    inline ArrayType(std::shared_ptr<sem::Type> arrayType) :
        arrayType{ std::move(arrayType) },
        scope{ *this }
    {
    }
    
    inline bool isArrayType(void) const override { return true; }
    
    std::string asString(void) const override;
    Scope& getScope(void) override;
    
    virtual llvm::Type* getLlvmType(llvm::LLVMContext& context) const override;
    inline std::shared_ptr<sem::Type> getArrayType(void) { return arrayType; }
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
    
    inline NativeType(Type type) :
        scope{ *this },
        type{ type }
    {
    }
    
    inline bool isNativeType(void) const override { return true; }
    
    std::string asString(void) const override;
    Scope& getScope(void) override;
    
    bool isIntegerType(void) const;
    
    llvm::Type* getLlvmType(llvm::LLVMContext& context) const override;
    virtual bool equals(const sem::Type& other) const override;
    
    /// cast an llvm::Value from another native type to this one
    llvm::Value* generateImplicitCast(llvm::Value* value);
};

#endif

#endif // QLOW_SEM_TYPE_H
