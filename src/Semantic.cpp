#include "Semantic.h"
#include "AstVisitor.h"

#include "Util.h"

using namespace qlow::sem;

namespace qlow
{
namespace sem
{

/// i don't like this, but I lack better ideas at the moment.
/// TODO: find better solution

/*!
* \brief tries to cast a unique_ptr and throws if it fails
*/
template<typename T, typename U>
std::unique_ptr<T> unique_dynamic_cast(std::unique_ptr<U>&& p)
{
    U* released = p.release();
    if (T* casted = dynamic_cast<T*>(released); casted)
        return std::unique_ptr<T> (casted);
    else {
        delete released;
        throw "invalid unique_dynamic_cast";
    }
}


SymbolTable<qlow::sem::Class>
    createFromAst(std::vector<std::unique_ptr<qlow::ast::Class>>& classes)
{

    // create classes
    SymbolTable<sem::Class> semClasses;
    for (auto& astClass : classes) {
        semClasses.insert(
            {
                astClass->name,
                std::make_unique<sem::Class>(astClass.get())
            }
        );
    }

    StructureVisitor av;
    
    // create all methods and fields
    for (auto& [name, semClass] : semClasses) {
        for (auto& feature : semClass->astNode->features) {
            if (auto* field = dynamic_cast<qlow::ast::FieldDeclaration*> (feature.get()); field) {
                semClass->fields.insert({
                    field->name,
                    unique_dynamic_cast<Field>(av.visit(*field, semClasses))
                });
            }
            if (auto* method = dynamic_cast<qlow::ast::MethodDefinition*> (feature.get()); method) {
                semClass->methods.insert({
                    method->name,
                    unique_dynamic_cast<Method>(av.visit(*method, semClasses))
                });
            }
        }
    }
    return semClasses;
}

}
}


SemanticObject::~SemanticObject(void)
{
}


std::string SemanticObject::toString(void) const
{
    return "SemanticObject [" + util::toString(this) + "]";
}


std::string Class::toString(void) const
{
    std::string val = "Class[";
    for (auto& field : fields)
        val += field.second->toString() + ", ";
    if (!fields.empty())
        val = val.substr(0, val.length() - 2);
    return val + "]";
}


std::string Field::toString(void) const
{
    return "Field[" + this->name + "]";
}


std::string Method::toString(void) const
{
    return "Method[" + this->name + "]";
}


std::string SemanticException::getMessage(void) const
{
    std::map<ErrorCode, std::string> error = {
        {UNKNOWN_TYPE, "unknown type"},
    };
    
    return error[errorCode] + ": " + message;
}


