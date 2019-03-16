#include "Type.h"
#include "Scope.h"
#include "Context.h"
#include "Semantic.h"
#include "Builtin.h"
#include "ErrorReporting.h"

using qlow::sem::TypeId;
using qlow::sem::Type;

qlow::sem::SemanticObject::~SemanticObject(void) = default;


std::string qlow::sem::SemanticObject::toString(void) const
{
    return "SemanticObject [" + util::toString(this) + "]";
}


Type::Type(Context& context, Union type, TypeId id) :
    Type{ context, type, "", id }
{
}


Type::Type(Context& context, Union type, std::string name, TypeId id) :
    name{ std::move(name) },
    type{ std::move(type) }
{
    if (getKind() == Kind::NATIVE) {
        typeScope = std::make_unique<NativeTypeScope>(context, id);
    }
    else {
        typeScope = std::make_unique<TypeScope>(context, id);
    }
}


Type::~Type(void) = default;


Type::Kind Type::getKind(void) const
{
    switch(type.index()) {
        case 0: return Kind::NATIVE;
        case 1: return Kind::CLASS;
        case 2: return Kind::POINTER;
        case 3: return Kind::ARRAY;
    }
    // should never arrive here
    throw InternalError(InternalError::INVALID_TYPE);
}


std::string Type::asString(void) const
{
    return std::visit(
        [&] (const auto& t) -> std::string {
            using T = std::decay_t<decltype(t)>;
            if constexpr (std::is_same<T, NativeType>::value) {
                return this->name;
            }
            else if constexpr (std::is_same<T, ClassType>::value) {
                return this->name;
            }
            else if constexpr (std::is_same<T, PointerType>::value) {
                // TODO rewrite
                //return context.getType(t.targetType) + "*";
                return "*";
            }
            else if constexpr (std::is_same<T, ArrayType>::value) {
                //return "[" + context.getType(t.targetType) + "]";
                return "[]";
            }
        }
        ,
        type
    );
}


size_t Type::hash(void) const
{
    // TODO implement
    return type.index() * 2345325 + std::hash<std::string>()(name);
}


bool Type::operator==(const Type& other) const
{
    return this->name == other.name &&
           this->type == other.type;
}


qlow::sem::Class* Type::getClass(void) const
{
    const auto* classType = std::get_if<ClassType>(&type);
    if (classType)
        return classType->classType;
    else
        return nullptr;
}


void Type::setTypeScope(std::unique_ptr<TypeScope> scope)
{
    this->typeScope = std::move(scope);
}
