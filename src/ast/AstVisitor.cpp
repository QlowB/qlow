#include "AstVisitor.h"
#include "Ast.h"
#include "ErrorReporting.h"

#include <typeinfo>

#include "Util.h"

using namespace qlow;


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::Class& ast, sem::Scope& scope)
{
    //auto c = std::make_unique<sem::Class>();
    //c->name = ast.name;
    //return c;
    throw "shouldn't be called";
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::FeatureDeclaration& ast, sem::Scope& scope)
{
    // not needed, because 
    throw "shouldn't be called";
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::FieldDeclaration& ast, sem::Scope& scope)
{
    auto f = std::make_unique<sem::Field>(scope.getContext());
    f->name = ast.name;
    auto type = scope.getType(*ast.type);
    if (type) {
        f->type = type;
    }
    else {
        throw SemanticError(SemanticError::UNKNOWN_TYPE,
            ast.type->asString(),
            ast.type->pos
        );
    }
    return f;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::MethodDefinition& ast, sem::Scope& scope)
{
    auto returnType = scope.getType(*ast.type);
    if (!returnType) {
        throw SemanticError(SemanticError::UNKNOWN_TYPE,
            ast.type->asString(),
            ast.type->pos
        );
    }
    auto m = std::make_unique<sem::Method>(scope, returnType);
    m->name = ast.name;
    m->astNode = &ast;
    
    for (auto& arg : ast.arguments) {
        auto var = arg->accept(*this, scope);
        if (dynamic_cast<sem::Variable*>(var.get())) {
            std::unique_ptr<sem::Variable> variable =
                unique_dynamic_cast<sem::Variable>(std::move(var));
            variable->isParameter = true;
            m->arguments.push_back(variable.get());
            std::string varname = variable->name;
            m->scope.putVariable(varname, std::move(variable));
        }
        else {
            throw "internal error creating argument";
        }
    }
    
    return m;
    //throw "  std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::MethodDefinition& ast, sem::Scope& scope) shouldn't be called";
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::VariableDeclaration& ast, sem::Scope& scope)
{
    auto v = std::make_unique<sem::Variable>(scope.getContext());
    v->name = ast.name;
    auto type = scope.getType(*ast.type);
    if (type) {
        v->type = std::move(type);
    }
    else {
        throw SemanticError(SemanticError::UNKNOWN_TYPE,
            ast.type->asString(),
            ast.type->pos
        );
    }
    return v;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::Statement& ast, sem::Scope& scope)
{
    printf("at: %d:%d to %d:%d\n", ast.pos.first_line, ast.pos.first_column, ast.pos.last_line, ast.pos.last_column);
    printf("type: %s\n", typeid(ast).name());
    throw "visit(Statement) shouldn't be called";
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::DoEndBlock& ast, sem::Scope& scope)
{
    sem::LocalScope* lscope = dynamic_cast<sem::LocalScope*>(&scope);
    if (!lscope)
        throw "error: non-method scope inside method";
    auto body = std::make_unique<sem::DoEndBlock>(*lscope);
    for (auto& statement : ast.statements) {
        
        if (ast::LocalVariableStatement* nvs = dynamic_cast<ast::LocalVariableStatement*>(statement.get()); nvs) {
            auto type = body->scope.getType(*nvs->type);

            if (!type)
                throw SemanticError(SemanticError::UNKNOWN_TYPE,
                                    nvs->type->asString(),
                                    nvs->type->pos);
            auto var = std::make_unique<sem::Variable>(scope.getContext(), std::move(type), nvs->name);
            body->scope.putVariable(nvs->name, std::move(var));
            continue;
        }
        
        auto v = statement->accept(*this, body->scope);
        if (dynamic_cast<sem::MethodCallExpression*>(v.get()) != nullptr) {
            body->statements.push_back(
                std::make_unique<sem::FeatureCallStatement>(
                unique_dynamic_cast<sem::MethodCallExpression>(std::move(v))));
        }
        else {
            body->statements.push_back(unique_dynamic_cast<sem::Statement>(std::move(v)));
        }
    }
    return body;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::IfElseBlock& ast, sem::Scope& scope)
{
    auto condition = ast.condition->accept(*this, scope);
    auto ifB = ast.ifBlock->accept(*this, scope);
    auto eB= ast.elseBlock->accept(*this, scope);
    
    if (!dynamic_cast<sem::DoEndBlock*>(ifB.get())
        || !dynamic_cast<sem::DoEndBlock*>(eB.get())
        || !dynamic_cast<sem::Expression*>(condition.get()))
        throw "internal error, invalid if block";
    
    auto condExpr = unique_dynamic_cast<sem::Expression>(std::move(condition));
    auto ifBBlock = unique_dynamic_cast<sem::DoEndBlock>(std::move(ifB));
    auto eBBlock= unique_dynamic_cast<sem::DoEndBlock>(std::move(eB));
    
    auto ieb = std::make_unique<sem::IfElseBlock>(std::move(condExpr), std::move(ifBBlock), std::move(eBBlock));
    
    return ieb;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::WhileBlock& ast, sem::Scope& scope)
{
    auto condition = ast.condition->accept(*this, scope);
    auto body = ast.body->accept(*this, scope);
    
    if (!dynamic_cast<sem::DoEndBlock*>(body.get()) ||
        !dynamic_cast<sem::Expression*>(condition.get()))
        throw "internal error, invalid while block";
    
    auto condExpr = unique_dynamic_cast<sem::Expression>(std::move(condition));
    auto bodyblock = unique_dynamic_cast<sem::DoEndBlock>(std::move(body));
    
    auto wb = std::make_unique<sem::WhileBlock>(std::move(condExpr), std::move(bodyblock));
    
    return wb;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::Expression& ast, sem::Scope& scope)
{
    throw "visit(Expression) shouldn't be called";
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::FeatureCall& ast, sem::Scope& scope)
{
    std::unique_ptr<sem::Expression> target = nullptr;
    if (ast.target) {
        target = unique_dynamic_cast<sem::Expression>(
            ast.target->accept(*this, scope));
    }
    
    sem::Method* method;
    sem::Variable* var;
    
    // TODO rewrite types
    if (target) {
        //method = target->type->getScope().getMethod(ast.name);
        //var = target->type->getScope().getVariable(ast.name);
    }
    else {
        method = scope.getMethod(ast.name);
        var = scope.getVariable(ast.name);
    }
    
    if (target) {
        if (var) {
            return std::make_unique<sem::FieldAccessExpression>(std::move(target), dynamic_cast<sem::Field*>(var));
        }
        else if (method) {
            auto fce = std::make_unique<sem::MethodCallExpression>(
                std::move(target), method);
    
            if (ast.arguments.size() != method->arguments.size())
                throw SemanticError(SemanticError::WRONG_NUMBER_OF_ARGUMENTS, ast.name, ast.pos);
            for (size_t i = 0; i < ast.arguments.size(); i++) {
                auto& arg = ast.arguments[i];
                auto& argTypeShouldHave = method->arguments[i]->type;
                auto argument = arg->accept(*this, scope);
                if (sem::Expression* expr =
                        dynamic_cast<sem::Expression*>(argument.get()); expr) {
                    if (expr->type != argTypeShouldHave)
                        throw SemanticError(SemanticError::TYPE_MISMATCH,
                            "argument passed to function has wrong type",
                            // TODO rewrite types
                            //expr->type->asString() + "' instead of '" +
                            //argTypeShouldHave->asString() + "'",
                            arg->pos
                        );
                    fce->arguments.push_back(
                        unique_dynamic_cast<sem::Expression>(std::move(argument)));
                }
                else {
                    throw "internal error: non-expression passed as function parameter";
                }
            }
            return fce;
        }
        else {
            throw SemanticError(SemanticError::FEATURE_NOT_FOUND, ast.name, ast.pos);
        }
    }
    else if (var) {
        if (sem::Field* field = dynamic_cast<sem::Field*>(var); field) {
            auto* thisExpr = scope.getVariable("this");
            if (!thisExpr)
                throw "no this found";
            //Printer::getInstance().debug() << "feature call " << var->toString() << " is a field\n";
            return std::make_unique<sem::FieldAccessExpression>(std::make_unique<sem::LocalVariableExpression>(thisExpr), field);
        }
        else {
            //Printer::getInstance().debug() << "feature call " << var->toString() << " is not a field\n";
            return std::make_unique<sem::LocalVariableExpression>(var);
        }
    }
    else if (method) {
        auto fce = std::make_unique<sem::MethodCallExpression>(nullptr, method);
        for (auto& arg : ast.arguments) {
            auto argument = arg->accept(*this, scope);
            if (dynamic_cast<sem::Expression*>(argument.get())) {
                fce->arguments.push_back(unique_dynamic_cast<sem::Expression>(std::move(argument)));
            }
            else {
                throw "internal error: non-expression passed as function parameter";
            }
        }
        fce->callee = method;
        return fce;
    }
    else {
#ifdef DEBUGGING
        printf("var not found: %s\n", ast.name.c_str());
        printf("current scope: %s\n", scope.toString().c_str());
#endif
        throw SemanticError(SemanticError::FEATURE_NOT_FOUND, ast.name, ast.pos);
    }
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::AssignmentStatement& ast, sem::Scope& scope)
{
    auto as = std::make_unique<sem::AssignmentStatement>(scope.getContext());
    
//    as->value = unique_dynamic_cast<sem::Expression>(visit(*ast.expr, classes));
//    as->target = unique_dynamic_cast<sem::Expression>(visit(*ast.target, classes));
    as->value = unique_dynamic_cast<sem::Expression>(ast.expr->accept(*this, scope));
    as->target = unique_dynamic_cast<sem::Expression>(ast.target->accept(*this, scope));
    
    if (as->target->type == as->value->type) {
        return as;
    }
    else {
        throw SemanticError(
            SemanticError::TYPE_MISMATCH,
            "Can't assign expression of type to type.",
            // TODO rewrite
            //"Can't assign expression of type '" + as->value->type->asString() +
            //"' to value of type '" + as->target->type->asString() + "'.",
            ast.pos
        );
    }
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::ReturnStatement& ast, sem::Scope& scope)
{
    auto shouldReturn = scope.getReturnableType();
    
    if (shouldReturn == sem::NO_TYPE) {
        if (ast.expr == nullptr)
            return std::make_unique<sem::ReturnStatement>(scope.getContext());
        else
            throw SemanticError(
                SemanticError::INVALID_RETURN_TYPE,
                "This method should not return any value.",
                ast.expr->pos
            );
    }
    else if (ast.expr == nullptr) {
        throw SemanticError(
            SemanticError::INVALID_RETURN_TYPE,
            "This method should return a value.",
            ast.pos
        );
    }
    
    auto returnValue = unique_dynamic_cast<sem::Expression>(ast.expr->accept(*this, scope));
    
    if (shouldReturn != returnValue->type) {
        throw SemanticError(
            SemanticError::INVALID_RETURN_TYPE,
            //TODO rewrite
            "wrong return value",
            //"return value must be of type '" + shouldReturn->asString() + "' (not '" +
            //returnValue->type->asString() + "')",
            ast.expr->pos
        );
    }
    
    auto as = std::make_unique<sem::ReturnStatement>(scope.getContext());
    as->value = std::move(returnValue);
    return as;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::LocalVariableStatement& ast, sem::Scope& scope)
{
    throw "shouldn't be called";
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(
    ast::AddressExpression& ast, sem::Scope& scope)
{
    auto target = unique_dynamic_cast<sem::Expression>(ast.target->accept(*this, scope));
    auto& targetType = target->type;
    
    if (!target->isLValue()) {
        // TODO rewrite
        //throw NotLValue(targetType->asString(), ast.pos);
        throw NotLValue("type", ast.pos);
    }
    
    return std::make_unique<sem::AddressExpression>(std::move(target));
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::IntConst& ast, sem::Scope& scope)
{
    return std::make_unique<sem::IntConst>(scope.getContext(), ast.value);
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::UnaryOperation& ast, sem::Scope& scope)
{
    auto argument = unique_dynamic_cast<sem::Expression>(ast.expr->accept(*this, scope));
    auto ret = std::make_unique<sem::UnaryOperation>(scope.getContext(), argument->type);
            // TODO not a feasible assumption
    ret->opString = ast.opString;
    ret->side = ast.side;
    ret->arg = std::move(argument);
    return ret;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::BinaryOperation& ast, sem::Scope& scope)
{
    auto leftEval = unique_dynamic_cast<sem::Expression>(ast.left->accept(*this, scope));
    auto rightEval = unique_dynamic_cast<sem::Expression>(ast.right->accept(*this, scope));
    
    throw SemanticError(SemanticError::OPERATOR_NOT_FOUND, "TODO implement", ast.pos);
    /*
    sem::Method* operationMethod = leftEval->type->getScope().resolveMethod(
        ast.opString, { rightEval->type }
    );
    
    Printer::getInstance().debug() << "looked for operation method for operator " <<
    ast.opString << std::endl;
    if (!operationMethod) {
        throw SemanticError(SemanticError::OPERATOR_NOT_FOUND,
            "operator " + ast.opString + " not found for types '" +
            leftEval->type->asString() + "' and '" + rightEval->type->asString() + "'",
            ast.opPos);
    }
    
    auto ret = std::make_unique<sem::BinaryOperation>(leftEval->type, &ast);
    
    ret->operationMethod = operationMethod;
    ret->opString = ast.opString;
    ret->left = std::move(leftEval);
    ret->right = std::move(rightEval);
    return ret;*/
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::NewArrayExpression& ast, sem::Scope& scope)
{
    auto ret = std::make_unique<sem::NewArrayExpression>(scope.getContext(), scope.getType(*ast.type));
    return ret;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::CastExpression& ast, sem::Scope& scope)
{
    auto expr = unique_dynamic_cast<sem::Expression>(ast.expression->accept(*this, scope));
    auto type = scope.getType(*ast.targetType);
    return std::make_unique<sem::CastExpression>(
        std::move(expr), std::move(type), &ast);
}

