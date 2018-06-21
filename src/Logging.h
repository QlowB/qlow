#ifndef QLOW_LOGGING_H
#define QLOW_LOGGING_H

#include <iostream>
#include <stack>


namespace qlow
{
    class Logger;

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
    
    struct CodePosition;
}


class qlow::Logger :
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
    
    static Logger instance;

    // color
    enum Color {
        BLACK = 0,
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN,
        WHITE
    };

    void foreground(Color color, bool bright);
    void background(Color color, bool bright);
    void bold(void);
    void removeFormatting(void);

public:
    explicit Logger(std::ostream& target);
    ~Logger(void) = default;

    void logError(const std::string& errMsg);
    void logError(const std::string& errMsg, const qlow::CodePosition& cp);

    inline void setLogType(LogLevel l) { logType = l; }

    inline static Logger& getInstance(void) { return instance; }

protected:
    virtual int overflow(int c);
    //virtual std::streamsize xsputn (const char* s, std::streamsize n);
};


std::ostream& operator << (std::ostream& o, qlow::LogLevel l);


#endif // QLOW_LOGGING_H

