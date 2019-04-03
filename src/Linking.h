#ifndef QLOW_LINKING_H_
#define QLOW_LINKING_H_

#include <filesystem>

namespace qlow
{
    std::string getExternalSymbol(const std::string& name);
    std::string getLinkerExecutable(void);
    int invokeProgram(const std::string& path, const std::vector<std::string>& args);
}


#endif // QLOW_LINKING_H_


