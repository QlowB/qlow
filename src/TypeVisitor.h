#ifndef QLOW_SEM_TYPEVISITOR_H
#define QLOW_SEM_TYPEVISITOR_H

#include "Visitor.h"
#include "Semantic.h"

namespace qlow
{
    namespace sem
    {
        class TypeVisitor;
    }
}


class qlow::sem::TypeVisitor :
    public Visitor<
        qlow::sem::Type*,
        const sem::SymbolTable<sem::Class>,

        sem::Expression,
        sem::UnaryOperation,
        sem::BinaryOperation,
        sem::FeatureCallExpression,
        sem::IntConst
    >
{

public:
    qlow::sem::Type* visit(sem::Expression& expr, const sem::SymbolTable<sem::Class>& classes) override;
    qlow::sem::Type* visit(sem::UnaryOperation& expr, const sem::SymbolTable<sem::Class>& classes) override;
    qlow::sem::Type* visit(sem::BinaryOperation& expr, const sem::SymbolTable<sem::Class>& classes) override;
    qlow::sem::Type* visit(sem::FeatureCallExpression& expr, const sem::SymbolTable<sem::Class>& classes) override;
    qlow::sem::Type* visit(sem::IntConst& expr, const sem::SymbolTable<sem::Class>& classes) override;
};

#endif // QLOW_SEM_TYPEVISITOR_H
