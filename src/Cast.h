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
    std::shared_ptr<Type> from;
    std::shared_ptr<Type> to;
    
    bool isExplicit;
    
    inline Cast(std::shared_ptr<Type> from, std::shared_ptr<Type> to) :
        from{ std::move(from) },
        to{ std::move(to) },
        isExplicit{ true }
    {
    }
    
    inline Cast(std::shared_ptr<Type> from, std::shared_ptr<Type> to,
                bool isExplicit) :
        from{ std::move(from) },
        to{ std::move(to) },
        isExplicit{ isExplicit }
    {
    }
};


#endif // QLOW_SEM_CAST_H
