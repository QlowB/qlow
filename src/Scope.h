#ifndef QLOW_SEM_SCOPE_H
#define QLOW_SEM_SCOPE_H

#include <optional>
#include <map>
#include <memory>

#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include "Util.h"
#include "Context.h"

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
        class Cast;

        class Scope;
        class GlobalScope;
        class NativeScope;
        class ClassScope;
        class LocalScope;
        class TypeScope;
        class NativeTypeScope;
        
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
    virtual TypeId getType(const ast::Type& name) = 0;
    virtual TypeId getReturnableType(void) = 0;
    virtual Method* resolveMethod(const std::string& name,
        const std::vector<TypeId> argumentTypes);

    virtual std::string toString(void) = 0;
};


class qlow::sem::GlobalScope : public Scope
{
public:
    SymbolTable<Class> classes;
    SymbolTable<Method> functions;
    OwningList<Cast> casts;

    Context typeContext;
public:
    virtual Variable* getVariable(const std::string& name);
    virtual Method* getMethod(const std::string& name);
    virtual TypeId getType(const ast::Type& name);
    virtual TypeId getReturnableType(void);

    virtual std::string toString(void);
};


class qlow::sem::NativeScope : public GlobalScope
{
    static NativeScope instance;
public:
    SymbolTable<std::shared_ptr<NativeType>> types;
public:
    virtual TypeId getType(const ast::Type& name);

    virtual std::string toString(void);
    
    static NativeScope& getInstance(void);
};



class qlow::sem::ClassScope : public Scope
{
    Scope& parentScope;
    Class* classRef;
public:
    inline ClassScope(Scope& parentScope, Class* classRef) :
        parentScope{ parentScope }, classRef{ classRef }
    {
    }
    virtual Variable* getVariable(const std::string& name);
    virtual Method* getMethod(const std::string& name);
    virtual TypeId getType(const ast::Type& name);
    virtual TypeId getReturnableType(void);
    virtual std::string toString(void);
};


class qlow::sem::LocalScope : public Scope
{
    Scope& parentScope;
    SymbolTable<Variable> localVariables;
    TypeId returnType;
    Method* enclosingMethod;
public:
    LocalScope(Scope& parentScope, Method* enclosingMethod);
    LocalScope(LocalScope& parentScope);

    void putVariable(const std::string& name, std::unique_ptr<Variable> v);
    SymbolTable<Variable>& getLocals(void);

    virtual Variable* getVariable(const std::string& name);
    virtual Method* getMethod(const std::string& name);
    virtual TypeId getType(const ast::Type& name);
    virtual TypeId getReturnableType(void);
    virtual std::string toString(void);
};


class qlow::sem::TypeScope : public Scope
{
protected:
    Type& type;
public:
    inline TypeScope(Type& type) :
        type{ type }
    {
    }
    
    virtual Variable* getVariable(const std::string& name);
    virtual Method* getMethod(const std::string& name);
    virtual TypeId getType(const ast::Type& name);
    virtual TypeId getReturnableType(void);
    virtual std::string toString(void);
};


class qlow::sem::NativeTypeScope : public TypeScope
{
    NativeType& nativeType;
public:
    inline NativeTypeScope(NativeType& type) :
        TypeScope{ (Type&) type },
        nativeType{ type }
    {
    }
    
    
    virtual Method* getMethod(const std::string& name);
    TypeId implementInlineOperation(const std::string&, llvm::Value* a);
};


#endif // QLOW_SEM_SCOPE_H
