#include "Logging.h"

#include "Ast.h"

using qlow::Logger;


Logger Logger::instance(std::cout);


Logger::Logger(std::ostream& target) :
    std::ostream{ this },
    target{ target },
    indentVal{ 0 },
    firstChar{ true }
{
}


void Logger::foreground(Color color, bool bright)
{
    using std::string;
    string cchar = string(1, '0' + color);
    string isbright = bright ? "9" : "3";
    *this << "\033[" << isbright << cchar << "m";
}


void Logger::background(Color color, bool bright)
{
    using std::string;
    string cchar = string(1, '0' + color);
    string isbright = bright ? "10" : "4";
    *this << "\033[" << isbright << cchar << "m"; 
}


void Logger::bold(void)
{
    *this << "\033[1m";
}


void Logger::removeFormatting(void)
{
    *this << "\033[0m";
}


void Logger::logError(const std::string& errMsg)
{
    bold();
    foreground(RED, true);
    *this << "error: ";
    removeFormatting();
    *this << errMsg << std::endl;
}


void Logger::logError(const std::string& errMsg, const qlow::CodePosition& cp)
{
    bold();
    *this << cp.filename << ":" << cp.first_line << ":" << cp.first_column << ": ";
    foreground(RED, true);
    *this << "error: ";
    removeFormatting();
    *this << errMsg << std::endl;
}


int Logger::overflow(int c)
{
    target.put(char(c));
    return 0;
}


std::ostream& operator << (std::ostream& o, qlow::LogLevel l)
{
    switch(l)
    {
        case qlow::LogLevel::WARNING:
            try {
                dynamic_cast<qlow::Logger&>(o).setLogType(l);
            }
            catch(...) {}
            return o << "\033[33m";
    }
    return o;
}


/*std::streamsize Logger::xsputn(const char* s, std::streamsize n)
{
    target.write(s, n);
}*/


/*int main()
{
    qlow::Logger& l = Logger::stdout;
    l << "aha" << std::endl;
    l.indent(10);
    l << "aha" << std::endl;
    l << qlow::LogLevel::WARNING << "ee";
    l.unindent(10);
    l << "aha" << std::endl;
}*/








