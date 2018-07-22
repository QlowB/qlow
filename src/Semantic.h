#ifndef QLOW_SEMANTIC_H
#define QLOW_SEMANTIC_H

#include <string>
#include <map>
#include "Util.h"
#include "Ast.h"
#include "Visitor.h"
#include "Scope.h"

#include "Type.h"

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>

namespace qlow
{
    namespace sem
    {
        std::unique_ptr<GlobalScope>
            createFromAst(const std::vector<std::unique_ptr<qlow::ast::AstObject>>& objects);

        struct Class;

        struct Variable;

        struct Field;
        struct Method;

        struct Statement;
        struct Expression;
        
        struct ThisExpression;

        struct DoEndBlock;
        struct IfElseBlock;
        struct WhileBlock;
        struct FeatureCallStatement;
        struct AssignmentStatement;
        struct ReturnStatement;
        
        struct LocalVariableExpression;

        struct Operation;
        struct UnaryOperation;
        struct BinaryOperation;
        
        struct CastExpression;
        
        struct NewArrayExpression;

        struct MethodCallExpression;
        struct FieldAccessExpression;
        
        struct IntConst;
    }

    class ExpressionCodegenVisitor;
    class LValueVisitor;
    class StatementVisitor;

    namespace gen
    {
        class FunctionGenerator;
    }
}


struct qlow::sem::Class : public SemanticObject
{
    qlow::ast::Class* astNode;
    std::string name;
    SymbolTable<Field> fields;
    SymbolTable<Method> methods;
    ClassScope scope;

    /// \brief generated during llvm code generation, not availab
    llvm::Type* llvmType;

    inline Class(qlow::ast::Class* astNode, GlobalScope& globalScope) :
        astNode{ astNode },
        name{ astNode->name },
        scope{ globalScope, this },
        llvmType{ nullptr }
    {
    }

    inline Class(const std::string& nativeName, GlobalScope& globalScope) :
        astNode{ nullptr },
        name{ nativeName },
        scope{ globalScope, this },
        llvmType{ nullptr }
    {
    }

    virtual std::string toString(void) const override;
};


struct qlow::sem::Variable : public SemanticObject
{
    std::shared_ptr<Type> type;
    std::string name;
    bool isParameter;

    /// if this is a local variable, this stores a reference to the llvm
    /// instance of this variable. If it is a parameter, the parameter value
    llvm::Value* allocaInst;
    
    Variable(void) = default;
    inline Variable(std::shared_ptr<Type> type, const std::string& name) :
        type{ std::move(type) },
        name{ name },
        allocaInst { nullptr }
    {
    }
        
    virtual std::string toString(void) const override;
};


struct qlow::sem::Field : public Variable
{
    int llvmStructIndex;
    virtual std::string toString(void) const override;
};


struct qlow::sem::Method : public SemanticObject
{
    Class* containingType;
    std::shared_ptr<Type> returnType;
    std::vector<Variable*> arguments;
    std::string name;
    ast::MethodDefinition* astNode;
    ThisExpression* thisExpression;
    std::unique_ptr<DoEndBlock> body;

    LocalScope scope;

    llvm::Function* llvmNode;

    inline Method(Scope& parentScope, std::shared_ptr<Type> returnType) :
        containingType{ nullptr },
        returnType{ std::move(returnType) },
        scope{ parentScope, this },
        thisExpression{ nullptr },
        body{ nullptr }
    {
    }
    
    inline Method(ast::MethodDefinition* astNode, Scope& parentScope) :
        containingType{ nullptr },
        astNode{ astNode },
        name{ astNode->name },
        scope{ parentScope, this },
        thisExpression{ nullptr },
        body{ nullptr }
    {
    }
    
    void generateThisExpression(void);
    
    virtual std::string toString(void) const override;
};


struct qlow::sem::ThisExpression : public Variable
{
    Method* method;
    inline ThisExpression(Method* method) :
        Variable{ std::make_shared<PointerType>(std::make_shared<ClassType>(method->containingType)), "this" },
        method{ method }
    {
    }
    
