#include "Type.h"

#include "Semantic.h"
#include "Builtin.h"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>


using namespace qlow;

sem::Type::Type(sem::Class* classType) :
    kind{ Kind::CLASS }
{
    data.classType = classType;
}


sem::Type::Type(Kind kind, Class* classType) :
    kind{ kind }
{
    data.classType = classType;
}

bool sem::Type::isClassType(void) const
{
    return kind == Kind::CLASS;
}


bool sem::Type::isNative(void) const
{
    return kind != Kind::CLASS;
}


sem::Class* sem::Type::getClassType(void)
{
    if (kind != Kind::CLASS)
        throw "internal error";
    return data.classType;
}


llvm::Type* sem::Type::getLlvmType(llvm::LLVMContext& context) const
{
    switch (kind) {
        case Kind::NULL_TYPE:
            return llvm::Type::getVoidTy(context);
        case Kind::INTEGER:
            return llvm::Type::getInt32Ty(context);
        case Kind::CLASS:
            return data.classType->llvmType;
    }
}


bool sem::Type::operator == (const Type& other) const
{
    if (other.kind != this->kind)
        return false;
    
    switch (kind) {
        case Kind::CLASS:
            return data.classType == other.data.classType;
        default:
            return true;
    }
}


bool sem::Type::operator != (const Type& other) const
{
    return !(*this == other);
}


const sem::Type sem::Type::NULL_TYPE = sem::Type{ sem::Type::Kind::NULL_TYPE };
const sem::Type sem::Type::INTEGER = sem::Type{ sem::Type::Kind::INTEGER, &sem::int32 };


