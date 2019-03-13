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

    inline Printer(std::ostream& target, bool isTty) :
        std::ostream{ static_cast<std::streambuf*> (this) },
        target{ target },
        isTty{ isTty } {}
public:

    Printer(const Printer&) = delete;
    Printer(Printer&&) = delete;
    Printer& operator=(const Printer&) = delete;
    Printer& operator=(Printer&&) = delete;

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
    int sync(void) override;
    int overflow(int c) override;
};





#endif // QLOW_LOGGING_H

