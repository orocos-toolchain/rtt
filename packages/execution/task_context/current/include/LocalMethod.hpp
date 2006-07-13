#ifndef ORO_LOCAL_METHOD_HPP
#define ORO_LOCAL_METHOD_HPP

#include <boost/function.hpp>
#include <string>
#include "Invoker.hpp"
#include "BindStorage.hpp"

namespace ORO_Execution
{
    namespace detail
    {
        template<class FunctionT>
        class LocalMethodImpl
            : public MethodBase<FunctionT>
        {
        protected:
            boost::function<FunctionT> mmeth;
        public:
            typedef FunctionT Signature;
            typedef typename boost::function_traits<Signature>::result_type result_type;
            typedef boost::function_traits<Signature> traits;

            void readArguments() {}

            bool execute() {
                return false;
            }
                
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

        };

        /**
         * A method which executes a local function.
         *
         * Usage: 
         @code
         LocalMethod<double(int, double)> mymeth( &Class::foo, &c);
         double result = mymeth( 3, 1.9);
         @endcode
        */
        template<class FunctionT>
        struct LocalMethod
            : public Invoker<FunctionT,LocalMethodImpl<FunctionT> >
        {
            typedef FunctionT Signature;
            typedef typename boost::function_traits<Signature>::result_type result_type;
            typedef boost::function_traits<Signature> traits;

            /**
             * Create an empty LocalMethod object.
             * Use assignment to initialise it.
             * @see method
             */
            LocalMethod()
            {}

            /** 
             * Construct a LocalMethod from a class member pointer and an
             * object of that class.
             * 
             * @param name The name of this method
             * @param meth A pointer to a class member function
             * @param object An object of the class which has \a meth as member function.
             */
            template<class M, class ObjectType>
            LocalMethod(M meth, ObjectType object)
            {
                this->mmeth = detail::MethodBinder<Signature>()(meth, object);
            }

            /** 
             * Construct a LocalMethod from a function pointer or function object.
             * 
             * @param name the name of this method
             * @param meth an pointer to a function or function object.
             */
            template<class M>
            LocalMethod(M meth)
            {
                this->mmeth = meth;
            }

            boost::function<Signature> getMethodFunction() const 
            {
                return this->mmeth;
            }

            LocalMethod<Signature>* clone() const
            {
                return new LocalMethod<Signature>(*this);
            }
        };
    }
}

#endif
