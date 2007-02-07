/***************************************************************************
  tag: FMTC  do nov 2 13:06:07 CET 2006  Method.hpp 

                        Method.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
    email                : peter.soetens@fmtc.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef ORO_TASK_METHOD_HPP
#define ORO_TASK_METHOD_HPP

#include <boost/function.hpp>
#include <string>
#include "UnMember.hpp"
#include "MethodBase.hpp"
#include "LocalMethod.hpp"
#include "ActionInterface.hpp"
#include "Logger.hpp"

namespace RTT
{
    /**
     * @defgroup Methods Method Interface
     * Invoking synchronous methods.
     * @ingroup RTTComponentInterface
     */

    
    /**
     * A method which executes a function.
     *
     * Usage: 
     @code
     Method<double(int, double)> mymeth("name", &Class::foo, &c);
     double result = mymeth( 3, 1.9);
     @endcode
     * @ingroup RTTComponentInterface
     * @ingroup Methods
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
            : Base( boost::dynamic_pointer_cast< detail::MethodBase<Signature> >(implementation) ),
              mname()
        {
            if ( !this->impl && implementation ) {
                log(Error) << "Tried to construct Method from incompatible type."<< endlog();
            }
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
            if (this->impl && this->impl == implementation)
                return *this;
            this->impl = boost::dynamic_pointer_cast< detail::MethodBase<Signature> >(implementation);
            if ( !this->impl && implementation ) {
                log(Error) << "Tried to assign Method '"<<mname<<"' from incompatible type."<< endlog();
            }
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
