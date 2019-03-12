#ifndef QLOW_SEMANTIC_H
#define QLOW_SEMANTIC_H

#include <string>
#include <map>
#include "Util.h"
#include "Ast.h"
#include "Visitor.h"
#include "Scope.h"

#include "Type.h"
#include "Context.h"

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>

namespace qlow
{
    namespace sem
    {
        std::unique_ptr<GlobalScope>
            createFromAst(const qlow::ast::Ast& ast);

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
        struct AddressExpression;

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
    TypeId classType;

    /// \brief generated during llvm code generation, not availab
    llvm::Type* llvmType;

    inline Class(Context& context, qlow::ast::Class* astNode,
            GlobalScope& globalScope) :
        SemanticObject{ context },
        astNode{ astNode },
        name{ astNode->name },
        scope{ globalScope, this },
        classType{ context.addType(Type::createClassType(context, this)) },
        llvmType{ nullptr }
    {
    }

    inline Class(Context& context, const std::string& nativeName,
            GlobalScope& globalScope) :
        SemanticObject{ context },
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
    TypeId type;
    std::string name;
    bool isParameter;

    /// if this is a local variable, this stores a reference to the llvm
    /// instance of this variable. If it is a parameter, the parameter value
    llvm::Value* allocaInst;
    
    Variable(void) = default;
    inline Variable(Context& context, TypeId type, const std::string& name) :
        SemanticObject{ context },
        type{ type },
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
    Class* containingClass;
    TypeId returnType;
    std::vector<Variable*> arguments;
    std::string name;
    ast::MethodDefinition* astNode;
    ThisExpression* thisExpression;
    std::unique_ptr<DoEndBlock> body;

    LocalScope scope;

    llvm::Function* llvmNode;

    inline Method(Context& context, Scope& parentScope,
            TypeId returnType) :
        SemanticObject{ context },
        containingClass{ nullptr },
        returnType{ returnType },
        thisExpression{ nullptr },
        body{ nullptr },
        scope{ parentScope, this }
    {
    }
    
    /*
    inline Method(ast::MethodDefinition* astNode, Scope& parentScope) :
        containingType{ nullptr },
        returnType{ parentScope.getReturnableType()},
        astNode{ astNode },
        name{ astNode->name },
        scope{ parentScope, this },
        thisExpression{ nullptr },
        body{ nullptr }
    {
    }*/
    
    void generateThisExpression(void);
    
    virtual std::string toString(void) const override;
};


struct qlow::sem::ThisExpression : public Variable
{
    Method* method;
    inline ThisExpression(Method* method) :
        Variable{
            method->context,
            method->context.addType(Type::createPointerType(method->context,
                        method->context.addType(Type::createClassType(method->context,
                            method->containingClass)))),
            "this"
        },
        method{ method }
    {
    }
    
    llvm::Value* accept(ExpressionCodegenVisitor& visitor, llvm::IRBuilder<>& arg2);
};


struct qlow::sem::Statement : public SemanticObject, public Visitable<llvm::Value*, gen::FunctionGenerator, qlow::StatementVisitor>
{
    inline Statement(Context& context) :
        SemanticObject{ context } {}
    virtual llvm::Value* accept(qlow::StatementVisitor&, gen::FunctionGenerator&) = 0;
};


struct qlow::sem::DoEndBlock : public Statement
{
    LocalScope scope;
    OwningList<Statement> statements;

    inline DoEndBlock(Context& context, LocalScope& parentScope) :
        Statement{ context },
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
        Statement{ ifBlock->context },
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
        Statement{ body->context },
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

    inline AssignmentStatement(Context& context) :
        Statement{ context } {}

    virtual std::string toString(void) const override;
    virtual llvm::Value* accept(qlow::StatementVisitor&, gen::FunctionGenerator&) override;
};


struct qlow::sem::ReturnStatement : public Statement 
{
    std::unique_ptr<Expression> value;

    inline ReturnStatement(Context& context) :
        Statement{ context } {}

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
    TypeId type;
    
    inline Expression(Context& context, TypeId type) :
        SemanticObject{ context },
        type{ type }
    {
    }
    
    inline virtual bool isLValue(void) const { return false; }
    
    virtual llvm::Value* accept(ExpressionCodegenVisitor& visitor, llvm::IRBuilder<>& arg2) override = 0;
    virtual llvm::Value* accept(LValueVisitor& visitor, llvm::IRBuilder<>& arg2) override;
};


struct qlow::sem::Operation : public Expression
{
    std::string opString;
    
