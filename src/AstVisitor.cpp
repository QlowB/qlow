#include "AstVisitor.h"


using namespace qlow;

std::unique_ptr<sem::SemanticObject> AstVisitor::visit(ast::Class& ast)
{
    auto c = std::make_unique<sem::Class> ();
    c->name = ast.name;

    return c;
}


