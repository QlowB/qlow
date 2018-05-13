#include "Ast.h"
#include "AstVisitor.h"
#include "Semantic.h"

#include <cstdlib>

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

ACCEPT_DEFINITION(DoEndBlock, StructureVisitor)
ACCEPT_DEFINITION(Statement, StructureVisitor)
ACCEPT_DEFINITION(Expression, StructureVisitor)
ACCEPT_DEFINITION(FeatureCall, StructureVisitor)
ACCEPT_DEFINITION(AssignmentStatement, StructureVisitor)
ACCEPT_DEFINITION(NewVariableStatement, StructureVisitor)
ACCEPT_DEFINITION(UnaryOperation, StructureVisitor)
ACCEPT_DEFINITION(BinaryOperation, StructureVisitor)



Statement::~Statement(void)
{
}


qlow::ast::IntConst::IntConst(const std::string& val, const qlow::CodePosition& p) :
    AstObject{ p },
    Expression{ p },
    value{ strtoull(val.c_str(), nullptr, 0) }
{
}








