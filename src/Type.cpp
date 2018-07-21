#include "Type.h"

#include "Semantic.h"
#include "Builtin.h"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>


using namespace qlow;


sem::Type::~Type(void)
{
}


bool sem::Type::equals(const Type& other) const
{
    return this == &other;
}


/*std::shared_ptr<sem::Type> sem::Type::VOID =
    std::make_shared<sem::NativeType>(sem::NativeType::Type::VOID);
std::shared_ptr<sem::Type> sem::Type::INTEGER =
    std::make_shared<sem::NativeType>(sem::NativeType::Type::INTEGER);
std::shared_ptr<sem::Type> sem::Type::BOOLEAN =
    std::make_shared<sem::NativeType>(sem::NativeType::Type::BOOLEAN);
*/

std::string sem::ClassType::asString(void) const
{
    return classType->name;
}



sem::Scope& sem::ClassType::getScope(void)
{
    return scope;
}


llvm::Type* sem::ClassType::getLlvmType (llvm::LLVMContext& context) const
{
    return classType->llvmType;
}


bool sem::ClassType::equals(const Type& other) const
{
    if (auto* oct = dynamic_cast<const ClassType*>(&other); oct) {
        return this->classType == oct->classType;
    }
    else {
        return false;
    }
}


std::string sem::ArrayType::asString(void) const
{
    return std::string("[") + arrayType->asString() + "]";
}


sem::Scope& sem::ArrayType::getScope(void)
{
    return scope;
}


llvm::Type* sem::ArrayType::getLlvmType (llvm::LLVMContext& context) const
{
    // TODO implement
    return nullptr;
}


bool sem::ArrayType::equals(const Type& other) const
{
    if (auto* oct = dynamic_cast<const ArrayType*>(&other); oct) {
        return this->arrayType->equals(*oct->arrayType);
    }
    else {
        return false;
    }
}


std::string sem::NativeType::asString(void) const
{
    switch(type) {
        case VOID:
            return "Void";
        case INTEGER:
            return "Integer";
        case BOOLEAN:
            return "Boolean";
        case CHAR:
            return "Char";
        case INT8:
            return "Int8";
        case INT16:
            return "Int16";
        case INT32:
            return "Int32";
        case INT64:
            return "Int64";
        case INT128:
            return "Int128";
        case UINT8:
            return "UInt8";
        case UINT16:
            return "UInt16";
        case UINT32:
            return "UInt32";
        case UINT64:
            return "UInt64";
        case UINT128:
            return "UInt128";
        case FLOAT32:
            return "Float32";
        case FLOAT64:
            return "Float64";
        case FLOAT128:
            return "Float128";
    }
}


sem::Scope& sem::NativeType::getScope(void)
{
    return scope;
}


bool sem::NativeType::isIntegerType(void) const
{
    switch(type) {
        case INTEGER:
        case INT8:
        case INT16:
        case INT32:
        case INT64:
        case INT128:
        case UINT8:
        case UINT16:
        case UINT32:
        case UINT64:
        case UINT128:
            return true;
        default:
            return false;
    }
}


llvm::Type* sem::NativeType::getLlvmType (llvm::LLVMContext& context) const
{
    switch (type) {
        case VOID:
            return llvm::Type::getVoidTy(context);
        case INTEGER:
            return llvm::Type::getInt32Ty(context);
        case BOOLEAN:
            return llvm::Type::getInt1Ty(context);
        case CHAR:
            return llvm::Type::getInt32Ty(context);
            
        case INT8:
            return llvm::Type::getInt8Ty(context);
        case INT16:
            return llvm::Type::getInt16Ty(context);
        case INT32:
            return llvm::Type::getInt32Ty(context);
        case INT64:
            return llvm::Type::getInt64Ty(context);
        case INT128:
            return llvm::Type::getInt128Ty(context);
            
        case UINT8:
            return llvm::Type::getInt8Ty(context);
        case UINT16:
            return llvm::Type::getInt16Ty(context);
        case UINT32:
            return llvm::Type::getInt32Ty(context);
        case UINT64:
            return llvm::Type::getInt64Ty(context);
        case UINT128:
            return llvm::Type::getInt128Ty(context);
            
        case FLOAT32:
            return llvm::Type::getFloatTy(context);
        case FLOAT64:
            return llvm::Type::getDoubleTy(context);
        case FLOAT128:
            return llvm::Type::getFP128Ty(context);
    }
}


bool sem::NativeType::equals(const sem::Type& other) const
{
    if (auto* oct = dynamic_cast<const NativeType*>(&other); oct) {
        return this->type == oct->type;
    }
    else {
        return false;
    }
}


llvm::Value* sem::NativeType::generateImplicitCast(llvm::Value* value)
{
    // TODO implement
}

