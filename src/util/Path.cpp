#include "Path.h"

using qlow::util::Path;


void Path::append(const Path& other)
{
    path += other.path;
}


Path::operator const std::string&(void) const
{
    return path;
}
