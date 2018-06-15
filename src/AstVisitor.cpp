#include "AstVisitor.h"
#include "Ast.h"

#include <typeinfo>

#include "Util.h"

using namespace qlow;


sem::Class* StructureVisitor::getType(const std::string& type, sem::Scope& scope)
{
    auto t = scope.getType(type);
    if (t)
        return t.value().typeClass;
    else
        return nullptr;
}

std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::Class& ast, sem::Scope& scope)
{
    auto c = std::make_unique<sem::Class>();
    c->name = ast.name;
    return c;
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
        f->type = type.value().typeClass;
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
    auto m = std::make_unique<sem::Method>();
    m->name = ast.name;
    auto returnType = scope.getType(ast.type);
    if (returnType) {
        m->returnType = returnType.value();
    }
    else {
        throw sem::SemanticException(sem::SemanticException::UNKNOWN_TYPE,
            ast.type,
            ast.pos
        );
    }
    m->astNode = &ast;
    return m;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::VariableDeclaration& ast, sem::Scope& scope)
{
    auto v = std::make_unique<sem::Variable>();
    v->name = ast.name;
    auto type = scope.getType(ast.type);
    if (type) {
        v->type = type.value().typeClass;
    }
    else {
        throw sem::SemanticException(sem::SemanticException::UNKNOWN_TYPE,
            ast.type,
            ast.pos
        );
    }
    return v;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::ArgumentDeclaration& ast, sem::Scope& scope)
{
    throw "shouldn't be called";
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::DoEndBlock& ast, sem::Scope& scope)
{
    auto body = std::make_unique<sem::DoEndBlock>();
    for (auto& statement : ast.statements) {
        
        if (ast::NewVariableStatement* nvs = dynamic_cast<ast::NewVariableStatement*>(statement.get()); nvs) {
            auto type = scope.getType(nvs->type);

            if (!type)
                throw sem::SemanticException(sem::SemanticException::UNKNOWN_TYPE, nvs->type, nvs->pos);

            auto var = std::make_unique<sem::Variable>(type.value().typeClass, nvs->name);
            body->variables.push_back(std::move(var));
            continue;
        }
        
        auto v = statement->accept(*this, scope);
        if (dynamic_cast<sem::FeatureCallExpression*>(v.get()) != nullptr) {
            body->statements.push_back(std::make_unique<sem::FeatureCallStatement>(unique_dynamic_cast<sem::FeatureCallExpression>(std::move(v))));
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
    auto fce = std::make_unique<sem::FeatureCallExpression>();
    //fce->target = unique_dynamic_cast<sem::Expression>(ast.target.accept(*this, classes));
    fce->callee = scope.getMethod(ast.name);
    return fce;
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


