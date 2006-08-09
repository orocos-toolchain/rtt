#ifndef ORO_TASK_METHOD_HPP
#define ORO_TASK_METHOD_HPP

#include <boost/function.hpp>
#include <string>
#include "UnMember.hpp"
#include "MethodBase.hpp"
#include "LocalMethod.hpp"
#include "ActionInterface.hpp"

namespace RTT
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
        : public detail::InvokerSignature<boost::function_traits<FunctionT>::arity,
                                          FunctionT,
                                          boost::shared_ptr< detail::MethodBase<FunctionT> > >
    {
        std::string mname;
        typedef detail::InvokerSignature<boost::function_traits<FunctionT>::arity,
                                         FunctionT,
                                         boost::shared_ptr< detail::MethodBase<FunctionT> > > Base;
    public:
        typedef FunctionT Signature;
        typedef typename boost::function_traits<Signature>::result_type result_type;
        typedef boost::function_traits<Signature> traits;
        typedef boost::shared_ptr< detail::MethodBase<FunctionT> > MethodBasePtr;
    
        /**
         * Create an empty Method object.
         * Use assignment to initialise it.
         * @see method
         */
        Method()
            : Base(), mname()
        {}

        /**
         * Create an empty Method object.
         * Use assignment to initialise it.
         * @see method
         */
        Method(std::string name)
            : Base(), mname(name)
        {}

        /** 
         * Method objects may be copied.
         * 
         * @param m the original
         */
        Method(const Method& m)
            : Base(m.impl),
              mname(m.mname)
        {}

        /** 
         * Method objects may be assigned
         * 
         * @param m the original
         * 
         * @return *this
         */
        Method& operator=(const Method& m)
        {
            if ( this == &m )
                return *this;
            mname = m.mname;
            this->impl = m.impl;
            return *this;
        }

        /** 
         * Initialise a nameless Method object from an implementation.
         * 
         * @param implementation The implementation which is acquired
         * by the Method object. If it has the wrong type, it is freed.
         */
        Method(boost::shared_ptr<ActionInterface> implementation)
            : Base( MethodBasePtr(boost::dynamic_pointer_cast< detail::MethodBase<Signature> >(implementation) ) ),
              mname()
        {
        }

        /** 
         * Method objects may be assigned to an implementation.
         * 
         * @param implementation the implementation, if it is not suitable,
         * it is freed.
         * 
         * @return *this
         */
        Method& operator=(ActionInterface* implementation)
        {
            if (this->impl == implementation)
                return *this;
            this->impl.reset( boost::dynamic_pointer_cast< detail::MethodBase<Signature> >(implementation) );
            return *this;
        }

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
            : Base( MethodBasePtr(new detail::LocalMethod<Signature>(meth, object) ) ),
              mname(name)
        {}

        /** 
         * Construct a Method from a function pointer or function object.
         * 
         * @param name the name of this method
         * @param meth an pointer to a function or function object.
         */
        template<class M>
        Method(std::string name, M meth)
            : Base( MethodBasePtr(new detail::LocalMethod<Signature>(meth) ) ),
              mname(name)
        {}

        /**
         * Clean up the Method object.
         */
        ~Method()
        {
        }

        /** 
         * Check if this Method is ready for execution.
         * 
         * @return true if so.
         */
        bool ready() const {
            return this->impl;
        }


        /**
         * Get the name of this method.
         */
        const std::string& getName() const {return mname;}

        /**
         * Returns the internal implementation of the Method object.
         */
        const MethodBasePtr getMethodImpl() const {
            return this->impl;
        }

        /**
         * Sets the internal implementation of the Method object.
         */
        void setMethodImpl( MethodBasePtr new_impl) const {
            this->impl = new_impl;
        }
    };

    /** 
     * Create a Method which executes a function locally.
     * 
     * @param name The name of the resulting Method object
     * @param method A pointer to a member function to be executed.
     * @param object A pointer to the object which has the above member function.
     */    
    template<class F, class O>
    Method< typename detail::UnMember<F>::type > method(std::string name, F method, O object) {
        return Method<  typename detail::UnMember<F>::type >(name, method, object);
    }

    /** 
     * Create a Method which executes a function locally.
     * 
     * @param name The name of the resulting Method object
     * @param method A pointer to a function to be executed.
     */
    template<class F>
    Method<F> method(std::string name, F method) {
        return Method<F>(name, method);
    }
    /** 
     * Create a Method which executes a function locally.
     * 
     * @param name The name of the resulting Method object
     * @param method A pointer to a function to be executed.
     */
    template<class F>
    Method< typename detail::ArgMember<F>::type > method_ds(std::string name, F method) {
        return Method<  typename detail::ArgMember<F>::type >(name, method);
    }
}

#endif
