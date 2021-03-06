#include "AstVisitor.h"
#include "Ast.h"
#include "ErrorReporting.h"
#include "Context.h"

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
    auto* type = scope.getType(ast.type.get());
    if (type != nullptr) {
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
    auto returnType = scope.getType(ast.type.get());
    if (returnType == nullptr) {
        throw SemanticError(SemanticError::UNKNOWN_TYPE,
            ast.type->asString(),
            ast.type->pos
        );
    }
    auto m = std::make_unique<sem::Method>(scope, returnType, ast.isExtern());
    m->name = ast.name;
    m->astNode = &ast;
    
    for (auto& arg : ast.arguments) {
        auto var = arg->accept(*this, scope);
        if (dynamic_cast<sem::Variable*>(var.get())) {
            std::unique_ptr<sem::Variable> variable =
                unique_dynamic_cast<sem::Variable>(std::move(var));
            variable->isParameter = true;
            m->arguments.push_back(variable.get());
            std::string& varname = variable->name;
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
    auto type = scope.getType(ast.type.get());
    if (type != nullptr) {
        v->type = type;
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
            auto type = body->scope.getType(nvs->type.get());

            if (type == nullptr)
                throw SemanticError(SemanticError::UNKNOWN_TYPE,
                                    nvs->type->asString(),
                                    nvs->type->pos);
            auto var = std::make_unique<sem::Variable>(scope.getContext(), type, nvs->name);
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
    // TODO rewrite this mess

    std::unique_ptr<sem::Expression> target = nullptr;
    if (ast.target) {
        auto expr = ast.target->accept(*this, scope);
        target = unique_dynamic_cast<sem::Expression>(std::move(expr));
    }

    sem::Method* method;
    sem::Variable* var;

    if (target) {
        auto& typeScope = target->type->getTypeScope();
        method = typeScope.getMethod(ast.name);
        var = typeScope.getVariable(ast.name);
    }
    else {
        method = scope.getMethod(ast.name);
        var = scope.getVariable(ast.name);
    }

    if (target) {
        if (var) {
            return std::make_unique<sem::FieldAccessExpression>(std::move(target), dynamic_cast<sem::Field*>(var), ast.pos);
        }
        else if (method) {
            auto fce = std::make_unique<sem::MethodCallExpression>(
                std::move(target), method, ast.pos);

            if (ast.arguments.size() != method->arguments.size())
                throw SemanticError(SemanticError::WRONG_NUMBER_OF_ARGUMENTS, ast.name, ast.pos);
            for (size_t i = 0; i < ast.arguments.size(); i++) {
                auto& arg = ast.arguments[i];
                auto& argTypeShouldHave = method->arguments[i]->type;
                auto argument = arg->accept(*this, scope);
                if (dynamic_cast<sem::Expression*>(argument.get())) {
                    auto expr = unique_dynamic_cast<sem::Expression>(std::move(argument));
                    auto castedParameter = createImplicitCast(std::move(expr), argTypeShouldHave, scope);
                    fce->arguments.push_back(std::move(castedParameter));
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
            return std::make_unique<sem::FieldAccessExpression>(std::make_unique<sem::LocalVariableExpression>(thisExpr, CodePosition::none()), field, ast.pos);
        }
        else {
            return std::make_unique<sem::LocalVariableExpression>(var, ast.pos);
        }
    }
    else if (method) {
        // create implicit 'this'
        std::unique_ptr<sem::Expression> thisExpr = nullptr;
        if (method->containingClass != nullptr) {
            auto* thisVar = scope.getVariable("this");
            if (!thisVar)
                throw SemanticError(SemanticError::UNKNOWN_TYPE, "no this found", ast.pos);
            thisExpr = std::make_unique<sem::LocalVariableExpression>(thisVar, CodePosition::none());
        }
        if (ast.arguments.size() != method->arguments.size())
                throw SemanticError(SemanticError::WRONG_NUMBER_OF_ARGUMENTS, ast.name, ast.pos);
        auto fce = std::make_unique<sem::MethodCallExpression>(std::move(thisExpr), method, ast.pos);
        for (size_t i = 0; i < ast.arguments.size(); i++) {
            auto& arg = ast.arguments[i];
            auto& argTypeShouldHave = method->arguments[i]->type;
            auto argument = arg->accept(*this, scope);

            if (dynamic_cast<sem::Expression*>(argument.get())) {
                auto expr = unique_dynamic_cast<sem::Expression>(std::move(argument));
                auto castedParameter = createImplicitCast(std::move(expr), argTypeShouldHave, scope);
                fce->arguments.push_back(std::move(castedParameter));
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

    if (as->target->type->operator==(*as->value->type)) {
        return as;
    }
    else {
        throw SemanticError(
            SemanticError::TYPE_MISMATCH,
            "Can't assign expression of type '" + as->value->type->asString() +
            "' to variable of type '" + as->target->type->asString() + "'.",
            ast.pos
        );
    }
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::ReturnStatement& ast, sem::Scope& scope)
{
    auto shouldReturn = scope.getReturnableType();
    
    if (shouldReturn == nullptr) {
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
        auto should = shouldReturn->asString();
        auto is = returnValue->type->asString();
        throw SemanticError::invalidReturnType(should, is, ast.expr->pos);
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
    auto targetType = target->type;
    
    if (!target->isLValue()) {
        throw NotLValue(targetType->asString(), ast.pos);
    }
    
    return std::make_unique<sem::AddressExpression>(std::move(target));
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(
    ast::ArrayAccessExpression& ast, sem::Scope& scope)
{
    auto array = unique_dynamic_cast<sem::Expression>(ast.array->accept(*this, scope));
    auto index = unique_dynamic_cast<sem::Expression>(ast.index->accept(*this, scope));

    return std::make_unique<sem::ArrayAccessExpression>(std::move(array), std::move(index), ast.pos);
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::IntConst& ast, sem::Scope& scope)
{
    return std::make_unique<sem::IntConst>(scope.getContext(), ast.value, ast.pos);
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::StringConst& ast, sem::Scope& scope)
{
    // TODO implement
    return nullptr;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::UnaryOperation& ast, sem::Scope& scope)
{
    auto argument = unique_dynamic_cast<sem::Expression>(ast.expr->accept(*this, scope));
    auto ret = std::make_unique<sem::UnaryOperation>(scope.getContext(), argument->type, ast.pos);
            // TODO not a feasible assumption
    ret->opString = ast.opString;
    ret->side = ast.side;
    ret->arg = std::move(argument);
    return ret;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::BinaryOperation& ast, sem::Scope& scope)
{
    sem::Context& context = scope.getContext();

    auto leftEval = unique_dynamic_cast<sem::Expression>(ast.left->accept(*this, scope));
    auto rightEval = unique_dynamic_cast<sem::Expression>(ast.right->accept(*this, scope));

    auto* leftType = leftEval->type;
    auto* rightType = rightEval->type;

    auto& scop =  leftType->getTypeScope();
    sem::Method* operationMethod = scop.resolveMethod(
        ast.opString, { rightEval->type }
    );

#ifdef DEBUGGING
    Printer::getInstance() << "looked for operation method for operator " <<
        ast.opString << std::endl;
#endif

    if (!operationMethod) {
        throw SemanticError(SemanticError::OPERATOR_NOT_FOUND,
            "operator " + ast.opString + " not found for types '" +
            leftType->asString() + "' and '" + rightType->asString() + "'",
            ast.opPos);
    }
    
    auto ret = std::make_unique<sem::BinaryOperation>(context, leftEval->type, &ast);
    
    ret->operationMethod = operationMethod;
    ret->opString = ast.opString;
    ret->left = std::move(leftEval);
    ret->right = std::move(rightEval);
    return ret;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::NewExpression& ast, sem::Scope& scope)
{
    auto ret = std::make_unique<sem::NewExpression>(scope.getContext(), scope.getType(ast.type.get()), ast.pos);
    auto* classType = ret->type->getClass();
    if (classType != nullptr && classType->isReferenceType) {
        return ret;
    }
    else {
        throw SemanticError::newForNonClass(ret->type->asString(), ast.pos);
    }
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::NewArrayExpression& ast, sem::Scope& scope)
{
    auto length = unique_dynamic_cast<sem::Expression>(ast.length->accept(*this, scope));
    auto ret = std::make_unique<sem::NewArrayExpression>(scope.getType(ast.type.get()), std::move(length), ast.pos);
    return ret;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::CastExpression& ast, sem::Scope& scope)
{
    auto expr = unique_dynamic_cast<sem::Expression>(ast.expression->accept(*this, scope));
    auto type = scope.getType(ast.targetType.get());
    return std::make_unique<sem::CastExpression>(
        std::move(expr), type, &ast, false, ast.pos);
}


std::unique_ptr<sem::Expression> StructureVisitor::createImplicitCast(
        std::unique_ptr<sem::Expression> expr, sem::Type* targetType, sem::Scope& scope)
{
#ifdef DEBUGGING
    Printer::getInstance() << "casting " << expr->type->asString() << " to " << targetType->asString() << std::endl;
#endif
    if (expr->type->equals(*targetType))
        return expr;
    auto* exprType = expr->type;
    if (false /*exprType->isImplicitelyCastableTo(targetType)*/) {
        return std::make_unique<sem::CastExpression>(
            std::move(expr), targetType, nullptr, true, expr->pos);
    }
    else {
        // TODO add position to error
        throw qlow::SemanticError(SemanticError::TYPE_MISMATCH,
                "cannot cast from '" + exprType->asString() + "' to '" + targetType->asString() + "'",
                expr->pos);
    }
}




