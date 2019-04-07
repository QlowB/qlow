#include "Context.h"
#include "Semantic.h"
#include "Scope.h"
#include "Builtin.h"


using qlow::sem::Context;
using namespace qlow;

size_t std::hash<qlow::sem::Type*>::operator() (qlow::sem::Type* t) const
{
    return t->hash();
}


Context::Context(void)
{
    nativeScope = std::make_unique<NativeScope>(sem::generateNativeScope(*this));
    sem::fillNativeScope(*nativeScope);
}


qlow::sem::Type* Context::getVoidType(void)
{
    // maybe small optimization potential here
    return getNativeType(NativeType::NType::VOID);
}


qlow::sem::NativeScope& Context::getNativeScope(void)
{
    return *nativeScope;
}


sem::Type* Context::getNativeType(NativeType::NType nativeType)
{
    auto t = std::unique_ptr<NativeType>(new NativeType(*this, nativeType));

    const auto& find = typeMap.find(t.get());
    if (find != typeMap.end()) {
        return types[find->second].get();
    }
    else {
        typeMap[t.get()] = types.size();
        types.push_back(std::move(t));
        return types[types.size() - 1].get();
    }
}


sem::Type* Context::getClassType(Class* classType)
{
    auto t = std::unique_ptr<ClassType>(new ClassType(*this, classType));

    const auto& find = typeMap.find(t.get());
    if (find != typeMap.end()) {
        return types[find->second].get();
    }
    else {
        typeMap[t.get()] = types.size();
        types.push_back(std::move(t));
        return types[types.size() - 1].get();
    }
}


sem::Type* Context::getArrayType(Type* pointsTo)
{
    auto t = std::unique_ptr<ArrayType>(new ArrayType(pointsTo));

    const auto& find = typeMap.find(t.get());
    if (find != typeMap.end()) {
        return types[find->second].get();
    }
    else {
        typeMap[t.get()] = types.size();
        types.push_back(std::move(t));
        return types[types.size() - 1].get();
    }
}


void Context::createLlvmTypes(llvm::LLVMContext& llvmCtxt)
{
    for (auto& type : types) {
        if (type->isNativeType()) {
            type->createLlvmTypeDecl(llvmCtxt);
        }
        else {
            // all structs and array types are structs
            type->llvmType = llvm::StructType::create(llvmCtxt, type->asIdentifier());
        }
    }
    for (auto& type : types) {
        if (type->isClassType()) {
            std::vector<llvm::Type*> structTypes;

            for (auto& [name, field] : type->getClass()->fields) {
                structTypes.push_back(field->type->getLlvmType(llvmCtxt));
                field->llvmStructIndex = structTypes.size() - 1;
            }

            llvm::dyn_cast<llvm::StructType>(type->llvmType)->setBody(llvm::ArrayRef(structTypes));
            if (type->getClass()->isReferenceType)
                type->llvmType = type->llvmType->getPointerTo();
        }
        if (type->isArrayType()) {
            ArrayType* arrType = static_cast<ArrayType*>(type.get());
            std::vector<llvm::Type*> structTypes {
                arrType->elementType->getLlvmType(llvmCtxt)->getPointerTo(),    // elements pointer
                llvm::Type::getInt64Ty(llvmCtxt)                                // length
            };
            llvm::dyn_cast<llvm::StructType>(type->llvmType)->setBody(llvm::ArrayRef(structTypes));
        }
    }
}

