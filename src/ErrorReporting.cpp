#include "ErrorReporting.h"

#include <map>
#include <fstream>
#include <sstream>

using qlow::CompileError;
using qlow::SyntaxError;
using qlow::SemanticError;

namespace qlow
{
    void reportError(const CompileError& ce)
    {
        Printer& printer = Printer::getInstance();
        
        ce.print(printer);
    }
    
    
    void reportError(const std::string& msg)
    {
        Printer& printer = Printer::getInstance();
        
        printError(printer, msg);
        
        printer <<
            "\n"
            "This kind of error isn't supposed to happen.\n\n"
            "Please submit a bug report to nicolas.winkler@gmx.ch\n"
        ;
    }


    void printError(Printer& printer, const std::string& msg)
    {
        printer.bold();
        printer.foreground(Printer::RED, true);
        printer << "error: ";
        printer.removeFormatting();
        printer << msg << std::endl;
    }


    void printError(Printer& printer, const std::string& msg, const CodePosition& cp)
    {
        printer.bold();
        printer << cp.getReportFormat() << ": "; //cp.filename << ":" << cp.first_line << ":" << cp.first_column << ": ";
        printer.foreground(Printer::RED, true);
        printer << "error: ";
        printer.removeFormatting();
        printer << msg << std::endl;
    }
}


std::string qlow::CodePosition::getReportFormat(void) const
{
    std::ostringstream s;
    if (filename == nullptr) {
        s << first_line << ":" << first_column;
    }
    else {
        s << filename << ":" << first_line << ":" << first_column;
    }
    return s.str();
}


CompileError::~CompileError(void)
{
}


// TODO rewrite more compact and more readable
void CompileError::underlineError(Printer& printer) const
{
    std::ifstream file(where.filename);
    
    if (!file)
        return;
    
    if (where.isMultiline()) {
        int lineNr = 1;
        while (lineNr < where.first_line) {
            if (file.get() == '\n') {
                lineNr++;
            }
        }
        std::string line;
        std::getline(file, line);
        
        int lineNrLength = std::to_string(lineNr).size();
        
        printer << "from here:" << std::endl;
        printer.foreground(Printer::Color::YELLOW, true);
        printer << lineNr;
        printer.removeFormatting();
        printer << ": " << line << std::endl;
        for (int i = 0; i < where.first_column + lineNrLength + 2; i++) {
            printer << ' ';
        }
        printer.foreground(Printer::Color::RED, true);
        for (size_t i = where.first_column; i < line.size(); i++) {
            printer << '^';
        }
        printer.removeFormatting();
        
        lineNr++;
        while (lineNr < where.last_line) {
            if (file.get() == '\n') {
                lineNr++;
            }
        }
        
        std::getline(file, line);
        lineNrLength = std::to_string(lineNr).size();
        printer << std::endl << "to here:" << std::endl;
        
        printer.foreground(Printer::Color::YELLOW, true);
        printer << lineNr;
        printer.removeFormatting();
        printer << ": " << line << std::endl;
        for (int i = 0; i < lineNrLength + 2; i++) {
            printer << ' ';
        }
        printer.foreground(Printer::Color::RED, true);
        for (int i = 0; i < where.last_column; i++) {
            printer << '^';
        }
        printer.removeFormatting();
        printer << std::endl;
        
    }
    else {
        int lineNr = 1;
        while (lineNr < where.first_line) {
            if (file.get() == '\n') {
                lineNr++;
            }
        }
        std::string line;
        std::getline(file, line);
        printer << line << std::endl;
        for (int i = 0; i < where.first_column; i++) {
            printer << ' ';
        }
        printer.foreground(Printer::Color::RED, true);
        for (int i = where.first_column; i < where.last_column; i++) {
            printer << '^';
        }
        printer.removeFormatting();
        printer << std::endl;
    }
}


void SyntaxError::print(Printer& printer) const
{
    using namespace std::literals;
    if (message == "")
        printError(printer, "Syntax error", where);
    else
        printError(printer, "Syntax error: "s + message, where);
    underlineError(printer);
}


void SemanticError::print(Printer& printer) const
{
    std::string errMsg = getMessage();
    printError(printer, errMsg + (errMsg != "" ?  ": " : "") + message, where);
    underlineError(printer);
}


std::string SemanticError::getMessage(void) const
{
    static const std::map<ErrorCode, std::string> error = {
        {UNKNOWN_TYPE, "unknown type"},
        {FEATURE_NOT_FOUND, "method or variable not found"},
        {DUPLICATE_CLASS_DEFINITION, "duplicate class definition"},
        {DUPLICATE_FIELD_DECLARATION, "duplicate field declaration"},
        {DUPLICATE_METHOD_DEFINITION, "duplicate method definition"},
        {OPERATOR_NOT_FOUND, ""},
        {WRONG_NUMBER_OF_ARGUMENTS, "wrong number of arguments passed"},
    };
    return error.at(errorCode);
}


