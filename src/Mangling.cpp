#include "Mangling.h"
#include "Semantic.h"
#include "Type.h"


std::string numberEncode(const std::string& x)
{
    return std::to_string(x.length()) + x;
}


std::string qlow::mangle(const qlow::sem::Method& method)
{
    static const std::string prefix = "_Q";

    std::string mangled = prefix;
    sem::Class* parent = method.containingClass;

    if (parent != nullptr) {
        mangled += numberEncode(parent->name);
    }

    mangled += numberEncode(method.name);

    for (const sem::Variable* arg : method.arguments) {
        mangled += mangle(arg->type);
    }

    return mangled;
}


std::string qlow::mangle(const qlow::sem::Type* type)
{
    if (type->isClassType()) {
        return "C" + numberEncode(type->getClass()->name);
    }
    else if (type->isArrayType()) {
        return "A" + mangle(type->getArrayOf());
    }

    else {
        return "N";
    }
}



