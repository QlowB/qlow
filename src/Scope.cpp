#include "Scope.h"

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
    return nullptr;
}


std::optional<sem::Type> sem::GlobalScope::getType(const std::string& name)
{
    auto t = classes.find(name);
    if (t != classes.end())
        return std::make_optional(Type{ t->second.get() });
    return std::nullopt;
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


std::optional<sem::Type> sem::ClassScope::getType(const std::string& name)
{
    return parentScope.getType(name);
}


sem::Variable* sem::MethodScope::getVariable(const std::string& name)
{
    auto m = localVariables.find(name);
    if (m != localVariables.end())
        return (*m).second.get();
    
    return parentScope.getVariable(name);
}


sem::Method * sem::MethodScope::getMethod(const std::string& name)
{
    return parentScope.getMethod(name);
}


std::optional<sem::Type> sem::MethodScope::getType(const std::string& name)
{
    return parentScope.getType(name);
}



