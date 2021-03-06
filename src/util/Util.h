#ifndef QLOW_UTIL_H
#define QLOW_UTIL_H

#include <vector>
#include <memory>
#include <sstream>
#include <typeinfo>


namespace qlow
{
    
    template<typename T>
    using OwningList = std::vector<std::unique_ptr<T>>;
    
    
    
    /// I don't like this, but I lack better ideas at the moment.
    /// TODO: find better solution
    /*!
    * \brief tries to cast a unique_ptr and throws if it fails
    */
    template<typename T, typename U>
    std::unique_ptr<T> unique_dynamic_cast(std::unique_ptr<U>&& p)
    {
        U* released = p.release();
        if (T* casted = dynamic_cast<T*>(released); casted)
            return std::unique_ptr<T> (casted);
        else {
            delete released;
            //throw "failed unique dynamic cast";
            throw std::string("invalid unique_dynamic_cast from ") + typeid(p.get()).name() + " to " + typeid(T).name();
        }
    }


    
    namespace util
    {
        inline std::string toString(const void* a)
        {
            std::ostringstream o;
            o << a;
            return o.str();
        }
    }
}









#endif // QLOW_UTIL_H

