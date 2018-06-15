#ifndef QLOW_VISITOR_H
#define QLOW_VISITOR_H


namespace qlow
{
    template<typename R, typename A, typename... T>
    class Visitor;

    template<typename R, typename A>
    class Visitor<R, A>
    {
    public:
        using ReturnType = R;
    };
    
    template<typename R, typename A, typename T>
    class Visitor<R, A, T> :
        public Visitor<R, A>
    {
    public:
        using ReturnType = R;
        virtual R visit(T& arg, A& arg2) = 0;
        inline R invokeVisit(T& arg, A& arg2) {
            arg.accept(*this, arg2);
        }
    };


    template<typename R, typename A, typename T, typename... V>
    class Visitor<R, A, T, V...> :
        public Visitor<R, A, V...>
    {
    public:
        using Visitor<R, A, V...>::visit;
        using ReturnType = R;
        virtual R visit(T& arg, A& arg2) = 0;
        inline R invokeVisit(T& arg, A& arg2) {
            arg.accept(*this, arg2);
        }
    };


    template<typename RT, typename A, typename V>
    class Visitable
    {
    public:
        virtual ~Visitable(void) {}
        virtual RT accept(V& visitor, A& arg2) = 0;
    };




//
//    template<class T>
//    class Visitable
//    {
//    public:
//        template<typename Visitor>
//        void accept(Visitor& v)
//        {
//            v.visit(static_cast<T&>(*this));
//        }
//    };
//

//
//// Visitor template declaration
//template<typename... Types>
//class Visitor;
//
//// specialization for single type    
//template<typename T>
//class Visitor<T> {
//public:
//    virtual void visit(T & visitable) = 0;
//};
//
//// specialization for multiple types
//template<typename T, typename... Types>
//class Visitor<T, Types...> : public Visitor<Types...> {
//public:
//    // promote the function(s) from the base class
//    using Visitor<Types...>::visit;
//
//    virtual void visit(T & visitable) = 0;
//};
//
//template<typename... Types>
//class Visitable {
//public:
//    virtual void accept(Visitor<Types...>& visitor) = 0;
//};
//
//template<typename Derived, typename... Types>
//class VisitableImpl : public virtual Visitable<Types...> {
//public:
//    virtual void accept(Visitor<Types...>& visitor) {
//        visitor.visit(static_cast<Derived&>(*this));
//    }
//};
//


#if 0
    template<typename... T>
    class Visitor;

    template<>
    class Visitor<>
    {
    public:
    };
    
    template<typename T>
    class Visitor<T> :
        public Visitor<>
    {
    public:
        virtual void visit(T& arg) = 0;
    };


    template<typename T, typename... V>
    class Visitor<T, V...> :
        public Visitor<V...>
    {
    public:
        using Visitor<V...>::visit;
        virtual void visit(T& arg) = 0;
    };

//
//    template<typename VT>
//    class VisitableBase
//    {
//    public:
//        virtual typename VisitorType::ReturnType accept(VisitorType& visitor);
//    };
//

    template<typename VT>
    class VisitableBase
    {
//    public:
//        using VisitorType = VT;
//        typename VisitorType::ReturnType accept(VisitorType& visitor)
//        {
//            return visitor.visit(static_cast<T&>(*this));
//        }
    public:
        template <typename T>
        void accept(T& visitor)
        {
            visitor.visit(static_cast<VT&>(*this));
        }
    };
#endif
}





#endif // QLOW_VISITOR_H

