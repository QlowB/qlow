#include "Type.h"
#include "Scope.h"
#include "Context.h"
#include "Semantic.h"
#include "Builtin.h"
#include "ErrorReporting.h"

#include <llvm/IR/Type.h>

#include <map>

using qlow::sem::Type;
using qlow::sem::NativeType;
using qlow::sem::ClassType;
using qlow::sem::ArrayType;

qlow::sem::SemanticObject::~SemanticObject(void) = default;


std::string qlow::sem::SemanticObject::toString(void) const
{
    return "SemanticObject [" + util::toString(this) + "]";
}

Type::Type(void) = default;
Type::~Type(void) = default;


bool Type::operator==(const Type& other) const
{
    // ensured because context only ever creates a type once
    return this->equals(other);
}


qlow::sem::Class* Type::getClass(void) const
{
    return nullptr;
}


void Type::setTypeScope(std::unique_ptr<TypeScope> scope)
{
    this->typeScope = std::move(scope);
}


/*void Type::setLlvmType(llvm::Type* type)
{
    llvmType = type;
}*/


llvm::Type* Type::getLlvmType(llvm::LLVMContext& c) const
{
    return llvmType;
}


bool Type::isClassType(void) const
{
    return false;
}


bool Type::isNativeType(void) const
{
    return false;
}


bool Type::isArrayType(void) const
{
    return false;
}


bool Type::isVoid(void) const
{
    return false;
}


bool NativeType::equals(const Type& other) const
{
    return other.isNativeType() && static_cast<const NativeType&>(other).type == type;
}


bool NativeType::isNativeType(void) const
{
    return true;
}


bool NativeType::isVoid(void) const
{
    return this->type == NType::VOID;
}


std::string NativeType::asString(void) const
{
    static const std::map<NType, std::string> names {
        { NType::VOID, "Void" },
        { NType::INTEGER, "Integer" },
        { NType::BOOLEAN, "Boolean" },
    };
    return names.at(type);
}


std::string NativeType::asIdentifier(void) const
{
    return asString();
}


size_t NativeType::hash(void) const
{
    return static_cast<size_t>(this->type) * 234598245 + 234568276587;
}


void NativeType::createLlvmTypeDecl(llvm::LLVMContext& ctxt)
{
    switch(type) {
    case NType::VOID:
        llvmType = llvm::Type::getVoidTy(ctxt);
        break;
    case NType::INTEGER:
        llvmType = llvm::Type::getInt64Ty(ctxt);
        break;
    case NType::BOOLEAN:
        llvmType = llvm::Type::getInt1Ty(ctxt);
        break;
    }
}


bool ClassType::equals(const Type& other) const
{
    return other.isClassType() && other.getClass() == getClass();
}


qlow::sem::Class* ClassType::getClass(void) const
{
    return type;
}


bool ClassType::isClassType(void) const
{
    return true;
}


std::string ClassType::asString(void) const
{
    return type->name;
}


std::string ClassType::asIdentifier(void) const
{
    return type->name;
}


size_t ClassType::hash(void) const
{
    return reinterpret_cast<size_t>(type);
}


void ClassType::createLlvmTypeDecl(llvm::LLVMContext& ctxt)
{
    llvmType = llvm::StructType::create(ctxt, asIdentifier());
}


bool ArrayType::equals(const Type& other) const
{
    return other.isArrayType() && elementType->equals(*static_cast<const ArrayType&>(other).elementType);
}


bool ArrayType::isArrayType(void) const
{
    return true;
}


std::string ArrayType::asString(void) const
{
    return "[" + elementType->asString() + "]";
}


std::string ArrayType::asIdentifier(void) const
{
    return elementType->asString() + "_arr";
}


size_t ArrayType::hash(void) const
{
    return 2345792834579ull + elementType->hash() * 1234817233;
}


void ArrayType::createLlvmTypeDecl(llvm::LLVMContext& ctxt)
{
    llvmType = llvm::StructType::create(ctxt, asIdentifier());
}
