#ifndef QLOW_SEM_SCOPE_H
#define QLOW_SEM_SCOPE_H

#include <optional>

#include "Semantic.h"

namespace qlow
{
    namespace sem
    {
        class Scope;
        class GlobalScope;
        class ClassScope;
        class MethodScope;
    }
}


class qlow::sem::Scope
{
public:
    virtual ~Scope(void);
    virtual Variable* getVariable(const std::string& name) = 0;
    virtual Method* getMethod(const std::string& name) = 0;
    virtual std::optional<Type> getType(const std::string& name) = 0;
};


class qlow::sem::GlobalScope : public Scope
{
public:
    SymbolTable<Class> classes;
public:
    virtual Variable* getVariable(const std::string& name);
    virtual Method* getMethod(const std::string& name);
    virtual std::optional<Type> getType(const std::string& name);
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
};


class qlow::sem::MethodScope : public Scope
{
    Scope& parentScope;
    SymbolTable<Variable> localVariables;
public:
    inline MethodScope(Scope& parentScope) :
        parentScope{ parentScope }
    {
    }
    virtual Variable* getVariable(const std::string& name);
    virtual Method* getMethod(const std::string& name);
    virtual std::optional<Type> getType(const std::string& name);
};


#endif // QLOW_SEM_SCOPE_H
