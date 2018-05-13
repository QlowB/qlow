#include "AstVisitor.h"
#include "Ast.h"

using namespace qlow;


sem::Class* StructureVisitor::getType(const std::string& type, const sem::SymbolTable<sem::Class>& classes)
{
    auto t = classes.find(type);
    if (t != classes.end())
        return t->second.get();
    else
        return nullptr;
}

std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::Class& ast, const sem::SymbolTable<sem::Class>& classes)
{
    auto c = std::make_unique<sem::Class>();
    c->name = ast.name;
    return c;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::FeatureDeclaration& ast, const sem::SymbolTable<sem::Class>& classes)
{
    // not needed, because 
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::FieldDeclaration& ast, const sem::SymbolTable<sem::Class>& classes)
{
    auto f = std::make_unique<sem::Field>();
    f->name = ast.name;
    f->type = getType(ast.type, classes);
    if (f->type == nullptr) {
        throw sem::SemanticException(sem::SemanticException::UNKNOWN_TYPE,
            ast.type,
            ast.pos
        );
    }
    return f;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::MethodDefinition& ast, const sem::SymbolTable<sem::Class>& classes)
{
    auto m = std::make_unique<sem::Method>();
    m->name = ast.name;
    m->returnType = getType(ast.type, classes);
    m->astNode = &ast;
    if (m->returnType == nullptr) {
        throw sem::SemanticException(sem::SemanticException::UNKNOWN_TYPE,
            ast.type,
            ast.pos
        );
    }
    return m;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::VariableDeclaration& ast, const sem::SymbolTable<sem::Class>& classes)
{
    auto v = std::make_unique<sem::Variable>();
    v->name = ast.name;
    v->type = getType(ast.type, classes);
    if (v->type == nullptr) {
        throw sem::SemanticException(sem::SemanticException::UNKNOWN_TYPE,
            ast.type,
            ast.pos
        );
    }
    return v;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::ArgumentDeclaration& ast, const sem::SymbolTable<sem::Class>& classes)
{
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::DoEndBlock& ast, const sem::SymbolTable<sem::Class>& classes)
{
    auto body = std::make_unique<sem::DoEndBlock>();
    for (auto& statement : ast.statements) {
        body->statements.push_back(unique_dynamic_cast<sem::Statement>(visit(*statement, classes)));
    }
    return body;
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::Statement& ast, const sem::SymbolTable<sem::Class>& classes)
{
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::Expression& ast, const sem::SymbolTable<sem::Class>& classes)
{
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::FeatureCall& ast, const sem::SymbolTable<sem::Class>& classes)
{
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::AssignmentStatement& ast, const sem::SymbolTable<sem::Class>& classes)
{
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::NewVariableStatement& ast, const sem::SymbolTable<sem::Class>& classes)
{
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::UnaryOperation& ast, const sem::SymbolTable<sem::Class>& classes)
{
}


std::unique_ptr<sem::SemanticObject> StructureVisitor::visit(ast::BinaryOperation& ast, const sem::SymbolTable<sem::Class>& classes)
{
}


