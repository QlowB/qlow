#include "ErrorReporting.h"

#include <map>
#include <fstream>
#include <sstream>

using qlow::InternalError;
using qlow::CompileError;
using qlow::SyntaxError;
using qlow::SemanticError;

namespace qlow
{
    void reportError(const CompileError& ce) noexcept
    {
        Printer& printer = Printer::getInstance();
        
        ce.print(printer);
    }
    
    
    void reportError(const std::string& msg) noexcept
    {
        Printer& printer = Printer::getInstance();
        
        printError(printer, msg);
    }


    void printError(Printer& printer, const std::string& msg) noexcept
    {
        printer.bold();
        printer.foreground(Printer::RED, true);
        printer << "error: ";
        printer.removeFormatting();
        printer << msg << std::endl;
    }


    void printError(Printer& printer, const std::string& msg, const CodePosition& cp) noexcept
    {
        printer.bold();
        printer << cp.getReportFormat() << ": "; //cp.filename << ":" << cp.first_line << ":" << cp.first_column << ": ";
        printer.foreground(Printer::RED, true);
        printer << "error: ";
        printer.removeFormatting();
        printer << msg << std::endl;
    }
}


std::string qlow::CodePosition::getReportFormat(void) const noexcept
{
    std::ostringstream s;
    s << filename << ":" << first_line << ":" << first_column;
    return s.str();
}


void InternalError::print(Printer& printer) const noexcept
{
    printError(printer, getMessage());
    printer <<
        "\n"
        "This kind of error isn't supposed to happen.\n\n"
        "Please submit a bug report to nicolas.winkler@gmx.ch\n"
    ;
}


const std::string& InternalError::getMessage(void) const noexcept
{
    static std::map<ErrorCode, std::string> errors = {
        {ErrorCode::OUT_OF_MEMORY, "out of memory"},
        {ErrorCode::PARSER_INIT_FAILED, "parser initialization failed"},
        {ErrorCode::PARSER_DEST_FAILED, "parser destruction failed"},
        {ErrorCode::PARSER_FAILED, "parser failed"},
        {ErrorCode::PARSER_FAILED, "invalid type encountered"},
    };
    return errors.at(errorCode);
}


CompileError::~CompileError(void)
{
}


// TODO rewrite more compact and more readable
void CompileError::underlineError(Printer& printer) const noexcept
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


void SyntaxError::print(Printer& printer) const noexcept
{
    using namespace std::literals;
    if (message == "")
        printError(printer, "Syntax error", where);
    else
        printError(printer, "Syntax error: "s + message, where);
    underlineError(printer);
}


void SemanticError::print(Printer& printer) const noexcept
{
    std::string errMsg = getMessage();
    printError(printer, errMsg + (errMsg != "" ?  ": " : "") + message, where);
    underlineError(printer);
}


std::string SemanticError::getMessage(void) const noexcept
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


SemanticError SemanticError::invalidReturnType(const std::string& should,
    const std::string& is, const CodePosition& where)
{
    return SemanticError{ INVALID_RETURN_TYPE, "invalid return type: return type should be " +
        should + ", but " + is + " is given.", where };
}

