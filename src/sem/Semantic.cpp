#include "Semantic.h"
#include "Type.h"
#include "Ast.h"
#include "AstVisitor.h"

#include "CodegenVisitor.h"

#include "Util.h"

#include <memory>

using namespace qlow::sem;

namespace qlow
{
namespace sem
{

std::pair<std::unique_ptr<Context>, std::unique_ptr<GlobalScope>>
    createFromAst(const qlow::ast::Ast& ast)
{
    std::unique_ptr<Context> context = std::make_unique<Context>();
    Printer& printer = Printer::getInstance();
    auto& objects = ast.getObjects();

#ifdef DEBUGGING
    printf("starting building semantic representation (%d objects)\n", (int) objects.size());
#endif

    // create classes
    std::unique_ptr<sem::GlobalScope> globalScope = std::make_unique<sem::GlobalScope>(*context);
    for (auto& astObject : objects) {
        if (auto* cls = dynamic_cast<ast::Class*>(astObject.get()); cls) {
            globalScope->classes[cls->name] = std::make_unique<sem::Class>(cls, *globalScope);
        }
        else if (auto* function = dynamic_cast<ast::MethodDefinition*>(astObject.get()); function) {
            globalScope->functions[function->name] = std::make_unique<sem::Method>(function, *globalScope);
        }
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
                    throw SemanticError(SemanticError::DUPLICATE_FIELD_DECLARATION, field->name, field->pos);
                
                // otherwise add to the fields list
                semClass->fields[field->name] = unique_dynamic_cast<Field>(field->accept(av, semClass->scope));
            }
            else if (auto* method = dynamic_cast<qlow::ast::MethodDefinition*> (feature.get()); method) {
                if (semClass->methods.find(method->name) != semClass->methods.end()) // throw, if method already exists
                    throw SemanticError(SemanticError::DUPLICATE_METHOD_DEFINITION, method->name, method->pos);
                
                // otherwise add to the methods list
                semClass->methods[method->name] = unique_dynamic_cast<Method>(method->accept(av, semClass->scope));
            }
            else {
                // if a feature is neither a method nor a field, something went horribly wrong
                throw "internal error, feature neither field nor method";
            }
        }
    }
    
    for (auto& [name, method] : globalScope->functions) {
        auto returnType = globalScope->getType(method->astNode->type.get());
        if (returnType != NO_TYPE) {
            method->returnType = returnType;
        }
        else {
            SemanticError se(SemanticError::UNKNOWN_TYPE,
                            method->astNode->type->asString(),
                            method->astNode->type->pos);
        }
        
        // otherwise add to the methods list
        globalScope->functions[method->name] = unique_dynamic_cast<Method>(method->astNode->accept(av, *globalScope));
    }
    
#ifdef DEBUGGING
    printf("created all methods and fields\n");
#endif
    
    for (auto& [name, semClass] : globalScope->classes) {
        for (auto& [name, method] : semClass->methods) {
            if (method->astNode->body) { // if not declaration
                method->containingClass = semClass.get();
                method->generateThisExpression();
                method->body = unique_dynamic_cast<sem::DoEndBlock>(method->astNode->body->accept(av, method->scope));
            }
        }
    }
    for (auto& [name, method] : globalScope->functions) {
        if (method->astNode->body) { // if not declaration
            method->body = unique_dynamic_cast<sem::DoEndBlock>(method->astNode->body->accept(av, method->scope));
        }
    }
    
#ifdef DEBUGGING
    printf("created all method bodies\n");
#endif
    
    return std::make_pair(std::move(context), std::move(globalScope));
}

}
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


void Method::generateThisExpression(void)
{
    auto te = std::make_unique<ThisExpression>(this);
    thisExpression = te.get();
    scope.putVariable(this->thisExpression->name, std::move(te));
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

ACCEPT_DEFINITION(LocalVariableExpression, ExpressionCodegenVisitor, llvm::Value*, llvm::IRBuilder<>&)
ACCEPT_DEFINITION(BinaryOperation, ExpressionCodegenVisitor, llvm::Value*, llvm::IRBuilder<>&)
ACCEPT_DEFINITION(CastExpression, ExpressionCodegenVisitor, llvm::Value*, llvm::IRBuilder<>&)
ACCEPT_DEFINITION(NewArrayExpression, ExpressionCodegenVisitor, llvm::Value*, llvm::IRBuilder<>&)
ACCEPT_DEFINITION(UnaryOperation, ExpressionCodegenVisitor, llvm::Value*, llvm::IRBuilder<>&)
ACCEPT_DEFINITION(MethodCallExpression, ExpressionCodegenVisitor, llvm::Value*, llvm::IRBuilder<>&)
ACCEPT_DEFINITION(FieldAccessExpression, ExpressionCodegenVisitor, llvm::Value*, llvm::IRBuilder<>&)
ACCEPT_DEFINITION(AddressExpression, ExpressionCodegenVisitor, llvm::Value*, llvm::IRBuilder<>&)
ACCEPT_DEFINITION(IntConst, ExpressionCodegenVisitor, llvm::Value*, llvm::IRBuilder<>&)
ACCEPT_DEFINITION(ThisExpression, ExpressionCodegenVisitor, llvm::Value*, llvm::IRBuilder<>&)

ACCEPT_DEFINITION(Expression, LValueVisitor, llvm::Value*, llvm::IRBuilder<>&)
ACCEPT_DEFINITION(LocalVariableExpression, LValueVisitor, llvm::Value*, llvm::IRBuilder<>&)
ACCEPT_DEFINITION(FieldAccessExpression, LValueVisitor, llvm::Value*, llvm::IRBuilder<>&)

ACCEPT_DEFINITION(AssignmentStatement, StatementVisitor, llvm::Value*, qlow::gen::FunctionGenerator&) 
ACCEPT_DEFINITION(DoEndBlock, StatementVisitor, llvm::Value*, qlow::gen::FunctionGenerator&) 
ACCEPT_DEFINITION(IfElseBlock, StatementVisitor, llvm::Value*, qlow::gen::FunctionGenerator&) 
ACCEPT_DEFINITION(WhileBlock, StatementVisitor, llvm::Value*, qlow::gen::FunctionGenerator&) 
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


std::string UnaryOperation::toString(void) const
{
    return "UnaryOperation[" + arg->toString() + "]";
}


std::string BinaryOperation::toString(void) const
{
    return "BinaryOperation[" + left->toString() + ", " +
        right->toString() + "]";
}


std::string CastExpression::toString(void) const
{
    // TODO remove optional unwrapping
    return "CastExpression[" + expression->toString() + " to " +
        context.getType(targetType).asString() + "]";
}


std::string NewArrayExpression::toString(void) const
{
    // TODO remove optional unwrapping
    return "NewArrayExpression[" + context.getType(arrayType).asString() + "; " +
        length->toString() + "]";
}


std::string MethodCallExpression::toString(void) const
{
    if (this->target)
        return "MethodCallExpression[" + target->toString() + "." + callee->toString() + "]";
    else
        return "MethodCallExpression[" + callee->toString() + "]";
}


std::string FieldAccessExpression::toString(void) const
{
    if (this->target)
        return "FieldAccessExpression[" + target->toString() + "." + accessed->toString() + "]";
    else
        return "FieldAccessExpression[" + accessed->toString() + "]";
}


std::string FeatureCallStatement::toString(void) const
{
    return "FeatureCallStatement[" + expr->callee->toString() + "]";
}






