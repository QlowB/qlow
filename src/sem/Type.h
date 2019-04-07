#ifndef QLOW_SEM_TYPE_H
#define QLOW_SEM_TYPE_H

#include <vector>
#include <variant>
#include <memory>
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
    class NativeType;
    class ClassType;
    class ArrayType;

    // forward declarations to other files
    struct Class;
    class TypeScope;

    class Context;
}


struct qlow::sem::SemanticObject
{
    Context& context;
    inline SemanticObject(Context& context) :
        context{ context } {}
    virtual ~SemanticObject(void);
    
    /**
     * @brief converts the object to a readable string for debugging purposes. 
     */
    virtual std::string toString(void) const;
};


class qlow::sem::Type
{
    friend class Context;
protected:
    std::unique_ptr<TypeScope> typeScope;
    llvm::Type* llvmType;
    Context& context;

    Type(Context& context);
public:

    virtual ~Type(void);

    Type(const Type& other) = delete;
    Type(Type&& other) = delete;
    void operator = (const Type& other) = delete;
    Type& operator = (Type&& other) = delete;

    virtual std::string asString(void) const = 0;
    virtual std::string asIdentifier(void) const = 0;
    virtual size_t hash(void) const = 0;

    bool operator == (const Type& other) const;
    inline bool operator != (const Type& other) const { return !this->operator==(other); }

    virtual bool equals(const Type& other) const = 0;

    /**
     * @brief return the class of this type if it is a class type,
     *        <code>nullptr</code> otherwise.
     * @post ensures that if <code>this->getKind() == Kind::CLASS</code>,
     *       it will not return a <code>nullptr</code>
     */
    virtual Class* getClass(void) const;

    /**
     * \brief get the type of which this type is an array type of.
     * 
     * \return the type of which this type is an array type of, or
     *         <code>nullptr</code> if this type is not an array type.
     */
    virtual Type* getArrayOf(void) const;

    virtual bool isReferenceType(void) const;
    
    /**
     * @brief returns the type scope of this type
     */
    inline TypeScope& getTypeScope(void) const { return *typeScope; }
    
    /**
     * @brief sets the type scope of this type
     */
    void setTypeScope(std::unique_ptr<TypeScope> scope);

    //virtual void setLlvmType(llvm::Type* type);
    virtual llvm::Type* getLlvmType(llvm::LLVMContext&) const;

    virtual void createLlvmTypeDecl(llvm::LLVMContext&) = 0;

    virtual bool isClassType(void) const;
    virtual bool isStructType(void) const;
    virtual bool isNativeType(void) const;
    virtual bool isArrayType(void) const;

    virtual bool isVoid(void) const;

    inline Context& getContext(void) const { return context; }
};


class qlow::sem::NativeType : public Type
{
    friend class Context;
public:
    enum class NType
    {
        VOID,
        INTEGER,
        BOOLEAN,
        C_CHAR,
        C_SHORT,
        C_INT,
        C_LONG,
    };
    static const std::vector<NType> nativeTypes;
protected:

    NType type;
    inline NativeType(Context& context, NType type) :
        Type{ context },
        type{ type }
    {
    }
public:

    virtual bool equals(const Type& other) const override;

    virtual bool isNativeType(void) const override;
    virtual bool isVoid(void) const override;

    virtual std::string asString(void) const override;
    virtual std::string asIdentifier(void) const override;
    virtual size_t hash(void) const override;

    virtual void createLlvmTypeDecl(llvm::LLVMContext&) override;
};


class qlow::sem::ClassType : public Type
{
    friend class Context;
protected:
    Class* type;
    inline ClassType(Context& context, Class* type) :
        Type{ context },
        type{ type }
    {
    }
public:
    virtual bool equals(const Type& other) const override;
    virtual bool isClassType(void) const override;
    virtual bool isStructType(void) const override;
    virtual bool isReferenceType(void) const override;
    virtual Class* getClass(void) const override;

    virtual std::string asString(void) const override;
    virtual std::string asIdentifier(void) const override;
    virtual size_t hash(void) const override;
    virtual void createLlvmTypeDecl(llvm::LLVMContext&) override;
};


class qlow::sem::ArrayType : public Type
{
    friend class Context;
protected:
    Type* elementType;
    inline ArrayType(Type* elementType) :
        Type{ elementType->getContext() },
        elementType{ elementType }
    {
    }
public:
    virtual bool equals(const Type& other) const override;
    virtual bool isArrayType(void) const override;
    virtual Type* getArrayOf(void) const override;

    virtual std::string asString(void) const override;
    virtual std::string asIdentifier(void) const override;
    virtual size_t hash(void) const override;
    virtual void createLlvmTypeDecl(llvm::LLVMContext&) override;
};


#endif // QLOW_SEM_TYPE_H

