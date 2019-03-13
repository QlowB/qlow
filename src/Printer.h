#ifndef QLOW_LOGGING_H
#define QLOW_LOGGING_H

#include <iostream>

namespace qlow
{
    class Printer;
}


class qlow::Printer :
    public std::ostream,
    private std::streambuf
{
    std::ostream& target;
    bool isTty;

    static Printer instance;
public:
    inline Printer(std::ostream& target, bool isTty) :
        target{ target }, isTty{ isTty } {}

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

    inline static Printer& getInstance(void) { return instance; }
protected:
    int overflow(int c) override;
};





#endif // QLOW_LOGGING_H

