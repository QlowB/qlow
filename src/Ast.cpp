#include "Ast.h"
#include "AstVisitor.h"
#include "Semantic.h"

using namespace qlow::ast;


AstObject::~AstObject(void)
{
}


#define ACCEPT_DEFINITION(ClassName) \
std::unique_ptr<qlow::sem::SemanticObject> ClassName::accept(AstVisitor& v) \
{ \
    v.visit(*this); \
}

ACCEPT_DEFINITION(Class)
ACCEPT_DEFINITION(FeatureDeclaration)
ACCEPT_DEFINITION(FieldDeclaration)
ACCEPT_DEFINITION(MethodDefinition)
ACCEPT_DEFINITION(VariableDeclaration)
ACCEPT_DEFINITION(ArgumentDeclaration)
ACCEPT_DEFINITION(DoEndBlock)
ACCEPT_DEFINITION(Statement)
ACCEPT_DEFINITION(Expression)
ACCEPT_DEFINITION(FeatureCall)
ACCEPT_DEFINITION(AssignmentStatement)
ACCEPT_DEFINITION(NewVariableStatement)
ACCEPT_DEFINITION(UnaryOperation)
ACCEPT_DEFINITION(BinaryOperation)



Statement::~Statement(void)
{
}







