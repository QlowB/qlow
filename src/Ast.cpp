#include "Ast.h"
#include "AstVisitor.h"
#include "Semantic.h"

#include <cstdlib>

using namespace qlow::ast;


AstObject::~AstObject(void)
{
}


#define ACCEPT_DEFINITION(ClassName, Visitor) \
std::unique_ptr<qlow::sem::SemanticObject> ClassName::accept(Visitor& v, sem::Scope& scope) \
{ \
    return v.visit(*this, scope); \
}

ACCEPT_DEFINITION(Class, StructureVisitor)
ACCEPT_DEFINITION(ClassType, StructureVisitor)
ACCEPT_DEFINITION(ArrayType, StructureVisitor)
ACCEPT_DEFINITION(FeatureDeclaration, StructureVisitor)
ACCEPT_DEFINITION(FieldDeclaration, StructureVisitor)
ACCEPT_DEFINITION(MethodDefinition, StructureVisitor)
ACCEPT_DEFINITION(VariableDeclaration, StructureVisitor)

ACCEPT_DEFINITION(Statement, StructureVisitor)
ACCEPT_DEFINITION(DoEndBlock, StructureVisitor)
ACCEPT_DEFINITION(IfElseBlock, StructureVisitor)
ACCEPT_DEFINITION(Expression, StructureVisitor)
ACCEPT_DEFINITION(FeatureCall, StructureVisitor)
ACCEPT_DEFINITION(AssignmentStatement, StructureVisitor)
ACCEPT_DEFINITION(ReturnStatement, StructureVisitor)
ACCEPT_DEFINITION(NewVariableStatement, StructureVisitor)
ACCEPT_DEFINITION(IntConst, StructureVisitor)
ACCEPT_DEFINITION(UnaryOperation, StructureVisitor)
ACCEPT_DEFINITION(BinaryOperation, StructureVisitor)



Statement::~Statement(void)
{
}


qlow::ast::IntConst::IntConst(std::string&& val, const qlow::CodePosition& p) :
    AstObject{ p },
    Expression{ p },
    value{ strtoull(val.c_str(), nullptr, 0) }
{
}








