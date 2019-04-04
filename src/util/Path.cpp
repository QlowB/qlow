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


Path::operator const std::string&(void) const
{
    return path;
}


bool Path::endsWithSeparator(void) const
{
    return path.size() > dirSeparator.size() &&
        std::equal(dirSeparator.rbegin(), dirSeparator.rend(), path.rbegin());
}
