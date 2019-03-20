#include "Context.h"
#include "Semantic.h"
#include "Scope.h"
#include "Builtin.h"


using qlow::sem::Context;
using namespace qlow;

size_t std::hash<std::reference_wrapper<qlow::sem::Type>>::operator() (const std::reference_wrapper<qlow::sem::Type>& t) const
{
    return t.get().hash();
}


Context::Context(void)
{
    nativeScope = std::make_unique<NativeScope>(sem::generateNativeScope(*this));
    sem::fillNativeScope(*nativeScope);
}


qlow::sem::TypeId Context::addType(Type&& type) {
    if (typesMap.count(type) != 0) {
        return typesMap[type];
    }
    else {
        Type gogo = std::move(type);
        types.push_back({ std::move(gogo), nullptr });
        auto id = types.size() - 1;
        typesMap[types[id].first] = id;
        return id;
    }
}


qlow::sem::Type& Context::getType(TypeId tid)
{
    if (tid <= types.size())
        return types[tid].first;
    else
        throw InternalError(InternalError::INVALID_TYPE);
}


std::optional<std::reference_wrapper<sem::Type>> Context::getMaybeType(TypeId tid)
{
    if (tid <= types.size())
        return std::make_optional(std::reference_wrapper<Type>(types[tid].first));
    else
        return std::nullopt;
}


std::string Context::getTypeString(TypeId tid)
{
    if (auto type = getMaybeType(tid))
        return type.value().get().asString();
    else
        return "";
}


qlow::sem::TypeId Context::getVoidTypeId(void)
{
    return getNativeTypeId(Type::Native::VOID);
}


qlow::sem::TypeId Context::getNativeTypeId(Type::Native n)
{
    return nativeScope->getType(n);
}


qlow::sem::NativeScope& Context::getNativeScope(void)
{
    return *nativeScope;
}

// TODO rewrite, so on creating already existant type, there should be no need to create type at all
sem::TypeId Context::createNativeType(std::string name, Type::Native type)
{
    TypeId reserved = types.size();
    Type t = { *this, Type::Union{ Type::NativeType{ type } }, std::move(name), reserved };
    return addType(std::move(t));
}


sem::TypeId Context::createClassType(Class* classType)
{
    TypeId reserved = types.size();
    Type t = Type{ *this, Type::Union{ Type::ClassType{ classType }}, reserved };
    return addType(std::move(t));
}


sem::TypeId Context::createPointerType(TypeId pointsTo)
{
    TypeId reserved = types.size();
    Type t = Type{ *this, Type::Union{ Type::PointerType{ pointsTo }}, reserved };
    return addType(std::move(t));
}


sem::TypeId Context::createArrayType(TypeId pointsTo)
{
    TypeId reserved = types.size();
    Type t = Type{ *this, Type::Union{ Type::ArrayType{ pointsTo }}, reserved };
    return addType(std::move(t));
}


llvm::Type* Context::getLlvmType(TypeId id, llvm::LLVMContext& llvmCtxt)
{
    // TODO at the moment, all types are integers --> fix that
    if (id < types.size()) {
        auto& llt = types[id].second;
        return llt;
    }
    else {
        return nullptr;
    }
}


void Context::createLlvmTypes(llvm::LLVMContext& llvmCtxt)
{
    for (auto& [type, llvmType] : types) {
        if (type.getKind() != Type::Kind::NATIVE) {
            llvmType = llvm::StructType::create(llvmCtxt, type.asIdentifier());
        }
    }
    for (auto& [type, llvmType] : types) {
        if (type.getKind() == Type::Kind::NATIVE) {
            switch (type.getNativeKind()) {
            case Type::Native::BOOLEAN:
                llvmType = llvm::Type::getInt1Ty(llvmCtxt);
                break;
            case Type::Native::INTEGER:
                llvmType = llvm::Type::getInt64Ty(llvmCtxt);
                break;
            case Type::Native::VOID:
                llvmType = llvm::Type::getVoidTy(llvmCtxt);
                break;
            }
        }
    }
    for (auto& [type, llvmType] : types) {
        if (type.getKind() == Type::Kind::CLASS) {
            std::vector<llvm::Type*> structTypes;

            for (auto& [name, field] : type.getClass()->fields) {
                structTypes.push_back(types[field->type].second);
                field->llvmStructIndex = structTypes.size() - 1;
            }

            llvm::dyn_cast<llvm::StructType>(llvmType)->setBody(llvm::ArrayRef(structTypes));
            llvmType = llvmType->getPointerTo();
        }
    }
}

