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


Type::Native Type::getNativeKind(void) const
{
    return std::get<NativeType>(this->type).type;
}


std::string Type::asString(void) const
{
    using namespace std::literals;
    return std::visit(
        [&] (const auto& t) -> std::string {
            using T = std::decay_t<decltype(t)>;
            if constexpr (std::is_same<T, NativeType>::value) {
                return "native";
            }
            else if constexpr (std::is_same<T, ClassType>::value) {
                return this->getClass()->name;
            }
            else if constexpr (std::is_same<T, PointerType>::value) {
                return this->typeScope->getContext().getType(t.targetType).asString() + "*";
            }
            else if constexpr (std::is_same<T, ArrayType>::value) {
                return "["s + this->typeScope->getContext().getType(t.targetType).asString() + "]";
            }
        }
        ,
        type
    );
}


size_t Type::hash(void) const
{
    auto value1 = std::visit(
        [&] (const auto& t) -> size_t {
            using T = std::decay_t<decltype(t)>;
            if constexpr (std::is_same<T, NativeType>::value) {
                return static_cast<size_t>(t.type) * 2345279;
            }
            else if constexpr (std::is_same<T, ClassType>::value) {
                return reinterpret_cast<size_t>(t.classType) * 1;
            }
            else if constexpr (std::is_same<T, PointerType>::value) {
                return t.targetType * 143115587;
            }
            else if constexpr (std::is_same<T, ArrayType>::value) {
                //return "[" + context.getType(t.targetType) + "]";
                return t.targetType * 2342345;
            }
        },
        type
    );
    auto h = type.index() * 11111111111111111 + value1;
    Printer::getInstance() << h << std::endl;
    return h;
}


bool Type::operator==(const Type& other) const
{
    return //this->name == other.name &&
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
