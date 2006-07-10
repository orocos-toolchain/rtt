#ifndef ORO_TASK_METHOD_HPP
#define ORO_TASK_METHOD_HPP

#include <boost/function.hpp>
#include <string>
#include "UnMember.hpp"
#include "BindStorage.hpp"

namespace ORO_Execution
{
    /**
     * A method which executes a function.
     *
     * Usage: 
     @code
     Method<double(int, double)> mymeth("name", &Class::foo, &c);
     double result = mymeth( 3, 1.9);
     @endcode
     */
    template<class FunctionT>
    class Method
    {
        std::string mname;
        boost::function<FunctionT> mmeth;
    public:
        typedef FunctionT Signature;
        typedef typename boost::function_traits<Signature>::result_type result_type;
        typedef boost::function_traits<Signature> traits;
    
        /**
         * Create an empty Method object.
         * Use assignment to initialise it.
         * @see method
         */
        Method()
        {}

        /** 
         * Construct a Method from a class member pointer and an
         * object of that class.
         * 
         * @param name The name of this method
         * @param meth A pointer to a class member function
         * @param object An object of the class which has \a meth as member function.
         */
        template<class M, class ObjectType>
        Method(std::string name, M meth, ObjectType object)
            : mname(name), mmeth( detail::MethodBinder<Signature>()(meth, object) )
        {}

        /** 
         * Construct a Method from a function pointer or function object.
         * 
         * @param name the name of this method
         * @param meth an pointer to a function or function object.
         */
        template<class M>
        Method(std::string name, M meth)
            : mname(name), mmeth( meth )
        {}

        /**
         * Get the name of this method.
         */
        const std::string& getName() const {return mname;}

        /**
         * Invoke this operator if the method has no arguments.
         */
        result_type operator()()
        {
            return mmeth();
        }

        /**
         * Invoke this operator if the method has one argument.
         */
        template<class T1>
        result_type operator()(T1 t)
        {
            return mmeth(t);
        }

        /**
         * Invoke this operator if the method has two arguments.
         */
        template<class T1, class T2>
        result_type operator()(T1 t1, T2 t2)
        {
            return mmeth(t1, t2);
        }

        /**
         * Invoke this operator if the method has three arguments.
         */
        template<class T1, class T2, class T3>
        result_type operator()(T1 t1, T2 t2, T3 t3)
        {
            return mmeth(t1, t2, t3);
        }

        /**
         * Invoke this operator if the method has four arguments.
         */
        template<class T1, class T2, class T3, class T4>
        result_type operator()(T1 t1, T2 t2, T3 t3, T4 t4)
        {
            return mmeth(t1, t2, t3, t4);
        }

        boost::function<FunctionT> getMethodFunction() const {
            return mmeth;
        }
    };

    template<class F, class O>
    Method< typename detail::UnMember<F>::type > method(std::string name, F method, O object) {
        return Method<  typename detail::UnMember<F>::type >(name, method, object);
    }
}

#endif
