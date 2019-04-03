#include "Linking.h"
#include "Printer.h"

#include <vector>
#include <string>

#include <cstring>
#if defined(__linux__) || defined(__APPLE__) || defined(__unix__)
#include <unistd.h>
#include <sys/wait.h>
#endif


std::string qlow::getExternalSymbol(const std::string& name)
{
#ifdef QLOW_TARGET_APPLE
    return "_" + name;
#else
    return name;
#endif
}


std::string qlow::getLinkerExecutable(void)
{
    return "ld";
}


int qlow::invokeProgram(const std::string& path, const std::vector<std::string>& args)
{
#ifdef _WIN32
    throw "unimplemented on windows";
#elif defined(__linux__) || defined(__APPLE__) || defined(__unix__)
    pid_t pid = fork();
    
    if (pid == 0) { // we are the child
        char** c_args = new char*[args.size() + 2];
        c_args[0] = new char[path.size() + 1];
        strncpy(c_args[0], path.c_str(), path.size() + 1);
        for (size_t i = 0; i < args.size(); i++) {
            c_args[i + 1] = new char[args[i].size() + 1];
            strncpy(c_args[i + 1], args[i].c_str(), args[i].size() + 1);
        }
        c_args[args.size() + 1] = nullptr;
#ifdef DEBUGGING
        for (size_t i = 0; i < args.size() + 1; i++)
            Printer::getInstance() << c_args[i] << " ";
        Printer::getInstance() << std::endl;
#endif
        execvp(path.c_str(), c_args);
    }
    else if (pid > 0) { // we are the parent
        int exitStatus;
        pid_t p;
        while ((waitpid(pid, &exitStatus, 0)) > 0) {
            if (WIFEXITED(exitStatus)) {
                return WEXITSTATUS(exitStatus);
            }
            else {
                throw "child interrupted";
            }
        }
    }
    else {
        throw "fork failed";
    }
#else
#error unsupported platform
#endif
    throw "error invoking program";
}








