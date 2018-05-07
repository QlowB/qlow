#ifndef QLOW_UTIL_H
#define QLOW_UTIL_H

#include <vector>
#include <memory>
#include <sstream>


namespace qlow
{
    namespace util
    {
        template<typename T>
        using OwningList = std::vector<std::unique_ptr<T>>;

        inline std::string toString(const void* a)
        {
            std::ostringstream o;
            o << a;
            return o.str();
        }
    }
}









#endif // QLOW_UTIL_H

