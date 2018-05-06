#ifndef QLOW_UTIL_H
#define QLOW_UTIL_H

#include <vector>
#include <memory>


namespace qlow
{
    namespace util
    {
        template<typename T>
        using OwningList = std::vector<std::unique_ptr<T>>;
    }
}









#endif // QLOW_UTIL_H


