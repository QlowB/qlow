#ifndef QLOW_SEMANTIC_H
#define QLOW_SEMANTIC_H

#include <string>
#include <map>
#include "Util.h"
#include "Ast.h"

namespace qlow
{
    namespace sem
    {
        
        SymbolTable<qlow::sem::Class> createFromAst(std::vector<std::unique_ptr<qlow::ast::Class>>& classes);
        
        /*!
         * \note contains owning pointers to elements
         */
        template<typename T>
        using SymbolTable = std::map<std::string, std::unique_ptr<T>>;

        struct SemanticObject;
        struct Class;

        struct Field;
        struct Method;

        struct Variable;

        struct DoEndBlock;
        struct Statement;
        struct Expression;

        struct FeatureCallStatement;
        struct AssignmentStatement;

        struct Operation;
        struct UnaryOperation;
        struct BinaryOperation;

        struct FeatureCallExpression;
        
        struct Type;

        class SemanticException;
    }
}


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


struct qlow::sem::Field : public SemanticObject
{
    Class* type;
    std::string name;
    
    virtual std::string toString(void) const override;
};


struct qlow::sem::Method : public SemanticObject
{
    Class* returnType;
    std::string name;
    ast::MethodDefinition* astNode;
    std::unique_ptr<DoEndBlock> body;
    
    virtual std::string toString(void) const override;
};


struct qlow::sem::Variable : public SemanticObject
{
    Class* type;
    std::string name;
    
    Variable(void) = default;
    inline Variable(Class* type, std::string& name) :
        type{ type }, name{ name } {}
};


struct qlow::sem::DoEndBlock : public SemanticObject
{
    OwningList<Variable> variables;
    OwningList<Statement> statements;
};


struct qlow::sem::Statement : public SemanticObject
{
};


struct qlow::sem::AssignmentStatement : public Statement 
{
    std::unique_ptr<Expression> target;
    std::unique_ptr<Expression> value;
};


struct qlow::sem::Expression : public SemanticObject
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
};


struct qlow::sem::UnaryOperation : public Operation
{
    qlow::ast::UnaryOperation::Side side;
    std::unique_ptr<Expression> arg;
};

struct qlow::sem::FeatureCallExpression : public Expression
{
    Method* callee;
    OwningList<Expression> arguments;
};


struct qlow::sem::FeatureCallStatement : public Statement 
{
    std::unique_ptr<FeatureCallExpression> expr;
    inline FeatureCallStatement(std::unique_ptr<FeatureCallExpression> expr) :
        expr{ std::move(expr) } {}
};


struct qlow::sem::Type
{
    Class* typeClass;
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


