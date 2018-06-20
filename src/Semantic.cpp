#include "Semantic.h"
#include "AstVisitor.h"

#include "CodegenVisitor.h"

#include "Util.h"

using namespace qlow::sem;

namespace qlow
{
namespace sem
{

std::unique_ptr<GlobalScope>
    createFromAst(const std::vector<std::unique_ptr<qlow::ast::Class>>& classes)
{

#ifdef DEBUGGING
    printf("starting building semantic representation\n");
#endif

    // create classes
    std::unique_ptr<sem::GlobalScope> globalScope = std::make_unique<sem::GlobalScope>();
    for (auto& astClass : classes) {
        globalScope->classes[astClass->name] = std::make_unique<sem::Class>(astClass.get(), *globalScope);
    }

#ifdef DEBUGGING
    printf("created symbol table entries for all classes\n");
#endif 
        
    StructureVisitor av;
    
    // create all methods and fields
    for (auto& [name, semClass] : globalScope->classes) {
        for (auto& feature : semClass->astNode->features) {
            
            if (auto* field = dynamic_cast<qlow::ast::FieldDeclaration*> (feature.get()); field) {
                if (semClass->fields.find(field->name) != semClass->fields.end()) // throw, if field already exists
                    throw SemanticException(SemanticException::DUPLICATE_FIELD_DECLARATION, field->name, field->pos);
                
                // otherwise add to the fields list
                semClass->fields[field->name] = unique_dynamic_cast<Field>(field->accept(av, semClass->scope));
            }
            else if (auto* method = dynamic_cast<qlow::ast::MethodDefinition*> (feature.get()); method) {
                if (semClass->methods.find(method->name) != semClass->methods.end()) // throw, if method already exists
                    throw SemanticException(SemanticException::DUPLICATE_METHOD_DEFINITION, method->name, method->pos);
                
                // otherwise add to the methods list
                semClass->methods[method->name] = unique_dynamic_cast<Method>(method->accept(av, semClass->scope));
            }
            else {
                // if a feature is neither a method nor a field, something went horribly wrong
                throw "internal error";
            }
        }
    }
    
#ifdef DEBUGGING
    printf("created all methods and fields\n");
#endif
    
    for (auto& [name, semClass] : globalScope->classes) {
        for (auto& [name, method] : semClass->methods) {
            method->body = unique_dynamic_cast<sem::DoEndBlock>(av.visit(*method->astNode->body, method->scope));
        }
    }
#ifdef DEBUGGING
    printf("created all method bodies\n");
#endif
    
    return globalScope;
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


std::string Variable::toString(void) const
{
    return "Variable[" + this->name + "]";
}


std::string Field::toString(void) const
{
    return "Field[" + this->name + "]";
}


std::string Method::toString(void) const
{
    return "Method[" + this->name + "]";
}



#define COMMA ,
#define ACCEPT_DEFINITION(ClassName, Visitor, ReturnType, Arg) \
ReturnType ClassName::accept(Visitor& v, Arg arg) \
{ \
    return v.visit(*this, arg); \
}

ACCEPT_DEFINITION(LocalVariableExpression, ExpressionVisitor, std::pair<llvm::Value* COMMA Type>, llvm::IRBuilder<>&)
ACCEPT_DEFINITION(BinaryOperation, ExpressionVisitor, std::pair<llvm::Value* COMMA Type>, llvm::IRBuilder<>&)
ACCEPT_DEFINITION(UnaryOperation, ExpressionVisitor, std::pair<llvm::Value* COMMA Type>, llvm::IRBuilder<>&)
ACCEPT_DEFINITION(FeatureCallExpression, ExpressionVisitor, std::pair<llvm::Value* COMMA Type>, llvm::IRBuilder<>&)
ACCEPT_DEFINITION(IntConst, ExpressionVisitor, std::pair<llvm::Value* COMMA Type>, llvm::IRBuilder<>&)

ACCEPT_DEFINITION(AssignmentStatement, StatementVisitor, llvm::Value*, qlow::gen::FunctionGenerator&) 
ACCEPT_DEFINITION(ReturnStatement, StatementVisitor, llvm::Value*, qlow::gen::FunctionGenerator&) 
ACCEPT_DEFINITION(FeatureCallStatement, StatementVisitor, llvm::Value*, qlow::gen::FunctionGenerator&) 

std::string AssignmentStatement::toString(void) const
{
    return "AssignmentStatement[" + this->target->toString() + " := " +
        this->value->toString() + "]";
}


std::string ReturnStatement::toString(void) const
{
    return "ReturnStatement[" + this->value->toString() + "]";
}


std::string LocalVariableExpression::toString(void) const
{
    return "LocalVariableExpression[" + var->name + "]";
}


std::string BinaryOperation::toString(void) const
{
    return "BinaryOperation[" + left->toString() + ", " +
        right->toString() + "]";
}


std::string UnaryOperation::toString(void) const
{
    return "UnaryOperation[" + arg->toString() + "]";
}


std::string FeatureCallExpression::toString(void) const
{
    return "FeatureCallExpression[" + callee->toString() + "]";
}


std::string FeatureCallStatement::toString(void) const
{
    return "FeatureCallStatement[" + expr->callee->toString() + "]";
}


std::string SemanticException::getMessage(void) const
{
    static std::map<ErrorCode, std::string> error = {
        {UNKNOWN_TYPE, "unknown type"},
        {FEATURE_NOT_FOUND, "method or variable not found"}
    };
    
    std::string pos = std::to_string(where.first_line) + ":" +
        std::to_string(where.first_column);
    
    return pos + ": " + error[errorCode] + ": " + message;
}