    llvm::Value* accept(ExpressionCodegenVisitor& visitor, llvm::IRBuilder<>& arg2);
};


struct qlow::sem::Statement : public SemanticObject, public Visitable<llvm::Value*, gen::FunctionGenerator, qlow::StatementVisitor>
{
    virtual llvm::Value* accept(qlow::StatementVisitor&, gen::FunctionGenerator&) = 0;
};


struct qlow::sem::DoEndBlock : public Statement
{
    LocalScope scope;
    OwningList<Statement> statements;

    inline DoEndBlock(LocalScope& parentScope) :
        scope{ parentScope } {}
    
    virtual llvm::Value* accept(qlow::StatementVisitor&, gen::FunctionGenerator&) override;
};


struct qlow::sem::IfElseBlock : public Statement
{
    std::unique_ptr<Expression> condition;
    std::unique_ptr<DoEndBlock> ifBlock;
    std::unique_ptr<DoEndBlock> elseBlock;
    inline IfElseBlock(std::unique_ptr<Expression> condition,
                       std::unique_ptr<DoEndBlock> ifBlock,
                       std::unique_ptr<DoEndBlock> elseBlock) :
        condition{ std::move(condition) },
        ifBlock{ std::move(ifBlock) },
        elseBlock{ std::move(elseBlock) }
    {
    }
    
    virtual llvm::Value* accept(qlow::StatementVisitor&, gen::FunctionGenerator&) override;
};


struct qlow::sem::WhileBlock : public Statement
{
    std::unique_ptr<Expression> condition;
    std::unique_ptr<DoEndBlock> body;
    inline WhileBlock(std::unique_ptr<Expression> condition,
                       std::unique_ptr<DoEndBlock> body) :
        condition{ std::move(condition) },
        body{ std::move(body) }
    {
    }
    
    virtual llvm::Value* accept(qlow::StatementVisitor&, gen::FunctionGenerator&) override;
};


struct qlow::sem::AssignmentStatement : public Statement 
{
    std::unique_ptr<Expression> target;
    std::unique_ptr<Expression> value;

    virtual std::string toString(void) const override;
    virtual llvm::Value* accept(qlow::StatementVisitor&, gen::FunctionGenerator&) override;
};


struct qlow::sem::ReturnStatement : public Statement 
{
    std::unique_ptr<Expression> value;

    virtual std::string toString(void) const override;
    virtual llvm::Value* accept(qlow::StatementVisitor&, gen::FunctionGenerator&) override;
};


struct qlow::sem::Expression :
    public SemanticObject,
    public Visitable<llvm::Value*,
                     llvm::IRBuilder<>,
                     qlow::ExpressionCodegenVisitor>,
    public Visitable<llvm::Value*,
                     llvm::IRBuilder<>,
                     qlow::LValueVisitor>
{
    std::shared_ptr<sem::Type> type;
    
    inline Expression(std::shared_ptr<Type> type) :
        type{ std::move(type) }
    {
    }
    
    virtual llvm::Value* accept(ExpressionCodegenVisitor& visitor, llvm::IRBuilder<>& arg2) override = 0;
    virtual llvm::Value* accept(LValueVisitor& visitor, llvm::IRBuilder<>& arg2) override;
};


struct qlow::sem::Operation : public Expression
{
    std::string opString;
    
    inline Operation(std::shared_ptr<Type> type) :
        Expression{ std::move(type) }
    {
    }
};


struct qlow::sem::LocalVariableExpression : public Expression
{
    Variable* var;
    
    inline LocalVariableExpression(Variable* var) :
        Expression{ var->type },
        var{ var }
    {
    }

    virtual llvm::Value* accept(ExpressionCodegenVisitor& visitor, llvm::IRBuilder<>& arg2) override;
    virtual llvm::Value* accept(LValueVisitor& visitor, llvm::IRBuilder<>& arg2);
    virtual std::string toString(void) const override;
};


struct qlow::sem::BinaryOperation : public Operation
{
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    ast::BinaryOperation* astNode;
    
    /// method that is called to execute the operator
    sem::Method* operationMethod;
    
