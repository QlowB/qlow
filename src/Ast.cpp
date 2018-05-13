#include "Ast.h"
#include "AstVisitor.h"
#include "Semantic.h"

using namespace qlow::ast;


AstObject::~AstObject(void)
{
}


#define ACCEPT_DEFINITION(ClassName, Visitor) \
std::unique_ptr<qlow::sem::SemanticObject> ClassName::accept(Visitor& v, const sem::SymbolTable<sem::Class>& c) \
{ \
    v.visit(*this, c); \
}

ACCEPT_DEFINITION(Class, StructureVisitor)
ACCEPT_DEFINITION(FeatureDeclaration, StructureVisitor)
ACCEPT_DEFINITION(FieldDeclaration, StructureVisitor)
ACCEPT_DEFINITION(MethodDefinition, StructureVisitor)
ACCEPT_DEFINITION(VariableDeclaration, StructureVisitor)
ACCEPT_DEFINITION(ArgumentDeclaration, StructureVisitor)


ACCEPT_DEFINITION(DoEndBlock, BodyVisitor)
ACCEPT_DEFINITION(Statement, BodyVisitor)
ACCEPT_DEFINITION(Expression, BodyVisitor)
ACCEPT_DEFINITION(FeatureCall, BodyVisitor)
ACCEPT_DEFINITION(AssignmentStatement, BodyVisitor)
ACCEPT_DEFINITION(NewVariableStatement, BodyVisitor)
ACCEPT_DEFINITION(UnaryOperation, BodyVisitor)
ACCEPT_DEFINITION(BinaryOperation, BodyVisitor)



Statement::~Statement(void)
{
}







