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
         * \brief contains owning pointers to elements
         */
        template<typename T>
        using SymbolTable = std::map<std::string, std::unique_ptr<T>>;

        struct SemanticObject;
        struct Class;

        struct Field;
        struct Method;

        SymbolTable<qlow::sem::Class> createFromAst(std::vector<std::unique_ptr<qlow::ast::Class>>& classes);
    }
}


struct qlow::sem::SemanticObject
{
    virtual ~SemanticObject(void);
};


struct qlow::sem::Class : public SemanticObject
{
    std::string name;
    SymbolTable<Field> fields;
    SymbolTable<Method> methods;

    inline Class(const std::string& name) :
        name{ name }
    {
    }
};


struct qlow::sem::Field : public SemanticObject
{
    Class* type;
    std::string name;
};


struct qlow::sem::Method : public SemanticObject
{
    Class* returnType;
    std::string name;
};








#endif // QLOW_SEMANTIC_H


