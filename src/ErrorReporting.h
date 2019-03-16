#ifndef QLOW_ERROR_REPORTING
#define QLOW_ERROR_REPORTING

#include "Printer.h"


namespace qlow
{
    enum class ErrorCode
    {
        /// no files were given to process
        NO_INFILES,
    };

    struct CodePosition;
    
    class InternalError;

    class CompileError;
    class SyntaxError;
    
    class SemanticError;
    class NotLValue;
    
    void reportError(const CompileError& ce) noexcept;
    void reportError(const std::string& message) noexcept;
    void printError(Printer& printer, const std::string& message) noexcept;
    void printError(Printer& printer, const std::string& message, const CodePosition& where) noexcept;

    void printError(ErrorCode ec, Printer& printer) noexcept;
}



/*!
 * \brief bison-compatible location struct
 */
struct qlow::CodePosition
{
    std::string filename = "";
    int first_line;
    int last_line;
    int first_column;
    int last_column;
    
    inline bool isMultiline(void) const { return first_line != last_line; }

    std::string getReportFormat(void) const noexcept;
};


class qlow::InternalError
{
public:
    enum ErrorCode
    {
        OUT_OF_MEMORY,
        /// initialization of the flex lexer failed
        PARSER_INIT_FAILED,
        /// destruction of the flex lexer failed
        PARSER_DEST_FAILED,
        /// bison routine returned error value
        PARSER_FAILED,
        /// tried to determine the kind of an invalid type
        INVALID_TYPE
    };
private:
    ErrorCode errorCode;
public:
    InternalError(ErrorCode ec) :
        errorCode{ ec } {}
    void print(Printer& printer = Printer::getInstance()) const noexcept;
    const std::string& getMessage(void) const noexcept;
};


class qlow::CompileError
{
protected:
    CodePosition where;
public:
    inline CompileError(const CodePosition& where) :
        where{ where }
    {
    }
    
    virtual ~CompileError(void);
    virtual void print(Printer& printer = Printer::getInstance()) const noexcept = 0;
    
    void underlineError(Printer& printer = Printer::getInstance()) const noexcept;
};


class qlow::SyntaxError : public CompileError
{
    std::string message;
public:
    inline SyntaxError(const CodePosition& where) :
        CompileError{ where }
    {
    }
    
    inline SyntaxError(const std::string& message, const CodePosition& where) :
        CompileError{ where },
        message{ message }
    {
    }
    
    virtual void print(Printer&) const noexcept override;
};


class qlow::SemanticError : public CompileError
{
    std::string message;
public:
    enum ErrorCode
    {
        UNKNOWN_TYPE,
        DUPLICATE_CLASS_DEFINITION,
        DUPLICATE_FIELD_DECLARATION,
        DUPLICATE_METHOD_DEFINITION,
        
        OPERATOR_NOT_FOUND,
        FEATURE_NOT_FOUND,
        WRONG_NUMBER_OF_ARGUMENTS,
        TYPE_MISMATCH,
        INVALID_RETURN_TYPE,
    };
    
    
    ErrorCode errorCode;
public:
    inline SemanticError(ErrorCode ec, const std::string& arg, const
        qlow::CodePosition& where) :
            CompileError{ where },
            message{ arg },
            errorCode{ ec }
    {
    }
    
    inline SemanticError(const CodePosition& where) :
        CompileError{ where }
    {
    }

    virtual void print(Printer& p = Printer::getInstance()) const noexcept override;
    virtual std::string getMessage(void) const noexcept;

    static SemanticError invalidReturnType(const std::string& should, const std::string& is, const CodePosition& where);
};


class qlow::NotLValue : public SemanticError
{
    std::string type;
public:
    inline NotLValue(const std::string& type, const CodePosition& where) noexcept :
        SemanticError{ where },
        type{ type }
    {
    }
    
    inline virtual std::string getMessage(void) const noexcept override
    {
        return "Can't take address of non-lvalue value of type '" +
            type + "'";
    }
};


#endif // QLOW_ERROR_REPORTING
