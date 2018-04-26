#include "AstVisitor.h"
#include "Ast.h"

using namespace qlow;

std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::Class& ast)
{
    auto c = std::make_unique<sem::Class> ();
    c->name = ast.name;

    return c;
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::FeatureDeclaration& ast)
{
    // not needed, because 
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::FieldDeclaration& ast)
{
    auto f = std::make_unique<sem::Field>();
    f->name = ast.name;
    return f;
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::MethodDefinition& ast)
{
    auto m = std::make_unique<sem::Method>();
    m->name = ast.name;
    return m;
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::VariableDeclaration& ast)
{
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::ArgumentDeclaration& ast)
{
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::DoEndBlock& ast)
{
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::Statement& ast)
{
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::Expression& ast)
{
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::FeatureCall& ast)
{
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::AssignmentStatement& ast)
{
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::NewVariableStatement& ast)
{
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::UnaryOperation& ast)
{
}


std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::BinaryOperation& ast)
{
}


