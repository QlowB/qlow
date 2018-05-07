#include "AstVisitor.h"
#include "Ast.h"

using namespace qlow;

std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::Class& ast, const sem::SymbolTable<sem::Class>& classes)
{
    auto c = std::make_unique<sem::Class> ();
    c->name = ast.name;

    return c;
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::FeatureDeclaration& ast, const sem::SymbolTable<sem::Class>& classes)
{
    // not needed, because 
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::FieldDeclaration& ast, const sem::SymbolTable<sem::Class>& classes)
{
    auto f = std::make_unique<sem::Field>();
    f->name = ast.name;
    return f;
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::MethodDefinition& ast, const sem::SymbolTable<sem::Class>& classes)
{
    auto m = std::make_unique<sem::Method>();
    m->name = ast.name;
    return m;
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::VariableDeclaration& ast, const sem::SymbolTable<sem::Class>& classes)
{
    
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::ArgumentDeclaration& ast, const sem::SymbolTable<sem::Class>& classes)
{
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::DoEndBlock& ast, const sem::SymbolTable<sem::Class>& classes)
{
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::Statement& ast, const sem::SymbolTable<sem::Class>& classes)
{
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::Expression& ast, const sem::SymbolTable<sem::Class>& classes)
{
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::FeatureCall& ast, const sem::SymbolTable<sem::Class>& classes)
{
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::AssignmentStatement& ast, const sem::SymbolTable<sem::Class>& classes)
{
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::NewVariableStatement& ast, const sem::SymbolTable<sem::Class>& classes)
{
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::UnaryOperation& ast, const sem::SymbolTable<sem::Class>& classes)
{
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::BinaryOperation& ast, const sem::SymbolTable<sem::Class>& classes)
{
}