    inline Operation(Context& context, TypeId type) :
        Expression{ context, type }
    {
    }
};


struct qlow::sem::LocalVariableExpression : public Expression
{
    Variable* var;
    
    inline LocalVariableExpression(Variable* var) :
        Expression{ var->context, var->type },
        var{ var }
    {
    }
    
    inline virtual bool isLValue(void) const override { return true; }

    virtual llvm::Value* accept(ExpressionCodegenVisitor& visitor, llvm::IRBuilder<>& arg2) override;
    virtual llvm::Value* accept(LValueVisitor& visitor, llvm::IRBuilder<>& arg2);
    virtual std::string toString(void) const override;
};


struct qlow::sem::AddressExpression : public Expression
{
    std::unique_ptr<sem::Expression> target;
    
    inline AddressExpression(std::unique_ptr<sem::Expression> target) :
        Expression{ target->context, context.getPointerTo(target->type) },
        target{ std::move(target) }
    {
    }
    
    virtual llvm::Value* accept(ExpressionCodegenVisitor& visitor, llvm::IRBuilder<>& arg2) override;
};


struct qlow::sem::BinaryOperation : public Operation
{
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    ast::BinaryOperation* astNode;
    
    /// method that is called to execute the operator
    sem::Method* operationMethod;
    
    inline BinaryOperation(Context& context,
            TypeId type, ast::BinaryOperation* astNode) :
        Operation{ context, type },
        astNode{ astNode }
    {
    }
    
    virtual llvm::Value* accept(ExpressionCodegenVisitor& visitor, llvm::IRBuilder<>& arg2) override;
    
    virtual std::string toString(void) const override;
};


struct qlow::sem::CastExpression : public Expression
{
    std::unique_ptr<Expression> expression;
    TypeId targetType;
    
    ast::CastExpression* astNode;
    
    inline CastExpression(std::unique_ptr<Expression> expression,
                          TypeId type,
                          ast::CastExpression* astNode) :
        Expression{ expression->context, type },
        expression{ std::move(expression) },
        targetType{ type },
        astNode{ astNode }
    {
    }
    
    virtual llvm::Value* accept(ExpressionCodegenVisitor& visitor, llvm::IRBuilder<>& arg2) override;
    
    virtual std::string toString(void) const override;
};


struct qlow::sem::NewArrayExpression : public Expression
{
    TypeId arrayType;
    std::unique_ptr<Expression> length;
    
    inline NewArrayExpression(Context& context, TypeId arrayType) :
        Expression{ context, context.getArrayOf(arrayType) },
        arrayType{ arrayType }
    {
    }
    
    virtual llvm::Value* accept(ExpressionCodegenVisitor& visitor, llvm::IRBuilder<>& arg2) override;
    virtual std::string toString(void) const override;
};


struct qlow::sem::UnaryOperation : public Operation
{
    qlow::ast::UnaryOperation::Side side;
    std::unique_ptr<Expression> arg;
    
    inline UnaryOperation(Context& context, TypeId type) :
        Operation{ context, type }
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
        Expression{ target->context, callee->returnType },
        callee{ callee },
        target{ std::move(target) }
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
        Expression{ target->context, accessed->type },
        accessed{ accessed },
        target{ std::move(target) }
    {
    }
    
    inline virtual bool isLValue(void) const override { return true; }
    
    virtual llvm::Value* accept(ExpressionCodegenVisitor& visitor, llvm::IRBuilder<>& arg2) override;
    virtual llvm::Value* accept(LValueVisitor& visitor, llvm::IRBuilder<>& arg2) override;
    
    virtual std::string toString(void) const override;
};


struct qlow::sem::IntConst : public Expression
{
    unsigned long long value;

    inline IntConst(Context& context, unsigned long long value) :
        Expression{ context.addType(/* integer */) },
        value{ value }
    {
    }
    
    virtual llvm::Value* accept(ExpressionCodegenVisitor& visitor, llvm::IRBuilder<>& arg2) override;
};


struct qlow::sem::FeatureCallStatement : public Statement 
{
    std::unique_ptr<MethodCallExpression> expr;
    inline FeatureCallStatement(std::unique_ptr<MethodCallExpression> expr) :
        Statement{ expr->context },
        expr{ std::move(expr) } {}

    virtual std::string toString(void) const override;
    virtual llvm::Value* accept(qlow::StatementVisitor&, gen::FunctionGenerator&) override;
};






#endif // QLOW_SEMANTIC_H


