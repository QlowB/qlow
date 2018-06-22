#ifndef QLOW_LOGGING_H
#define QLOW_LOGGING_H

#include <iostream>


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
    
    class NullStream :
        public std::ostream,
        public std::streambuf
    {
    public:
        NullStream(void) : std::ostream{ this } {}
        inline int overflow(int c) override { return c; }
    };
    
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

    std::ostream& target;
    NullStream nullStream;
    bool firstChar;
    int indentVal;

    // type of current logging
    LogLevel logType = LogLevel::OFF;
    LogLevel logLevel = LogLevel::INFO;

    static Logger instance;

    void foreground(Color color, bool bright);
    void background(Color color, bool bright);
    void bold(void);
    void removeFormatting(void);

public:
    explicit Logger(std::ostream& target);
    ~Logger(void) = default;

    void logError(const std::string& errMsg);
    void logError(const std::string& errMsg, const qlow::CodePosition& cp);

    std::ostream& operator() (LogLevel ll);
    inline std::ostream& none (void) { return (*this)(LogLevel::NONE); }
    inline std::ostream& err  (void) { return (*this)(LogLevel::ERROR); }
    inline std::ostream& warn (void) { return (*this)(LogLevel::WARNING); }
    inline std::ostream& info (void) { return (*this)(LogLevel::INFO); }
    inline std::ostream& debug(void) { return (*this)(LogLevel::DEBUG); }
    inline std::ostream& trace(void) { return (*this)(LogLevel::TRACE); }

    inline void setLogType(LogLevel l) { logType = l; }

    inline static Logger& getInstance(void) { return instance; }

protected:
    int overflow(int c) override;
    //virtual std::streamsize xsputn (const char* s, std::streamsize n);
};


std::ostream& operator << (std::ostream& o, qlow::LogLevel l);

inline void shouldbe()
{
    qlow::Logger& logger = qlow::Logger::getInstance();
    logger.err() << "something happened" << std::endl;
}

#endif // QLOW_LOGGING_H

