#ifndef QLOW_SEM_SCOPE_H
#define QLOW_SEM_SCOPE_H

#include <optional>
#include <map>
#include <memory>

#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include "Util.h"
#include "Type.h"
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

        class Scope;
        class GlobalScope;
        class NativeScope;
        class ClassScope;
        class LocalScope;
        class TypeScope;
        class NativeTypeScope;
        
        class Type;
        struct NativeMethod;
    }
}


class qlow::sem::Scope
{
protected:
    Context& context;
public:
    inline Scope(Context& context) :
        context{ context } {}

    virtual ~Scope(void);
    virtual Variable* getVariable(const std::string& name) = 0;
    virtual Method* getMethod(const std::string& name) = 0;
    virtual Type* getType(const ast::Type* name) = 0;
    virtual Type* getReturnableType(void) = 0;
    virtual Method* resolveMethod(const std::string& name,
        const std::vector<Type*>& argumentTypes);

    virtual std::string toString(void) = 0;

    inline Context& getContext(void) { return context; }
};


class qlow::sem::GlobalScope : public Scope
{
public:
    SymbolTable<Class> classes;
    SymbolTable<Method> functions;
    //OwningList<Cast> casts;
public:
    inline GlobalScope(Context& context) :
        Scope{ context } {}

    virtual Variable* getVariable(const std::string& name);
    virtual Method* getMethod(const std::string& name);
    virtual Type* getType(const ast::Type* name);
    virtual Type* getReturnableType(void);

    inline const SymbolTable<Class>& getClasses(void) const { return classes; }
    inline const SymbolTable<Method>& getMethods(void) const { return functions; }

    virtual std::string toString(void);
};


class qlow::sem::NativeScope : public GlobalScope
{
protected:
    std::unordered_map<std::string, Type*> types;
    std::map<NativeType::NType, Type*> typesByNative;
public:
    inline NativeScope(Context& context) :
        GlobalScope{ context } {}

    virtual Type* getType(const ast::Type* name);
    virtual Type* getType(NativeType::NType nt);
    virtual void addNativeType(std::string name, NativeType::NType nt, Type* id);

    virtual std::string toString(void);
};



class qlow::sem::ClassScope : public Scope
{
    Scope& parentScope;
    Class* classRef;
public:
    inline ClassScope(Scope& parentScope, Class* classRef) :
        Scope{ parentScope.getContext() },
        parentScope{ parentScope }, classRef{ classRef }
    {
    }
    virtual Variable* getVariable(const std::string& name);
    virtual Method* getMethod(const std::string& name);
    virtual Type* getType(const ast::Type* name);
    virtual Type* getReturnableType(void);
    virtual std::string toString(void);
};


class qlow::sem::LocalScope : public Scope
{
    Scope& parentScope;
    SymbolTable<Variable> localVariables;
    Type* returnType;
    Method* enclosingMethod;
public:
    LocalScope(Scope& parentScope, Method* enclosingMethod);
    LocalScope(LocalScope& parentScope);

    void putVariable(const std::string& name, std::unique_ptr<Variable> v);
    SymbolTable<Variable>& getLocals(void);

    virtual Variable* getVariable(const std::string& name);
    virtual Method* getMethod(const std::string& name);
    virtual Type* getType(const ast::Type* name);
    virtual Type* getReturnableType(void);
    virtual std::string toString(void);
};


class qlow::sem::TypeScope : public Scope
{
protected:
    Type* type;
public:
    inline TypeScope(Context& context, Type* type) :
        Scope{ context },
        type{ type }
    {
    }
    
    virtual Variable* getVariable(const std::string& name);
    virtual Method* getMethod(const std::string& name);
    virtual Type* getType(const ast::Type* name);
    virtual Type* getReturnableType(void);
    virtual std::string toString(void);

    virtual bool isNativeTypeScope(void) const;
};


class qlow::sem::NativeTypeScope : public TypeScope
{
public:
    SymbolTable<NativeMethod> nativeMethods;
    inline NativeTypeScope(Context& context, Type* type) :
        TypeScope{ context, type }
    {
    }

    NativeTypeScope(NativeTypeScope&&);
    //NativeTypeScope& operator=(NativeTypeScope&&);

    ~NativeTypeScope(void);
    
    virtual Method* getMethod(const std::string& name);
    virtual bool isNativeTypeScope(void) const;

    Type* implementInlineOperation(const std::string&, llvm::Value* a);
};


#endif // QLOW_SEM_SCOPE_H
