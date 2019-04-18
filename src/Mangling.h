#ifndef QLOW_MANGLING_H_
#define QLOW_MANGLING_H_

#include <string>

namespace qlow
{
    class Mangler;
    
    namespace sem
    {
        struct Method;
        class Type;
    }



    std::string mangle(const sem::Method& method);
    std::string mangle(const sem::Type* type);
}


#endif // QLOW_MANGLING_H_
