#include "Printer.h"

//#include "Ast.h"
#include <cstdio>

using qlow::Printer;


#ifdef _WIN32
#include <io.h>
Printer Printer::instance(std::cout, _isatty(_fileno(stdout)));
#else
#include <unistd.h>
Printer Printer::instance(std::cout, isatty(fileno(stdout)));
#endif


void Printer::foreground(Color color, bool bright)
{
    if (!isTty) return;
    using std::string;
    string cchar = string(1, '0' + color);
    string isbright = bright ? "9" : "3";
    target << "\033[" << isbright << cchar << "m";
}


void Printer::background(Color color, bool bright)
{
    if (!isTty) return;
    using std::string;
    string cchar = string(1, '0' + color);
    string isbright = bright ? "10" : "4";
    target << "\033[" << isbright << cchar << "m"; 
}


void Printer::bold(void)
{
    if (!isTty) return;
    target << "\033[1m";
}


void Printer::removeFormatting(void)
{
    if (!isTty) return;
    target << "\033[0m";
}


int Printer::sync(void)
{
    return target.eof() ? EOF : 0;
}


int Printer::overflow(int c)
{
    target.put(char(c));
    return 0;
}
