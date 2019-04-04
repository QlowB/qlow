#include "Path.h"

using qlow::util::Path;

#ifdef QLOW_TARGET_WINDOWS
const std::string Path::dirSeparator = "\\";
#else
const std::string Path::dirSeparator = "/";
#endif


void Path::append(const Path& other)
{
    if (!endsWithSeparator())
        path += dirSeparator;
    path += other.path;
}


Path Path::parentPath(void) const
{
    Path parent = *this;

    if (parent.path == dirSeparator)
        return parent;

    if (parent.endsWithSeparator()) {
        parent.path.pop_back();
        return parent;
    }

    while (!parent.endsWithSeparator()) {
        parent.path.pop_back();
    }

    if (parent.path.size() > dirSeparator.size() && parent.endsWithSeparator())
        parent.path.pop_back();

    return parent;
}


Path Path::operator+(const std::string& op) const
{
    Path r = *this;
    r.path += op;
    return r;
}


Path::operator const std::string&(void) const
{
    return path;
}


const std::string& Path::string(void) const
{
    return path;
}


const char* Path::c_str(void) const
{
    return path.c_str();
}


bool Path::endsWithSeparator(void) const
{
    return path.size() >= dirSeparator.size() &&
        std::equal(dirSeparator.rbegin(), dirSeparator.rend(), path.rbegin());
}
