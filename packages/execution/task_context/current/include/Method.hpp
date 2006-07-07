#ifndef ORO_TASK_METHOD_HPP
#define ORO_TASK_METHOD_HPP

#include <boost/function.hpp>
#include <string>

namespace ORO_Execution
{
    namespace detail {
        template<int, class F>
        struct MethodBinderImpl;

        template<class F>
        struct MethodBinderImpl<0,F>
        {
            template<class M, class O>
            boost::function<F> operator()(M m, O o) {
                return boost::bind( m, o );
            }
        };

        template<class F>
        struct MethodBinderImpl<1,F>
        {
            template<class M, class O>
            boost::function<F> operator()(M m, O o) {
                return boost::bind( m, o, _1 );
            }
        };

        template<class F>
        struct MethodBinderImpl<2,F>
        {
            template<class M, class O>
            boost::function<F> operator()(M m, O o) {
                return boost::bind( m, o, _1, _2 );
            }
        };

        template<class F>
        struct MethodBinderImpl<3,F>
        {
            template<class M, class O>
            boost::function<F> operator()(M m, O o) {
                return boost::bind( m, o, _1, _2, _3 );
            }
        };

        template<class F>
        struct MethodBinderImpl<4,F>
        {
            template<class M, class O>
            boost::function<F> operator()(M m, O o) {
                return boost::bind( m, o, _1, _2, _3, _4 );
            }
        };

        template<class F>
        struct MethodBinder
            : public MethodBinderImpl<boost::function_traits<F>::arity, F>
        {};
    }


    /**
     * A method which executes a function.
     *
     * Usage: Method<double(int, double)> mymeth("name", &Class::foo, &c);
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
         * Construct a Method from a class member pointer and an
         * object of that class.
         */
        template<class M, class ObjectType>
        Method(std::string name, M meth, ObjectType t)
            : mname(name), mmeth( detail::MethodBinder<Signature>()(meth,t) )
        {}

        /**
         * Construct a Method from a function pointer.
         */
        template<class M>
        Method(std::string name, M meth)
            : mname(name), mmeth( meth )
        {}

        const std::string& getName() const {return mname;}

        result_type operator()()
        {
            return mmeth();
        }

        template<class T1>
        result_type operator()(T1 t)
        {
            return mmeth(t);
        }

        template<class T1, class T2>
        result_type operator()(T1 t1, T2 t2)
        {
            return mmeth(t1, t2);
        }

        template<class T1, class T2, class T3>
        result_type operator()(T1 t1, T2 t2, T3 t3)
        {
            return mmeth(t1, t2, t3);
        }

        template<class T1, class T2, class T3, class T4>
        result_type operator()(T1 t1, T2 t2, T3 t3, T4 t4)
        {
            return mmeth(t1, t2, t3, t4);
        }

        boost::function<FunctionT> getMethodFunction() const {
            return mmeth;
        }
    };
}

#endif
