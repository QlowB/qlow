#ifndef QLOW_SEM_TYPE_H
#define QLOW_SEM_TYPE_H

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
    
    using TypeId = size_t;
    const TypeId NO_TYPE = std::numeric_limits<TypeId>::max();

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

    std::unique_ptr<TypeScope> typeScope;
    Union type;

    Type(Context& context, Union type, TypeId id);
    Type(Context& context, Union type, std::string name, TypeId id);

public:
    ~Type(void);
    Type(const Type& other) = delete;
    Type(Type&& other) = default;
    void operator = (const Type& other) = delete;
    Type& operator = (Type&& other) = default;

    Kind getKind(void) const;
    Native getNativeKind(void) const;

    std::string asString(void) const;
    std::string asIdentifier(void) const;
    size_t hash(void) const;

    bool operator == (const Type& other) const;
    inline bool operator != (const Type& other) const { return !this->operator==(other); }

    /**
     * @brief return the class of this type if it is a class type,
     *        <code>nullptr</code> otherwise.
     * @post ensures that if <code>this->getKind() == Kind::CLASS</code>,
     *       it will not return a <code>nullptr</code>
     */
    Class* getClass(void) const;
    
    /**
     * @brief returns the type scope of this type
     */
    inline TypeScope& getTypeScope(void) const { return *typeScope; }
    
    /**
     * @brief sets the type scope of this type
     */
    void setTypeScope(std::unique_ptr<TypeScope> scope);
};


#endif // QLOW_SEM_TYPE_H

