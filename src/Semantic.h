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
        struct Variable;

        SymbolTable<qlow::sem::Class> createFromAst(std::vector<std::unique_ptr<qlow::ast::Class>>& classes);
        
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
    
    virtual std::string toString(void) const override;
};


struct qlow::sem::Variable : public SemanticObject
{
    Class* type;
    std::string name;
};



class qlow::sem::SemanticException
{
    std::string message;
public:
    enum ErrorCode
    {
        UNKNOWN_TYPE,
    };
    
    
    ErrorCode errorCode;
public:
    inline SemanticException(ErrorCode ec, const std::string& arg) :
        message{ arg }, errorCode{ ec }
    {}

    std::string getMessage(void) const;
};






#endif // QLOW_SEMANTIC_H


