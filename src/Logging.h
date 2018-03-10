#ifndef QLOW_LOGGING_H
#define QLOW_LOGGING_H

#include <iostream>
#include <stack>

namespace qlow
{
    class Out;
}


class qlow::Out :
    public std::ostream,
    private std::streambuf
{
protected:
    std::ostream& target;
    bool firstChar;
    int indentVal;

public:
    Out(std::ostream& target);
    ~Out(void) = default;

    inline void indent(int width = 4) { indentVal += width; }
    inline void unindent(int width = 4) { indentVal -= width; }

    static Out stdout;

protected:
    virtual int overflow(int c);
    //virtual std::streamsize xsputn (const char* s, std::streamsize n);
};


#endif // QLOW_LOGGING_H

