#include "Scope.h"
#include "Ast.h"
#include "Semantic.h"
#include "Type.h"
#include "Builtin.h"

using namespace qlow;

sem::Scope::~Scope(void)
{
}


sem::Variable* sem::GlobalScope::getVariable(const std::string& name)
{
    return nullptr;
}


sem::Method* sem::GlobalScope::getMethod(const std::string& name)
{
    if (const auto& f = functions.find(name); f != functions.end()) {
        return f->second.get();
    }
    return nullptr;
}


sem::Type* sem::GlobalScope::getType(const ast::Type& name)
{
    if (const auto* arr = dynamic_cast<const ast::ArrayType*>(&name); arr) {
        return new sem::ArrayType(getType(*arr->arrayType));
    }
    
    auto native = NativeScope::getInstance().getType(name);
    if (native) {
        return native;
    }
    
    const auto* classType = dynamic_cast<const ast::ClassType*>(&name);
   
    if (!classType)
        throw "internal error, non class-type top-level type";
    
    
    
    auto t = classes.find(classType->typeName);
    if (t != classes.end())
        return new sem::ClassType(t->second.get());
    
    return nullptr;
}


sem::Type* sem::GlobalScope::getReturnableType(void)
{
    return nullptr;
}


std::string sem::GlobalScope::toString(void)
{
    std::string ret;
    ret += "Classes:\n";
    for (auto& [name, c] : classes) {
        ret += "\t";
        ret += c->toString() + "\n";
    }
    return ret;
}


sem::Type* sem::NativeScope::getType(const ast::Type& name)
{
    if (const auto* arr = dynamic_cast<const ast::ArrayType*>(&name); arr) {
        return new sem::ArrayType(getType(*arr->arrayType));
    }
    
    const auto* classType = dynamic_cast<const ast::ClassType*>(&name);
   
    if (!classType)
        return sem::Type::VOID;
    
    
    auto t = types.find(classType->typeName);
    if (t != types.end())
        return t->second.get();
    
    return nullptr;
}


sem::NativeScope sem::NativeScope::instance = sem::generateNativeScope();
sem::NativeScope& sem::NativeScope::getInstance(void)
{
    return instance;
}


std::string sem::NativeScope::toString(void)
{
    return "NativeScope";
}


sem::Variable* sem::ClassScope::getVariable(const std::string& name)
{
    if (class_ref == nullptr)
        return parentScope.getVariable(name);
    auto m = class_ref->fields.find(name);
    if (m != class_ref->fields.end())
        return (*m).second.get();
    
    return parentScope.getVariable(name);
}


sem::Method * sem::ClassScope::getMethod(const std::string& name)
{
    if (class_ref == nullptr)
        return parentScope.getMethod(name);
    auto m = class_ref->methods.find(name);
    if (m != class_ref->methods.end())
        return (*m).second.get();
    
    return parentScope.getMethod(name);
}


std::string sem::ClassScope::toString(void)
{
    std::string ret;
    for (auto& [name, m] : class_ref->methods) {
        ret += "\t";
        ret += m->toString() + "\n";
    }
    for (auto& [name, f] : class_ref->fields) {
        ret += "\t";
        ret += f->toString() + "\n";
    }

    return ret + "\nParent:\n" + parentScope.toString();
}


sem::Type* sem::ClassScope::getType(const ast::Type& name)
{
    return parentScope.getType(name);
}


sem::Type* sem::ClassScope::getReturnableType(void)
{
    return nullptr;
}


void sem::LocalScope::putVariable(const std::string& name, std::unique_ptr<Variable> v)
{
    localVariables.insert({name, std::move(v)});
}


sem::SymbolTable<sem::Variable>& sem::LocalScope::getLocals(void)
{
    return localVariables;
}


sem::Variable* sem::LocalScope::getVariable(const std::string& name)
{
    auto m = localVariables.find(name);
    if (m != localVariables.end())
        return (*m).second.get();
    
    return parentScope.getVariable(name);
}


sem::Method* sem::LocalScope::getMethod(const std::string& name)
{
    return parentScope.getMethod(name);
}


sem::Type* sem::LocalScope::getType(const ast::Type& name)
{
    return parentScope.getType(name);
}


sem::Type* sem::LocalScope::getReturnableType(void)
{
    return returnType;
}


std::string sem::LocalScope::toString(void)
{
    std::string ret;
    for (auto& [name, v] : localVariables) {
        ret += "\t";
        ret += v->toString() + "\n";
    }

    return ret + "\nParent:\n" + parentScope.toString();
}


