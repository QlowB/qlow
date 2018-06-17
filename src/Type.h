#ifndef QLOW_SEM_TYPE_H
#define QLOW_SEM_TYPE_H

namespace qlow
{
    namespace sem
    {
        // forward declarations
        class Class;
    }


    namespace sem
    {
        class Type;

        class ClassType;
        class NativeType;
    }
}


class qlow::sem::Type
{
public:
    virtual ~Type(void);

    virtual bool isNative(void) const = 0;
};


class qlow::sem::ClassType : public Type
{
    Class* classType;
public:

    inline ClassType(Class* classType) :
        classType{ classType } {}

    Class* getClass(void) { return classType; }

    virtual bool isNative(void) const;
};


class qlow::sem::NativeType : public Type
{
public:

    virtual bool isNative(void) const;
};


#endif // QLOW_SEM_TYPE_H
