#include "ErrorReporting.h"

#include <map>
#include <fstream>

using qlow::CompileError;
using qlow::SyntaxError;
using qlow::SemanticError;


CompileError::~CompileError(void)
{
}


void CompileError::underlineError(Logger& logger) const
{
    // TODO implement for multiline errors
    //if (where.first_line != where.last_line)
    //    throw "TODO";
    std::ifstream file(where.filename);
    
    if (!file)
        return;
    
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


void SyntaxError::print(Logger& logger) const
{
    logger.logError("Syntax error", where);
    underlineError(logger);
}


void SemanticError::print(Logger& logger) const
{
    static std::map<ErrorCode, std::string> error = {
        {UNKNOWN_TYPE, "unknown type"},
        {FEATURE_NOT_FOUND, "method or variable not found"},
        {DUPLICATE_CLASS_DEFINITION, "duplicate class definition"},
        {DUPLICATE_FIELD_DECLARATION, "duplicate field declaration"},
        {DUPLICATE_METHOD_DEFINITION, "duplicate method definition"},
    };
    std::string& errMsg = error[errorCode];
    logger.logError(errMsg + (errMsg != "" ?  ": " : "") + message, where);
    underlineError(logger);
}


