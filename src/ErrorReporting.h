#ifndef QLOW_ERROR_REPORTING
#define QLOW_ERROR_REPORTING

#include "Logging.h"


namespace qlow
{
    struct CodePosition;
    
    class CompileError;
    
    class SyntaxError;
    class SemanticError;
    
    void reportError(const CompileError& ce);
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
    virtual void print(Logger& logger) const = 0;
    
    void underlineError(Logger& logger) const;
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
    
    virtual void print(Logger&) const override;
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

    virtual void print(Logger&) const override;
};


#endif // QLOW_ERROR_REPORTING
