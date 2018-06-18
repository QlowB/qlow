#ifndef QLOW_SEM_TYPE_H
#define QLOW_SEM_TYPE_H

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

    inline Type(Kind kind) :
        kind{ kind }, data{ nullptr } {}

    bool isClassType(void) const;
    bool isNative(void) const;

    Class* getClassType(void);


    static const Type INTEGER;

};


#endif // QLOW_SEM_TYPE_H
