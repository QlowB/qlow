#include "Ast.h"
#include "AstVisitor.h"
#include "Semantic.h"

#include <cstdlib>

using namespace qlow::ast;

void Ast::merge(Ast other)
{
    std::move(other.objects.begin(), other.objects.end(), std::back_inserter(this->objects));
}


qlow::util::Path ImportDeclaration::getRelativePath(void) const
{
    if (imported.empty())
        return "";
    qlow::util::Path path = imported[0];

    for (size_t i = 1; i < imported.size(); i++) {
        path.append(imported[i]);
    }

    return path + ".qlw";
}


AstObject::~AstObject(void)
{
}


#define ACCEPT_DEFINITION(ClassName, Visitor) \
std::unique_ptr<qlow::sem::SemanticObject> ClassName::accept(Visitor& v, sem::Scope& scope) \
{ \
    return v.visit(*this, scope); \
}

ACCEPT_DEFINITION(Class, StructureVisitor)
ACCEPT_DEFINITION(FeatureDeclaration, StructureVisitor)
ACCEPT_DEFINITION(FieldDeclaration, StructureVisitor)
ACCEPT_DEFINITION(MethodDefinition, StructureVisitor)
ACCEPT_DEFINITION(VariableDeclaration, StructureVisitor)

ACCEPT_DEFINITION(Statement, StructureVisitor)
ACCEPT_DEFINITION(DoEndBlock, StructureVisitor)
ACCEPT_DEFINITION(IfElseBlock, StructureVisitor)
ACCEPT_DEFINITION(WhileBlock, StructureVisitor)
ACCEPT_DEFINITION(Expression, StructureVisitor)
ACCEPT_DEFINITION(FeatureCall, StructureVisitor)
ACCEPT_DEFINITION(AssignmentStatement, StructureVisitor)
ACCEPT_DEFINITION(ReturnStatement, StructureVisitor)
ACCEPT_DEFINITION(LocalVariableStatement, StructureVisitor)
ACCEPT_DEFINITION(AddressExpression, StructureVisitor)
ACCEPT_DEFINITION(ArrayAccessExpression, StructureVisitor)
ACCEPT_DEFINITION(IntConst, StructureVisitor)
ACCEPT_DEFINITION(StringConst, StructureVisitor)
ACCEPT_DEFINITION(UnaryOperation, StructureVisitor)
ACCEPT_DEFINITION(BinaryOperation, StructureVisitor)
ACCEPT_DEFINITION(NewExpression, StructureVisitor)
ACCEPT_DEFINITION(NewArrayExpression, StructureVisitor)
ACCEPT_DEFINITION(CastExpression, StructureVisitor)


Statement::~Statement(void)
{
}


qlow::ast::IntConst::IntConst(const std::string& val, const qlow::CodePosition& p) :
    AstObject{ p },
    Expression{ p },
    value{ strtoull(val.c_str(), nullptr, 0) }
{
}

std::unique_ptr<qlow::sem::SemanticObject> qlow::ast::Type::accept(StructureVisitor&, sem::Scope&)
{
    return nullptr;
}