    inline BinaryOperation(std::shared_ptr<Type> type, ast::BinaryOperation* astNode) :
        Operation{ std::move(type) },
        astNode{ astNode }
    {
    }
    
    virtual llvm::Value* accept(ExpressionCodegenVisitor& visitor, llvm::IRBuilder<>& arg2) override;
    
    virtual std::string toString(void) const override;
};


struct qlow::sem::CastExpression : public Expression
{
    std::unique_ptr<Expression> expression;
    std::shared_ptr<Type> targetType;
    
    ast::CastExpression* astNode;
    
    inline CastExpression(std::unique_ptr<Expression> expression,
                          std::shared_ptr<Type> type,
                          ast::CastExpression* astNode) :
        Expression{ type },
        expression{ std::move(expression) },
        targetType{ std::move(type) },
        astNode{ astNode }
    {
    }
    
    virtual llvm::Value* accept(ExpressionCodegenVisitor& visitor, llvm::IRBuilder<>& arg2) override;
    
    virtual std::string toString(void) const override;
};


struct qlow::sem::NewArrayExpression : public Expression
{
    std::shared_ptr<Type> arrayType;
    std::unique_ptr<Expression> length;
    
    inline NewArrayExpression(std::shared_ptr<Type> arrayType) :
        Expression{ std::make_shared<ArrayType>(arrayType) },
        arrayType{ std::move(arrayType) }
    {
    }
    
    virtual llvm::Value* accept(ExpressionCodegenVisitor& visitor, llvm::IRBuilder<>& arg2) override;
    virtual std::string toString(void) const override;
};


struct qlow::sem::UnaryOperation : public Operation
{
    qlow::ast::UnaryOperation::Side side;
    std::unique_ptr<Expression> arg;
    
    inline UnaryOperation(std::shared_ptr<Type> type) :
        Operation{ std::move(type) }
    {
    }
    
    virtual llvm::Value* accept(ExpressionCodegenVisitor& visitor, llvm::IRBuilder<>& arg2) override;
    virtual std::string toString(void) const override;
};


struct qlow::sem::MethodCallExpression : public Expression
{
    Method* callee;
    std::unique_ptr<Expression> target;
    OwningList<Expression> arguments;
    
    inline MethodCallExpression(std::unique_ptr<Expression> target,
                                 Method* callee) :
        Expression{ callee->returnType },
        target{ std::move(target) },
        callee{ callee }
    {
    }
    
    virtual llvm::Value* accept(ExpressionCodegenVisitor& visitor, llvm::IRBuilder<>& arg2) override;
    
    virtual std::string toString(void) const override;
};


struct qlow::sem::FieldAccessExpression : public Expression
{
    sem::Field* accessed;
    std::unique_ptr<Expression> target;
    //OwningList<Expression> arguments;
    
    inline FieldAccessExpression(std::unique_ptr<Expression> target,
                                 Field* accessed ) :
        Expression{ accessed->type },
        target{ std::move(target) },
        accessed{ accessed }
    {
    }
    
    virtual llvm::Value* accept(ExpressionCodegenVisitor& visitor, llvm::IRBuilder<>& arg2) override;
    virtual llvm::Value* accept(LValueVisitor& visitor, llvm::IRBuilder<>& arg2) override;
    
    virtual std::string toString(void) const override;
};


struct qlow::sem::IntConst : public Expression
{
    unsigned long long value;

    inline IntConst(unsigned long long value) :
        Expression{ std::make_shared<NativeType>(NativeType::INTEGER) },
        value{ value }
    {
    }
    
    virtual llvm::Value* accept(ExpressionCodegenVisitor& visitor, llvm::IRBuilder<>& arg2) override;
};


struct qlow::sem::FeatureCallStatement : public Statement 
{
    std::unique_ptr<MethodCallExpression> expr;
    inline FeatureCallStatement(std::unique_ptr<MethodCallExpression> expr) :
        expr{ std::move(expr) } {}

    virtual std::string toString(void) const override;
    virtual llvm::Value* accept(qlow::StatementVisitor&, gen::FunctionGenerator&) override;
};






#endif // QLOW_SEMANTIC_H


