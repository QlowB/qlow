#include "Logging.h"

using qlow::Out;


Out Out::stdout(std::cout);


Out::Out(std::ostream& target) :
    std::ostream(this),
    target(target),
    indentVal(0),
    firstChar(true)
{
}


int Out::overflow(int c)
{
    // needs indenting on first char
    if (firstChar)
        for (int i = 0; i < indentVal; i++)
            target.put(' ');

    if (logType <= logLevel)
        target.put(char(c));

    if (c == '\n') {
        // remove formatting
        target << "\033[0m";
        logType = LogLevel::NONE;
        firstChar = true;
    }
    else
        firstChar = false;

    return 0;
}


std::ostream& operator << (std::ostream& o, qlow::LogLevel l)
{
    switch(l)
    {
        case qlow::LogLevel::WARNING:
            try {
                dynamic_cast<qlow::Out&>(o).setLogType(l);
            }
            catch(...) {}
            return o << "\033[33m";
    }
    return o;
}


/*std::streamsize Out::xsputn(const char* s, std::streamsize n)
{
    target.write(s, n);
}*/


/*int main()
{
    qlow::Out& l = Out::stdout;
    l << "aha" << std::endl;
    l.indent(10);
    l << "aha" << std::endl;
    l << qlow::LogLevel::WARNING << "ee";
    l.unindent(10);
    l << "aha" << std::endl;
}*/








