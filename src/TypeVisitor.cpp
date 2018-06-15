#include "TypeVisitor.h"

using namespace qlow;



sem::Type sem::TypeVisitor::visit(sem::Expression& expr, const sem::SymbolTable<sem::Class>& classes)
{
}


sem::Type sem::TypeVisitor::visit(sem::UnaryOperation& expr, const sem::SymbolTable<sem::Class>& classes)
{
    return visit(*expr.arg, classes);
}


sem::Type sem::TypeVisitor::visit(sem::BinaryOperation& expr, const sem::SymbolTable<sem::Class>& classes)
{
    return visit(*expr.left, classes);
}


sem::Type sem::TypeVisitor::visit(sem::FeatureCallExpression& expr, const sem::SymbolTable<sem::Class>& classes)
{
    return Type{ expr.callee->returnType };
}


sem::Type sem::TypeVisitor::visit(sem::IntConst& expr, const sem::SymbolTable<sem::Class>& classes)
{
    return Type{ new sem::Class() };
}





