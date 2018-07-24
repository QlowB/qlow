#ifndef QLOW_SEM_CAST_H
#define QLOW_SEM_CAST_H

#include <memory>

namespace qlow
{
    namespace sem
    {
        // forward declaration
        class Type;
        
        class Cast;
    }
}


class qlow::sem::Cast
{
public:
    std::weak_ptr<Type> to;
    
    bool isExplicit;
    
    inline Cast(std::weak_ptr<Type> to) :
        to{ std::move(to) },
        isExplicit{ true }
    {
    }
    
    inline Cast(std::weak_ptr<Type> to, bool isExplicit) :
        to{ std::move(to) },
        isExplicit{ isExplicit }
    {
    }
};


#endif // QLOW_SEM_CAST_H
