#ifndef QLOW_CODE_GENERATION_H
#define QLOW_CODE_GENERATION_H

#include "Semantic.h"

namespace qlow
{
    namespace gen
    {
        void generateObjectFile(const std::string& name, const sem::SymbolTable<sem::Class>& classes);
    }
}


#endif // QLOW_CODE_GENERATION_H
