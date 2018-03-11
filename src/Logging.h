#ifndef QLOW_LOGGING_H
#define QLOW_LOGGING_H

#include <iostream>
#include <stack>

namespace qlow
{
    // indenting ostream
    class Out;


    enum class LogLevel
    {
        NONE,
        ERROR,
        WARNING,
        INFO,
        DEBUG,
        TRACE,
        OFF,
    };
}


class qlow::Out :
    public std::ostream,
    private std::streambuf
{
protected:
    std::ostream& target;
    bool firstChar;
    int indentVal;

    // type of current logging
    LogLevel logType = LogLevel::OFF;
    LogLevel logLevel = LogLevel::INFO;

public:
    Out(std::ostream& target);
    ~Out(void) = default;

    inline void indent(int width = 4) { indentVal += width; }
    inline void unindent(int width = 4) { indentVal -= width; }

    inline void setLogType(LogLevel l) { logType = l; }

    static Out stdout;

protected:
    virtual int overflow(int c);
    //virtual std::streamsize xsputn (const char* s, std::streamsize n);
};


std::ostream& operator << (std::ostream& o, qlow::LogLevel l);


#endif // QLOW_LOGGING_H

