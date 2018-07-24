#include "ErrorReporting.h"

#include <map>
#include <fstream>

using qlow::CompileError;
using qlow::SyntaxError;
using qlow::SemanticError;


namespace qlow
{
    void reportError(const CompileError& ce)
    {
        Logger& logger = Logger::getInstance();
        
        ce.print(logger);
    }
    
    
    void reportError(const std::string& msg)
    {
        Logger& logger = Logger::getInstance();
        
        logger.logError(msg);
        
        logger.info() <<
            "\n"
            "This kind of error isn't supposed to happen.\n\n"
            "Please submit a bug report to nicolas.winkler@gmx.ch\n"
        ;
    }
}



CompileError::~CompileError(void)
{
}


// TODO rewrite more compact and more readable
void CompileError::underlineError(Logger& logger) const
{
    std::ifstream file(where.filename);
    
    if (!file)
        return;
    
    if (where.isMultiline()) {
        size_t lineNr = 1;
        while (lineNr < where.first_line) {
            if (file.get() == '\n') {
                lineNr++;
            }
        }
        std::string line;
        std::getline(file, line);
        
        int lineNrLength = std::to_string(lineNr).size();
        
        logger.err() << "from here:" << std::endl;
        logger.foreground(Logger::Color::YELLOW, true);
        logger.err() << lineNr;
        logger.removeFormatting();
        logger.err() << ": " << line << std::endl;
        for (size_t i = 0; i < where.first_column + lineNrLength + 2; i++) {
            logger.err() << ' ';
        }
        logger.foreground(Logger::Color::RED, true);
        for (size_t i = where.first_column; i < line.size(); i++) {
            logger.err() << '^';
        }
        logger.removeFormatting();
        
        lineNr++;
        while (lineNr < where.last_line) {
            if (file.get() == '\n') {
                lineNr++;
            }
        }
        
        std::getline(file, line);
        lineNrLength = std::to_string(lineNr).size();
        logger.err() << std::endl << "to here:" << std::endl;
        
        logger.foreground(Logger::Color::YELLOW, true);
        logger.err() << lineNr;
        logger.removeFormatting();
        logger.err() << ": " << line << std::endl;
        for (size_t i = 0; i < lineNrLength + 2; i++) {
            logger.err() << ' ';
        }
        logger.foreground(Logger::Color::RED, true);
        for (size_t i = 0; i < where.last_column; i++) {
            logger.err() << '^';
        }
        logger.removeFormatting();
        logger.err() << std::endl;
        
    }
    else {
        size_t lineNr = 1;
        while (lineNr < where.first_line) {
            if (file.get() == '\n') {
                lineNr++;
            }
        }
        std::string line;
        std::getline(file, line);
        logger.err() << line << std::endl;
        for (size_t i = 0; i < where.first_column; i++) {
            logger.err() << ' ';
        }
        logger.foreground(Logger::Color::RED, true);
        for (size_t i = where.first_column; i < where.last_column; i++) {
            logger.err() << '^';
        }
        logger.removeFormatting();
        logger.err() << std::endl;
    }
}


void SyntaxError::print(Logger& logger) const
{
    using namespace std::literals;
    if (message == "")
        logger.logError("Syntax error", where);
    else
        logger.logError("Syntax error: "s + message, where);
    underlineError(logger);
}


void SemanticError::print(Logger& logger) const
{
    std::string errMsg = getMessage();
    logger.logError(errMsg + (errMsg != "" ?  ": " : "") + message, where);
    underlineError(logger);
}


std::string SemanticError::getMessage(void) const
{
    static std::map<ErrorCode, std::string> error = {
        {UNKNOWN_TYPE, "unknown type"},
        {FEATURE_NOT_FOUND, "method or variable not found"},
        {DUPLICATE_CLASS_DEFINITION, "duplicate class definition"},
        {DUPLICATE_FIELD_DECLARATION, "duplicate field declaration"},
        {DUPLICATE_METHOD_DEFINITION, "duplicate method definition"},
        {OPERATOR_NOT_FOUND, ""},
        {WRONG_NUMBER_OF_ARGUMENTS, "wrong number of arguments passed"},
    };
    return error[errorCode];
}


