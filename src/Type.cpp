#include "Type.h"

#include "Semantic.h"
#include "Builtin.h"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>


using namespace qlow;


sem::Type::~Type(void)
{
}


bool sem::Type::equals(const Type* other) const
{
    return this == other;
}


sem::Type* sem::Type::VOID = new sem::NativeType(sem::NativeType::Type::VOID);
sem::Type* sem::Type::INTEGER = new sem::NativeType(sem::NativeType::Type::INTEGER);
sem::Type* sem::Type::BOOLEAN = new sem::NativeType(sem::NativeType::Type::BOOLEAN);


llvm::Type* sem::ClassType::getLlvmType (llvm::LLVMContext& context) const
{
    return classType->llvmType;
}


bool sem::ClassType::equals(const Type* other) const
{
    if (auto* oct = dynamic_cast<const ClassType*>(other); oct) {
        return this->classType == oct->classType;
    }
    else {
        return false;
    }
}


llvm::Type* sem::ArrayType::getLlvmType (llvm::LLVMContext& context) const
{
    // TODO implement
    return nullptr;
}


bool sem::ArrayType::equals(const Type* other) const
{
    if (auto* oct = dynamic_cast<const ArrayType*>(other); oct) {
        return this->arrayType->equals(oct->arrayType);
    }
    else {
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


bool sem::NativeType::equals(const sem::Type* other) const
{
    if (auto* oct = dynamic_cast<const NativeType*>(other); oct) {
        return this->type == oct->type;
    }
    else {
        return false;
    }
}

