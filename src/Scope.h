#ifndef QLOW_SEM_SCOPE_H
#define QLOW_SEM_SCOPE_H

#include <optional>
#include <map>
#include <memory>

#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include "Util.h"

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

        class Semantic;
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
        using TypeId = size_t;

        class NativeType;
    }
}


class qlow::sem::Scope
{
protected:
    const Semantic& semantic;
public:
    inline Scope(const Semantic& semantic) :
        semantic{ semantic }
    {
    }

    virtual ~Scope(void);
    virtual Variable* getVariable(const std::string& name) = 0;
    virtual Method* getMethod(const std::string& name) = 0;
    virtual std::optional<TypeId> getType(const ast::Type& name) = 0;
    virtual std::optional<TypeId> getReturnableType(void) = 0;
    virtual Method* resolveMethod(const std::string& name,
        const std::vector<TypeId> argumentTypes);

    virtual std::string toString(void) = 0;

    inline const Semantic& getSemantic(void) const { return semantic; }
};


class qlow::sem::GlobalScope : public Scope
{
public:
    SymbolTable<Class> classes;
    SymbolTable<Method> functions;
    OwningList<Cast> casts;
public:
    inline GlobalScope(const Semantic& semantic) :
        Scope{ semantic }
    {
    }


    virtual Variable* getVariable(const std::string& name) override;
    virtual Method* getMethod(const std::string& name) override;
    virtual std::optional<TypeId> getType(const ast::Type& name) override;
    virtual std::optional<TypeId> getReturnableType(void) override;

    virtual std::string toString(void) override;
};


class qlow::sem::NativeScope : public GlobalScope
{
    static NativeScope instance;
public:
    SymbolTable<std::shared_ptr<NativeType>> types;
public:
    inline NativeScope(const Semantic& semantic) :
        GlobalScope{ semantic }
    {
    }

    virtual std::optional<TypeId> getType(const ast::Type& name);

    virtual std::string toString(void) override;
    
    static NativeScope& getInstance(void);
};



class qlow::sem::ClassScope : public Scope
{
    Scope& parentScope;
    Class* classRef;
public:
    inline ClassScope(Scope& parentScope, Class* classRef) :
        Scope{ parentScope.getSemantic() },
        parentScope{ parentScope },
        classRef{ classRef }
    {
    }

    virtual Variable* getVariable(const std::string& name) override;
    virtual Method* getMethod(const std::string& name) override;
    virtual std::optional<TypeId> getType(const ast::Type& name) override;
    virtual std::optional<TypeId> getReturnableType(void) override;
    virtual std::string toString(void) override;
};


class qlow::sem::LocalScope : public Scope
{
    Scope& parentScope;
    SymbolTable<Variable> localVariables;
    std::optional<TypeId> returnType;
    Method* enclosingMethod;
public:
    LocalScope(Scope& parentScope, Method* enclosingMethod);
    LocalScope(LocalScope& parentScope);

    void putVariable(const std::string& name, std::unique_ptr<Variable> v);
    SymbolTable<Variable>& getLocals(void);

    virtual Variable* getVariable(const std::string& name) override;
    virtual Method* getMethod(const std::string& name) override;
    virtual std::optional<TypeId> getType(const ast::Type& name) override;
    virtual std::optional<TypeId> getReturnableType(void) override;
    virtual std::string toString(void) override;
};


class qlow::sem::TypeScope : public Scope
{
protected:
    Type& type;
public:
    inline TypeScope(const Semantic& semantic, Type& type) :
        Scope{ semantic },
        type{ type }
    {
    }
    
    
    virtual Variable* getVariable(const std::string& name) override;
    virtual Method* getMethod(const std::string& name) override;
    virtual std::optional<TypeId> getType(const ast::Type& name) override;
    virtual std::optional<TypeId> getReturnableType(void) override;
    virtual std::string toString(void) override;
};


class qlow::sem::NativeTypeScope : public TypeScope
{
    NativeType& nativeType;
public:
    inline NativeTypeScope(const Semantic& semantic, NativeType& type) :
        TypeScope{ semantic, (Type&) type },
        nativeType{ type }
    {
    }
    
    
    virtual Method* getMethod(const std::string& name) override;
    std::shared_ptr<Type> implementInlineOperation(const std::string&, llvm::Value* a);
};


#endif // QLOW_SEM_SCOPE_H
