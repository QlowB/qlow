#include "Scope.h"
#include "Semantic.h"
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


std::optional<sem::Type> sem::GlobalScope::getType(const std::string& name)
{
    auto native = NativeScope::getInstance().getType(name);
    if (native) {
        return native;
    }
    
    auto t = classes.find(name);
    if (t != classes.end())
        return std::make_optional(Type(t->second.get()));
    return std::nullopt;
}


std::optional<sem::Type> sem::GlobalScope::getReturnableType(void)
{
    return std::nullopt;
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


std::optional<sem::Type> sem::NativeScope::getType(const std::string& name)
{
    auto t = types.find(name);
    if (t != types.end())
        return *t->second;
    return std::nullopt;
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


std::optional<sem::Type> sem::ClassScope::getType(const std::string& name)
{
    return parentScope.getType(name);
}


std::optional<sem::Type> sem::ClassScope::getReturnableType(void)
{
    return std::nullopt;
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


std::optional<sem::Type> sem::LocalScope::getType(const std::string& name)
{
    return parentScope.getType(name);
}


std::optional<sem::Type> sem::LocalScope::getReturnableType(void)
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


