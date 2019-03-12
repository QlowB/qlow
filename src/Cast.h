#ifndef QLOW_SEM_CAST_H
#define QLOW_SEM_CAST_H

#include <memory>
#include "Type.h"

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
    TypeId to;
    
    bool isExplicit;
    
    inline Cast(TypeId to) :
        to{ to },
        isExplicit{ true }
    {
    }
    
    inline Cast(TypeId to, bool isExplicit) :
        to{ to },
        isExplicit{ isExplicit }
    {
    }
};


#endif // QLOW_SEM_CAST_H
