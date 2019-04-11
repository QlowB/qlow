#ifndef QLOW_MANGLING_H_
#define QLOW_MANGLING_H_

#include <string>

namespace qlow
{
    class Mangler;
    
    namespace sem
    {
        struct Method;
    }
}


class qlow::Mangler
{
public:
    Mangler(void) = default;

    std::string mangle(const sem::Method& method);
};

#endif // QLOW_MANGLING_H_
