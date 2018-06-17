#ifndef QLOW_SEM_SCOPE_H
#define QLOW_SEM_SCOPE_H

#include <optional>
#include <map>
#include <memory>

namespace qlow
{
    namespace sem
    {
        /*!
         * \note contains owning pointers to elements
         */
        template<typename T>
        using SymbolTable = std::map<std::string, std::unique_ptr<T>>;


        struct Class;
        struct Method;
        struct Variable;
        struct Type;

        class Scope;
        class GlobalScope;
        class ClassScope;
        class LocalScope;
    }
}


struct qlow::sem::Type
{
    Class* typeClass;
};


class qlow::sem::Scope
{
public:
    virtual ~Scope(void);
    virtual Variable* getVariable(const std::string& name) = 0;
    virtual Method* getMethod(const std::string& name) = 0;
    virtual std::optional<Type> getType(const std::string& name) = 0;

    virtual std::string toString(void) = 0;
};


class qlow::sem::GlobalScope : public Scope
{
public:
    SymbolTable<Class> classes;
public:
    virtual Variable* getVariable(const std::string& name);
    virtual Method* getMethod(const std::string& name);
    virtual std::optional<Type> getType(const std::string& name);

    virtual std::string toString(void);
};


class qlow::sem::ClassScope : public Scope
{
    Scope& parentScope;
    Class* class_ref;
public:
    inline ClassScope(Scope& parentScope, Class* class_ref) :
        parentScope{ parentScope }, class_ref{ class_ref }
    {
    }
    virtual Variable* getVariable(const std::string& name);
    virtual Method* getMethod(const std::string& name);
    virtual std::optional<Type> getType(const std::string& name);
    virtual std::string toString(void);
};


class qlow::sem::LocalScope : public Scope
{
    Scope& parentScope;
    SymbolTable<Variable> localVariables;
public:
    inline LocalScope(Scope& parentScope) :
        parentScope{ parentScope }
    {
    }
    void putVariable(const std::string& name, std::unique_ptr<Variable> v);
    SymbolTable<Variable>& getLocals(void);

    virtual Variable* getVariable(const std::string& name);
    virtual Method* getMethod(const std::string& name);
    virtual std::optional<Type> getType(const std::string& name);
    virtual std::string toString(void);
};


#endif // QLOW_SEM_SCOPE_H
