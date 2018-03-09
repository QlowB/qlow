// =============================================================================
//
// This file is part of the qlow compiler.
//
// Copyright (C) 2014-2015 Nicolas Winkler
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// =============================================================================

#include <string>
#include <vector>
#include <memory>


namespace qlow
{
    namespace ast
    {
        template<typename T>
        using List = std::vector<std::unique_ptr<T>>;


        struct Class;

        struct FeatureDeclaration;

        struct FieldDeclaration;
        struct MethodDefinition;

        struct VariableDeclaration;

        struct Statement;

        struct Expression;
        struct Identifier;
        struct BinaryOperation;
        struct FunctionCall;
    }
}



struct qlow::ast::Class
{
    std::string name;
};


struct qlow::ast::FeatureDeclaration
{
    std::string name;
    std::string type;
};


struct qlow::ast::FieldDeclaration
{
    inline MethodDefinition(const std::string& type, const std::string& name) :
        FeatureDeclaration(type, name)
    {
    }
};


struct qlow::ast::MethodDefinition
{
    List<ArgumentDeclaration> arguments;
    std::unique_ptr<DoEndBlock> body;

    inline MethodDefinition(const std::string& type, const std::string& name,
            std::unique_ptr<DoEndBlock>&& body) :
        FeatureDeclaration(type, name),
        body(body)
    {
    }
};


struct qlow::ast::VariableDeclaration
{
    std::string name;
    std::string type;
};


struct qlow::ast::Statement
{
};


struct qlow::ast::Expression
{
};


struct qlow::ast::BinaryOperation : public Expression
{
    std::unique_ptr<Expression> left;
    std::string operator_str;
    std::unique_ptr<Expression> right;
};


struct qlow::ast::FunctionCall : public Expression
{
    std::string name;
    std::vector<std::unique_ptr<Expression>> arguments;
};






