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

    static const std::string dirSeparators;
    static const std::string defaultDirSeparator;

public:

    inline Path(std::string path) :
        path{ std::move(path) }
    {
    }

    inline Path(const char* path) :
        path{ path }
    {
    }

    void append(const Path& other);
    Path parentPath(void) const;

    Path operator +(const std::string& op) const;

    /// for compatibilty with std::map
    inline bool operator < (const Path& other) const { return path < other.path; }

    operator const std::string&(void) const;
    const std::string& string(void) const;
    const char* c_str(void) const;
    
private:
    bool endsWithSeparator(void) const;
};

#endif // QLOW_UTIL_PATH_H_

