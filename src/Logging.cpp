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

    target.put(char(c));

    firstChar = c == '\n';

    return 0;
}


/*std::streamsize Out::xsputn(const char* s, std::streamsize n)
{
    target.write(s, n);
}*/


int main()
{
    qlow::Out& l = Out::stdout;
    l << "aha" << std::endl;
    l.indent(10);
    l << "aha" << std::endl;
    l.unindent(10);
    l << "aha" << std::endl;
}








