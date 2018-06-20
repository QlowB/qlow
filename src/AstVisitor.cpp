#include "AstVisitor.h"
#include "Ast.h"

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
    auto f = std::make_unique<sem::Field>();
    f->name = ast.name;
    auto type = scope.getType(ast.type);
    if (type) {
        f->type = type.value();
    }
    else {
        throw sem::SemanticException(sem::SemanticException::UNKNOWN_TYPE,
            ast.type,
            ast.pos
        );
    }
    return f;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::MethodDefinition& ast, sem::Scope& scope)
{
    auto returnType = scope.getType(ast.type);
    if (!returnType) {
        throw sem::SemanticException(sem::SemanticException::UNKNOWN_TYPE,
            ast.type,
            ast.pos
        );
    }
    auto m = std::make_unique<sem::Method>(scope, returnType.value());
    m->name = ast.name;
    m->astNode = &ast;
    
    for (auto& arg : ast.arguments) {
        auto var = arg->accept(*this, scope);
        if (dynamic_cast<sem::Variable*>(var.get())) {
            std::unique_ptr<sem::Variable> variable =
                unique_dynamic_cast<sem::Variable>(std::move(var));
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
    auto v = std::make_unique<sem::Variable>();
    v->name = ast.name;
    auto type = scope.getType(ast.type);
    if (type) {
        v->type = type.value();
    }
    else {
        throw sem::SemanticException(sem::SemanticException::UNKNOWN_TYPE,
            ast.type,
            ast.pos
        );
    }
    return v;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::DoEndBlock& ast, sem::Scope& scope)
{
    auto body = std::make_unique<sem::DoEndBlock>(scope);
    for (auto& statement : ast.statements) {
        
        if (ast::NewVariableStatement* nvs = dynamic_cast<ast::NewVariableStatement*>(statement.get()); nvs) {
            auto type = body->scope.getType(nvs->type);

            if (!type)
                throw sem::SemanticException(sem::SemanticException::UNKNOWN_TYPE, nvs->type, nvs->pos);

            auto var = std::make_unique<sem::Variable>(type.value(), nvs->name);
            body->scope.putVariable(nvs->name, std::move(var));
            continue;
        }
        
        auto v = statement->accept(*this, body->scope);
        if (dynamic_cast<sem::FeatureCallExpression*>(v.get()) != nullptr) {
            body->statements.push_back(
                std::make_unique<sem::FeatureCallStatement>(
                unique_dynamic_cast<sem::FeatureCallExpression>(std::move(v))));
        }
        else {
            body->statements.push_back(unique_dynamic_cast<sem::Statement>(std::move(v)));
        }
    }
    return body;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::Statement& ast, sem::Scope& scope)
{
    printf("at: %d:%d to %d:%d\n", ast.pos.first_line, ast.pos.first_column, ast.pos.last_line, ast.pos.last_column);
    printf("type: %s\n", typeid(ast).name());
    throw "visit(Statement) shouldn't be called";
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::Expression& ast, sem::Scope& scope)
{
    throw "visit(Expression) shouldn't be called";
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::FeatureCall& ast, sem::Scope& scope)
{
    auto* method = scope.getMethod(ast.name);
    auto* var = scope.getVariable(ast.name);
    
    if (var) {
        auto lve = std::make_unique<sem::LocalVariableExpression>();
        lve->var = var;
        return lve;
    }
    else if (method) {
        auto fce = std::make_unique<sem::FeatureCallExpression>();
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
        throw sem::SemanticException(sem::SemanticException::FEATURE_NOT_FOUND, ast.name, ast.pos);
    }
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::AssignmentStatement& ast, sem::Scope& scope)
{
    auto as = std::make_unique<sem::AssignmentStatement>();
    
//    as->value = unique_dynamic_cast<sem::Expression>(visit(*ast.expr, classes));
//    as->target = unique_dynamic_cast<sem::Expression>(visit(*ast.target, classes));
    as->value = unique_dynamic_cast<sem::Expression>(ast.expr->accept(*this, scope));
    as->target = unique_dynamic_cast<sem::Expression>(ast.target->accept(*this, scope));
    return as;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::ReturnStatement& ast, sem::Scope& scope)
{
    auto as = std::make_unique<sem::ReturnStatement>();
    as->value = unique_dynamic_cast<sem::Expression>(ast.expr->accept(*this, scope));
    return as;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::NewVariableStatement& ast, sem::Scope& scope)
{
    throw "shouldn't be called";
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::IntConst& ast, sem::Scope& scope)
{
    return std::make_unique<sem::IntConst>(ast.value);
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::UnaryOperation& ast, sem::Scope& scope)
{
    auto ret = std::make_unique<sem::UnaryOperation>();
    ret->op = ast.op;
    ret->side = ast.side;
    ret->arg = unique_dynamic_cast<sem::Expression>(ast.expr->accept(*this, scope));
    return ret;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::BinaryOperation& ast, sem::Scope& scope)
{
    auto ret = std::make_unique<sem::BinaryOperation>();
    ret->op = ast.op;
    ret->left = unique_dynamic_cast<sem::Expression>(ast.left->accept(*this, scope));
    ret->right = unique_dynamic_cast<sem::Expression>(ast.right->accept(*this, scope));
    return ret;
}


