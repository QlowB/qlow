#include "Type.h"

#include "Semantic.h"
#include "Builtin.h"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>


using namespace qlow;


sem::Type::~Type(void)
{
}


bool sem::Type::operator == (const Type& other) const
{
    return this == &other;
}


bool sem::Type::operator != (const Type& other) const
{
    return !(*this != other);
}


sem::Type* sem::Type::INTEGER = new sem::NativeType(sem::NativeType::Type::INTEGER);
sem::Type* sem::Type::BOOLEAN = new sem::NativeType(sem::NativeType::Type::BOOLEAN);


llvm::Type* sem::ClassType::getLlvmType (llvm::LLVMContext& context) const
{
    return classType->llvmType;
}


llvm::Type* sem::NativeType::getLlvmType (llvm::LLVMContext& context) const
{
    switch(type) {
        case INTEGER:
            context
    }
}


