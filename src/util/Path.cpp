#include "Path.h"

using qlow::util::Path;

#ifdef QLOW_TARGET_WINDOWS
const std::string Path::dirSeparators = "\\/";
const std::string Path::defaultDirSeparator = "\\";
#else
const std::string Path::dirSeparators = "/";
const std::string Path::defaultDirSeparator = "/";
#endif


void Path::append(const Path& other)
{
    if (!endsWithSeparator())
        path += defaultDirSeparator;
    path += other.path;
}


Path Path::parentPath(void) const
{
    Path parent = *this;

    if (parent.path == defaultDirSeparator)
        return parent;

    if (parent.endsWithSeparator()) {
        parent.path.pop_back();
        return parent;
    }

    while (!parent.endsWithSeparator()) {
        parent.path.pop_back();
    }

    if (parent.path.size() >= 2 && parent.endsWithSeparator())
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
    return !path.empty() && dirSeparators.find(path[path.size() - 1]) != std::string::npos;
    /*return path.size() >= dirSeparator.size() &&
        std::equal(dirSeparator.rbegin(), dirSeparator.rend(), path.rbegin());*/
}
