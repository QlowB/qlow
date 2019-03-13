#ifndef QLOW_ERROR_REPORTING
#define QLOW_ERROR_REPORTING

#include "Printer.h"


namespace qlow
{
    struct CodePosition;
    
    class CompileError;
    
    class SyntaxError;
    
    class SemanticError;
    class NotLValue;
    
    void reportError(const CompileError& ce);
    void reportError(const std::string& message);
    void printError(Printer& printer, const std::string& message);
    void printError(Printer& printer, const std::string& message, const CodePosition& where);
}


/*!
 * \brief bison-compatible location struct
 */
struct qlow::CodePosition
{
    const char* filename = "";
    int first_line;
    int last_line;
    int first_column;
    int last_column;
    
    inline bool isMultiline(void) const { return first_line != last_line; }

    std::string getReportFormat(void) const;
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
    virtual void print(Printer& printer = Printer::getInstance()) const = 0;
    
    void underlineError(Printer& printer = Printer::getInstance()) const;
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
    
    virtual void print(Printer&) const override;
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

    virtual void print(Printer& p = Printer::getInstance()) const override;
    virtual std::string getMessage(void) const;
};


class qlow::NotLValue : public SemanticError
{
    std::string type;
public:
    inline NotLValue(const std::string& type, const CodePosition& where) :
        SemanticError{ where },
        type{ type }
    {
    }
    
    inline virtual std::string getMessage(void) const override
    {
        return "Can't take address of non-lvalue value of type '" +
            type + "'";
    }
};


#endif // QLOW_ERROR_REPORTING
