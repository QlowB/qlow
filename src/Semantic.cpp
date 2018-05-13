#include "Semantic.h"
#include "AstVisitor.h"

#include "Util.h"

using namespace qlow::sem;

namespace qlow
{
namespace sem
{

SymbolTable<qlow::sem::Class>
    createFromAst(std::vector<std::unique_ptr<qlow::ast::Class>>& classes)
{

    // create classes
    SymbolTable<sem::Class> semClasses;
    for (auto& astClass : classes) {
        semClasses[astClass->name] = std::make_unique<sem::Class>(astClass.get());
    }

    StructureVisitor av;
    
    // create all methods and fields
    for (auto& [name, semClass] : semClasses) {
        for (auto& feature : semClass->astNode->features) {
            
            if (auto* field = dynamic_cast<qlow::ast::FieldDeclaration*> (feature.get()); field) {
                if (semClass->fields.find(field->name) != semClass->fields.end()) // throw, if field already exists
                    throw SemanticException(SemanticException::DUPLICATE_FIELD_DECLARATION, field->name, field->pos);
                
                // otherwise add to the fields list
                semClass->fields[field->name] = unique_dynamic_cast<Field>(av.visit(*field, semClasses));
            }
            else if (auto* method = dynamic_cast<qlow::ast::MethodDefinition*> (feature.get()); method) {
                if (semClass->methods.find(method->name) != semClass->methods.end()) // throw, if method already exists
                    throw SemanticException(SemanticException::DUPLICATE_METHOD_DEFINITION, method->name, method->pos);
                
                // otherwise add to the methods list
                semClass->methods[method->name] = unique_dynamic_cast<Method>(av.visit(*method, semClasses));
            }
            else {
                // if a feature is neither a method nor a field, something went horribly wrong
                throw "internal error";
            }
        }
    }
    
    for (auto& [name, semClass] : semClasses) {
        for (auto& [name, method] : semClass->methods) {
            method->body = unique_dynamic_cast<sem::DoEndBlock>(av.visit(*method->astNode->body, semClasses));
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
    
    // add fields
    for (auto& field : fields)
        val += field.second->toString() + ", ";
    if (!fields.empty())
        val = val.substr(0, val.length() - 2);
    
    // add methods
    for (auto& method : methods)
        val += method.second->toString() + ", ";
    if (!methods.empty())
        val = val.substr(0, val.length() - 2);
    
    val += " (";
    val += std::to_string(this->astNode->pos.first_line) + ", ";
    val += std::to_string(this->astNode->pos.first_column);
    val += " )";
    
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
    
    std::string pos = std::to_string(where.first_line) + ":" +
        std::to_string(where.first_column);
    
    return pos + ": " + error[errorCode] + ": " + message;
}



