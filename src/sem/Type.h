#ifndef QLOW_SEM_TYPE_H
#define QLOW_SEM_TYPE_H

#include <variant>
#include <string>
#include <limits>

namespace llvm {
    class Value;
    class Type;
    class LLVMContext;
}

namespace qlow::sem
{
    struct SemanticObject;
    class Type;
    
    using TypeId = size_t;
    const TypeId NO_TYPE = std::numeric_limits<TypeId>::max();

    // forward declarations to other files
    struct Class;
    class Context;
}


struct qlow::sem::SemanticObject
{
    Context& context;
    inline SemanticObject(Context& context) :
        context{ context } {}
    virtual ~SemanticObject(void);
    
    /**
     * \brief converts the object to a readable string for debugging purposes. 
     */
    virtual std::string toString(void) const;
};


/*
class qlow::sem::TypeId
{
    Context& context;
    size_t id;
public:
    inline TypeId(Context& context, size_t id) :
        context{ context }, id{ id } {}

    inline TypeId(Context& context) :
        context{ context }, id{ std::numeric_limits<size_t>::max() } {}

    inline Context& getContext(void) const { return context; }
    inline size_t getId(void) const { return id; }

    TypeId toPointer(void) const;
    TypeId toArray(void) const;
};
*/


class qlow::sem::Type
{
    friend class Context;
public:
    enum class Kind
    {
        NATIVE,
        CLASS,
        POINTER,
        ARRAY
    };

    enum class Native
    {
        VOID,
        BOOLEAN,
        INTEGER,
    };

private:
    std::string name;

    struct NativeType
    {
        Native type;
        inline bool operator==(const NativeType& other) const { return type == other.type; }
    };

    struct ClassType
    {
        Class* classType;
        inline bool operator==(const ClassType& other) const { return classType == other.classType; }
    };

    struct PointerType
    {
        TypeId targetType;
        inline bool operator==(const PointerType& other) const { return targetType == other.targetType; }
    };

    struct ArrayType 
    {
        TypeId targetType;
        inline bool operator==(const ArrayType& other) const { return targetType == other.targetType; }
    };

    using Union = std::variant<NativeType, ClassType, PointerType, ArrayType>;
    Union type;

    inline Type(Union type) :
        type{ type } {}

    inline Type(Union type, std::string name) :
        name{ std::move(name) }, type{ type } {}

public:
    Kind getKind(void) const;
    std::string asString(void) const;
    size_t hash(void) const;

    bool operator == (const Type& other) const;

    Class* getClass(void) const;

    llvm::Type* getLLVMType(llvm::LLVMContext* context);

    static Type createNativeType(Context& c, std::string name, Native type);
    static Type createClassType(Context& c, Class* classType);
    static Type createPointerType(Context& c, TypeId pointsTo);
    static Type createArrayType(Context& c, TypeId pointsTo);
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
    
    virtual y = 0;
    
    virtual bool equals(const Type& other) const;

    virtual size_t hash(void) const;
    
//    static TypeId VOID;
//    static TypeId INTEGER;
//    static TypeId BOOLEAN;
};


class qlow::sem::PointerType : public Type
{
    TypeId derefType;
    sem::TypeScope scope;
public:
    inline PointerType(TypeId derefType) :
        derefType{ derefType },
        scope{ *this }
    {
    }
    
    const TypeId& getDerefType(void) const { return derefType; }
    
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
