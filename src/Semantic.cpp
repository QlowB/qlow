#include "Semantic.h"

#include "AstVisitor.h"

using namespace qlow::sem;

namespace qlow
{
    namespace sem
    {
        template<typename T, typename U>
        std::unique_ptr<T> unique_dynamic_cast(std::unique_ptr<U>&& p)
        {
            return std::unique_ptr<T> (dynamic_cast<T*>(p.release()));
        }


        SymbolTable<qlow::sem::Class>
            createFromAst(std::vector<std::unique_ptr<qlow::ast::Class>>&
                    classes)
        {
            AstVisitor av;

            // create classes
            SymbolTable<sem::Class> semClasses;
            for (auto& astClass : classes) {
                semClasses.insert(
                    {
                        astClass->name,
                        std::make_unique<sem::Class>(astClass->name)
                    }
                );
            }

            return semClasses;
        }

    }
}


SemanticObject::~SemanticObject(void)
{
}






