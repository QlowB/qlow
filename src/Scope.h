#ifndef QLOW_SEM_SCOPE_H
#define QLOW_SEM_SCOPE_H

#include <optional>
#include <map>
#include <memory>

namespace qlow
{
    namespace ast
    {
        struct Type;
    }
    
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

        class Scope;
        class GlobalScope;
        class NativeScope;
        class ClassScope;
        class LocalScope;
        
        class Type;
        class NativeType;
    }
}


class qlow::sem::Scope
{
public:
    virtual ~Scope(void);
    virtual Variable* getVariable(const std::string& name) = 0;
    virtual Method* getMethod(const std::string& name) = 0;
    virtual std::shared_ptr<Type> getType(const ast::Type& name) = 0;
    virtual std::shared_ptr<Type> getReturnableType(void) = 0;

    virtual std::string toString(void) = 0;
};


class qlow::sem::GlobalScope : public Scope
{
public:
    SymbolTable<Class> classes;
    SymbolTable<Method> functions;
public:
    virtual Variable* getVariable(const std::string& name);
    virtual Method* getMethod(const std::string& name);
    virtual std::shared_ptr<Type> getType(const ast::Type& name);
    virtual std::shared_ptr<Type> getReturnableType(void);

    virtual std::string toString(void);
};


class qlow::sem::NativeScope : public GlobalScope
{
    static NativeScope instance;
public:
    SymbolTable<std::shared_ptr<NativeType>> types;
public:
    virtual std::shared_ptr<Type> getType(const ast::Type& name);

    virtual std::string toString(void);
    
    static NativeScope& getInstance(void);
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
    virtual std::shared_ptr<Type> getType(const ast::Type& name);
    virtual std::shared_ptr<Type> getReturnableType(void);
    virtual std::string toString(void);
};


class qlow::sem::LocalScope : public Scope
{
    Scope& parentScope;
    SymbolTable<Variable> localVariables;
    std::shared_ptr<Type> returnType;
public:
    inline LocalScope(Scope& parentScope, Type* returnType) :
        parentScope{ parentScope },
        returnType{ returnType }
    {
    }

    inline LocalScope(Scope& parentScope) :
        parentScope{ parentScope }
    {
        auto returnable = parentScope.getReturnableType();
        if (returnable) {
            returnType = std::move(returnable);
        }
        else {
            returnType = nullptr;
        }
    }

    void putVariable(const std::string& name, std::unique_ptr<Variable> v);
    SymbolTable<Variable>& getLocals(void);

    virtual Variable* getVariable(const std::string& name);
    virtual Method* getMethod(const std::string& name);
    virtual std::shared_ptr<Type> getType(const ast::Type& name);
    virtual std::shared_ptr<Type> getReturnableType(void);
    virtual std::string toString(void);
};


#endif // QLOW_SEM_SCOPE_H
