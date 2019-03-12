#include "Scope.h"
#include "Ast.h"
#include "Semantic.h"
#include "Type.h"
#include "Builtin.h"

using namespace qlow;

sem::Scope::~Scope(void)
{
}


sem::Method* sem::Scope::resolveMethod(const std::string& name,
    const std::vector<TypeId> argumentTypes)
{
    sem::Method* m = getMethod(name);
    if (!m)
        return nullptr;
    
    if (m->arguments.size() != argumentTypes.size())
        return nullptr;
    
    for (size_t i = 0; i < argumentTypes.size(); i++) {
        if (!m->arguments[i]->type == argumentTypes[i])
            return nullptr;
    }
    
    return m;
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


sem::TypeId sem::GlobalScope::getType(const ast::Type& name)
{
    if (const auto* arr = dynamic_cast<const ast::ArrayType*>(&name); arr) {
        return context.getArrayOf(getType(*arr->arrayType));
    }
    
    if (const auto* ptr = dynamic_cast<const ast::PointerType*>(&name)) {
        return context.getPointerTo(getType(*ptr->derefType));
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
        return context.addType(Type::createClassType(context, t->second.get()));
    
    return NO_TYPE;
}


qlow::sem::TypeId sem::GlobalScope::getReturnableType(void)
{
    return NO_TYPE;
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


sem::TypeId sem::NativeScope::getType(const ast::Type& name)
{
    if (const auto* arr = dynamic_cast<const ast::ArrayType*>(&name); arr) {
        return context.getArrayOf(getType(*arr->arrayType));
    }
    
    const auto* classType = dynamic_cast<const ast::ClassType*>(&name);
   
    if (!classType)
        return NO_TYPE;
    
    
    auto t = types.find(classType->typeName);
    if (t != types.end())
        return t->second;
    
    return NO_TYPE;
}


// TODO rewrite
static sem::Context c;
sem::NativeScope sem::NativeScope::instance = sem::generateNativeScope(c);
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
    if (classRef == nullptr)
        return parentScope.getVariable(name);
    auto m = classRef->fields.find(name);
    if (m != classRef->fields.end())
        return (*m).second.get();
    
    return parentScope.getVariable(name);
}


sem::Method* sem::ClassScope::getMethod(const std::string& name)
{
    if (classRef == nullptr)
        return parentScope.getMethod(name);
    auto m = classRef->methods.find(name);
    if (m != classRef->methods.end())
        return (*m).second.get();
    
    return parentScope.getMethod(name);
}


std::string sem::ClassScope::toString(void)
{
    std::string ret;
    for (auto& [name, m] : classRef->methods) {
        ret += "\t";
        ret += m->toString() + "\n";
    }
    for (auto& [name, f] : classRef->fields) {
        ret += "\t";
        ret += f->toString() + "\n";
    }

    return ret + "\nParent:\n" + parentScope.toString();
}


sem::TypeId sem::ClassScope::getType(const ast::Type& name)
{
    return parentScope.getType(name);
}


sem::TypeId sem::ClassScope::getReturnableType(void)
{
    return NO_TYPE;
}


sem::LocalScope::LocalScope(Scope& parentScope, Method* enclosingMethod) :
    Scope{ parentScope.getContext() },
    parentScope{ parentScope },
    returnType{ enclosingMethod->returnType },
    enclosingMethod{ enclosingMethod }
{
}

sem::LocalScope::LocalScope(LocalScope& parentScope) :
    Scope{ parentScope.getContext() },
    parentScope{ parentScope },
    returnType{ parentScope.returnType },
    enclosingMethod{ parentScope.enclosingMethod }
{
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
    /*
    if (name == "this") {
        return enclosingMethod->thisExpression.get();
    }
    */
    
    auto m = localVariables.find(name);
    if (m != localVariables.end())
        return (*m).second.get();
    
    return parentScope.getVariable(name);
}


sem::Method* sem::LocalScope::getMethod(const std::string& name)
{
    return parentScope.getMethod(name);
}


sem::TypeId sem::LocalScope::getType(const ast::Type& name)
{
    return parentScope.getType(name);
}


sem::TypeId sem::LocalScope::getReturnableType(void)
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


sem::Variable* sem::TypeScope::getVariable(const std::string& name)
{
    if (auto ty = context.getType(type)) {
        if (ty.value().get().getKind() == Type::Kind::CLASS) {
            auto& fields = ty.value().get().getClass()->fields;
            if (fields.find(name) != fields.end())
                return fields[name].get();
        }
    }
    return nullptr;
}



sem::Method* sem::TypeScope::getMethod(const std::string& name)
{
    if (auto ty = context.getType(type)) {
        if (ty.value().get().getKind() == Type::Kind::CLASS) {
            auto classRef = ty.value().get().getClass();
            auto& methods = classRef->methods;
            if (methods.find(name) != methods.end())
                return methods[name].get();
        }
    }
    return nullptr;
}


sem::TypeId sem::TypeScope::getType(const ast::Type& name)
{
    return NO_TYPE;
}


sem::TypeId sem::TypeScope::getReturnableType(void)
{
    return NO_TYPE;
}


std::string sem::TypeScope::toString(void)
{
    std::string ret;
    return ret;
}


sem::Method* sem::NativeTypeScope::getMethod(const std::string& name)
{
    /*auto m = nativeType.nativeMethods.find(name);
    if (m != nativeType.nativeMethods.end())
        return m->second.get();
    else*/
        return TypeScope::getMethod(name);
}




