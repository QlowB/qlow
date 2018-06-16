#ifndef QLOW_SEMANTIC_H
#define QLOW_SEMANTIC_H

#include <string>
#include <map>
#include "Util.h"
#include "Ast.h"
#include "Visitor.h"

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>

namespace qlow
{
    namespace sem
    {
        /*!
         * \note contains owning pointers to elements
         */
        template<typename T>
        using SymbolTable = std::map<std::string, std::unique_ptr<T>>;

        
        SymbolTable<qlow::sem::Class> createFromAst(const std::vector<std::unique_ptr<qlow::ast::Class>>& classes);

        struct SemanticObject;
        struct Class;

        struct Variable;

        struct Field;
        struct Method;

        struct DoEndBlock;
        struct Statement;
        struct Expression;

        struct FeatureCallStatement;
        struct AssignmentStatement;

        struct Operation;
        struct UnaryOperation;
        struct BinaryOperation;

        struct FeatureCallExpression;
        
        struct IntConst;
        
        struct Type;

        class SemanticException;
    }
    
    class ExpressionVisitor;
    class StatementVisitor;
    namespace gen
    {
        class FunctionGenerator;
    }
}


struct qlow::sem::Type
{
    Class* typeClass;
};


struct qlow::sem::SemanticObject
{
    virtual ~SemanticObject(void);
    
    /**
     * \brief converts the object to a readable string for debugging purposes. 
     */
    virtual std::string toString(void) const;
};


struct qlow::sem::Class : public SemanticObject
{
    qlow::ast::Class* astNode;
    std::string name;
    SymbolTable<Field> fields;
    SymbolTable<Method> methods;
    
    Class(void) = default;
    inline Class(qlow::ast::Class* astNode) :
        astNode{ astNode }, name{ astNode->name }
    {
    }
    
    virtual std::string toString(void) const override;
};


struct qlow::sem::Variable : public SemanticObject
{
    Class* type;
    std::string name;

    /// if this is a local variable, this stores a reference to the llvm
    /// instance of this variable.
    llvm::AllocaInst* allocaInst;
    
    Variable(void) = default;
    inline Variable(Class* type, std::string& name) :
        type{ type }, name{ name }, allocaInst { nullptr } {}
};


struct qlow::sem::Field : public Variable
{
    Class* type;
    std::string name;
    
    virtual std::string toString(void) const override;
};


struct qlow::sem::Method : public SemanticObject
{
    Class* containingType;
    Type returnType;
    std::string name;
    ast::MethodDefinition* astNode;
    std::unique_ptr<DoEndBlock> body;
    
    virtual std::string toString(void) const override;
};


struct qlow::sem::DoEndBlock : public SemanticObject
{
    OwningList<Variable> variables;
    OwningList<Statement> statements;
};


struct qlow::sem::Statement : public SemanticObject, public Visitable<llvm::Value*, gen::FunctionGenerator, qlow::StatementVisitor>
{
    virtual llvm::Value* accept(qlow::StatementVisitor&, gen::FunctionGenerator&) = 0;
};


struct qlow::sem::AssignmentStatement : public Statement 
{
    std::unique_ptr<Expression> target;
    std::unique_ptr<Expression> value;

    virtual llvm::Value* accept(qlow::StatementVisitor&, gen::FunctionGenerator&);
};


struct qlow::sem::Expression : public SemanticObject, public Visitable<llvm::Value*, llvm::IRBuilder<>, qlow::ExpressionVisitor>
{
};


struct qlow::sem::Operation : public Expression
{
    ast::Operation::Operator op;
};


struct qlow::sem::BinaryOperation : public Operation
{
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    
    virtual llvm::Value* accept(ExpressionVisitor& visitor, llvm::IRBuilder<>& arg2);
};


struct qlow::sem::UnaryOperation : public Operation
{
    qlow::ast::UnaryOperation::Side side;
    std::unique_ptr<Expression> arg;
    virtual llvm::Value* accept(ExpressionVisitor& visitor, llvm::IRBuilder<>& arg2);
};

struct qlow::sem::FeatureCallExpression : public Expression
{
    Method* callee;
    OwningList<Expression> arguments;
    virtual llvm::Value* accept(ExpressionVisitor& visitor, llvm::IRBuilder<>& arg2);
};


struct qlow::sem::IntConst : public Expression
{
    unsigned long long value;

    inline IntConst(unsigned long long value) :
        value{ value }
    {
    }
    
    virtual llvm::Value* accept(ExpressionVisitor& visitor, llvm::IRBuilder<>& arg2);
};


struct qlow::sem::FeatureCallStatement : public Statement 
{
    std::unique_ptr<FeatureCallExpression> expr;
    inline FeatureCallStatement(std::unique_ptr<FeatureCallExpression> expr) :
        expr{ std::move(expr) } {}

    virtual llvm::Value* accept(qlow::StatementVisitor&, gen::FunctionGenerator&);
};


class qlow::sem::SemanticException
{
    std::string message;
    qlow::CodePosition where;
public:
    enum ErrorCode
    {
        UNKNOWN_TYPE,
        DUPLICATE_CLASS_DEFINITION,
        DUPLICATE_FIELD_DECLARATION,
        DUPLICATE_METHOD_DEFINITION,
    };
    
    
    ErrorCode errorCode;
public:
    inline SemanticException(ErrorCode ec, const std::string& arg, const
        qlow::CodePosition& where) :
        message{ arg }, where{ where }, errorCode{ ec }
    {}

    std::string getMessage(void) const;
};






#endif // QLOW_SEMANTIC_H


