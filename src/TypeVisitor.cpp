#include "TypeVisitor.h"

using namespace qlow;



std::unique_ptr<sem::Type> sem::TypeVisitor::visit(sem::Expression& expr, const sem::SymbolTable<sem::Class>& classes)
{
}


std::unique_ptr<sem::Type> sem::TypeVisitor::visit(sem::UnaryOperation& expr, const sem::SymbolTable<sem::Class>& classes)
{
    return visit(*expr.arg, classes);
}


std::unique_ptr<sem::Type> sem::TypeVisitor::visit(sem::BinaryOperation& expr, const sem::SymbolTable<sem::Class>& classes)
{
    return visit(*expr.left, classes);
}


std::unique_ptr<sem::Type> sem::TypeVisitor::visit(sem::FeatureCallExpression& expr, const sem::SymbolTable<sem::Class>& classes)
{
    return Type{ expr.callee->returnType };
}


std::unique_ptr<sem::Type> sem::TypeVisitor::visit(sem::IntConst& expr, const sem::SymbolTable<sem::Class>& classes)
{
    return std::make_unique<NativeType>();
}





