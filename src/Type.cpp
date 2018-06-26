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
        /*case Kind::NULL_TYPE:
            return llvm::Type::getVoidTy(context);*/
        case INTEGER:
            return llvm::Type::getInt32Ty(context);
        case BOOLEAN:
            return llvm::Type::getInt1Ty(context);
        /*case Kind::CLASS:
            return data.classType->llvmType;*/
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

