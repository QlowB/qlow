#ifndef QLOW_UTIL_PATH_H_
#define QLOW_UTIL_PATH_H_

#include <string>
#include <utility>

namespace qlow::util
{
    class Path;
}


class qlow::util::Path
{
    std::string path;

    static const std::string dirSeparator;

public:

    inline Path(std::string path) :
        path{ std::move(path) }
    {
    }

    void append(const Path& other);
    operator const std::string&(void) const;
    
private:
    bool endsWithSeparator(void) const;
};

#endif // QLOW_UTIL_PATH_H_
