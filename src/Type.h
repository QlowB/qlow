#ifndef QLOW_SEM_TYPE_H
#define QLOW_SEM_TYPE_H

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
        struct NativeType;
    }


    namespace sem
    {
        class Type;
    }
}


class qlow::sem::Type
{
public:
    enum class Kind
    {
        NULL_TYPE,
        INTEGER,
        BOOLEAN,
        CLASS,
    };

private:
    union Data
    {
        Class* classType;
    };

    Kind kind;
    Data data;

public:

    inline Type(void) :
        kind{ Kind::NULL_TYPE }, data{ nullptr } {}

    Type(Class* classType);
    Type(Kind kind, qlow::sem::Class* classType);

    inline Type(Kind kind) :
        kind{ kind }, data{ nullptr } {}

    bool isClassType(void) const;
    bool isNative(void) const;

    Class* getClassType(void);
    llvm::Type* getLlvmType(llvm::LLVMContext& context) const;
    
    bool operator == (const Type& other) const;
    bool operator != (const Type& other) const;

    static const Type NULL_TYPE;
    static const Type INTEGER;
    static const Type BOOLEAN;
};


#endif // QLOW_SEM_TYPE_H
