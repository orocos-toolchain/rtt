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

#include <string>
#include <boost/function.hpp>
#include "base/MethodBase.hpp"
#ifdef ORO_TEST_METHOD
#include "internal/UnMember.hpp"
#include "internal/LocalMethod.hpp"
#endif
#include "internal/InvokerSignature.hpp"
#include "base/MethodBaseInvoker.hpp"
#include "Logger.hpp"
#ifdef ORO_REMOTING
#include "interface/OperationRepository.hpp"
#include "internal/RemoteMethod.hpp"
#endif

namespace RTT
{
    /**
     * @defgroup Methods Method Interface
     * Invoking synchronous and asynchronous methods.
     * @ingroup RTTComponentInterface
     */


    /**
     * A Method serves as a placeholder (aka 'proxy') for a remote
     * Operation. If you want to call an operation, you need a method to do it.
     *
     * The Method has the exact same signature template argument as the operation
     * it wishes to call.
     *
     * Asynchronous methods need a caller's ExecutionEngine to be able to process
     * the completion message.
     *
     * @ingroup RTTComponentInterface
     * @ingroup Methods
     */
    template<class SignatureT>
    class Method
        : public internal::InvokerSignature<boost::function_traits<SignatureT>::arity,
                                          SignatureT,
                                          boost::shared_ptr< base::MethodBase<SignatureT> > >,
          public base::MethodBaseInvoker
    {
        std::string mname;
        ExecutionEngine* mcaller;
    public:
        typedef SignatureT Signature;
        typedef internal::InvokerSignature<boost::function_traits<Signature>::arity,
                                         Signature,
                                         boost::shared_ptr< base::MethodBase<Signature> > > Base;
        typedef typename boost::function_traits<Signature>::result_type result_type;
        typedef boost::function_traits<Signature> traits;
        typedef boost::shared_ptr< base::MethodBase<Signature> > MethodBasePtr;

        /**
         * Create an empty Method object.
         * Use assignment to initialise it.
         */
        Method()
            : Base(), mname()
        {}

        /**
         * Create an empty Method object.
         * Use assignment to initialise it.
         */
        Method(std::string name, ExecutionEngine* caller = 0)
            : Base(), mname(name), mcaller(caller)
        {}

        /**
         * Method objects may be copied.
         *
         * @param m the original
         */
        Method(const Method& m)
            : Base(m.impl),
              mname(m.mname), mcaller(m.mcaller)
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
            mcaller = m.mcaller;
            this->impl = m.impl;
            return *this;
        }

        /**
         * Initialise a nameless Method object from an implementation.
         *
         * @param implementation The implementation which is acquired
         * by the Method object. If it has the wrong type, it is freed.
         */
        Method(boost::shared_ptr<base::DisposableInterface> implementation, ExecutionEngine* caller = 0)
            : Base( boost::dynamic_pointer_cast< base::MethodBase<Signature> >(implementation) ),
              mname(), mcaller(caller)
        {
            if ( !this->impl && implementation ) {
                log(Error) << "Tried to construct Method from incompatible type."<< endlog();
            } else
                this->impl->setCaller(mcaller);
        }

        /**
         * Method objects may be assigned to an implementation.
         *
         * @param implementation the implementation, if it is not suitable,
         * it is freed.
         *
         * @return *this
         */
        Method& operator=(boost::shared_ptr<base::DisposableInterface> implementation)
        {
            if (this->impl && this->impl == implementation)
                return *this;
            this->impl = boost::dynamic_pointer_cast< base::MethodBase<Signature> >(implementation);
            if ( !this->impl && implementation ) {
                log(Error) << "Tried to assign Method '"<<mname<<"' from incompatible type."<< endlog();
            } else
                this->impl->setCaller(mcaller);
            return *this;
        }

#ifdef ORO_TEST_METHOD
        /**
         * Construct a Method from a class member pointer and an
         * object of that class.
         *
         * @param name The name of this method
         * @param meth A pointer to a class member function
         * @param object An object of the class which has \a meth as member function.
         */
        template<class M, class ObjectType>
        Method(std::string name, M meth, ObjectType object, ExecutionEngine* ee = 0, ExecutionEngine* caller = 0)
            : Base( MethodBasePtr(new internal::LocalMethod<Signature>(meth, object, ee, caller) ) ),
              mname(name), mcaller(caller)
        {}

        /**
         * Construct a Method from a function pointer or function object.
         *
         * @param name the name of this method
         * @param meth an pointer to a function or function object.
         */
        template<class M>
        Method(std::string name, M meth, ExecutionEngine* ee = 0, ExecutionEngine* caller = 0)
            : Base( MethodBasePtr(new internal::LocalMethod<Signature>(meth,ee,caller) ) ),
              mname(name), mcaller(caller)
        {}
#endif
#ifdef ORO_REMOTING
        /**
         * Construct a Method from an operation repository part.
         *
         * @param name the name of this method
         * @param meth an pointer to a function or function object.
         */
        template<class M>
        Method(std::string name, interface::OperationRepositoryPart* orp, ExecutionEngine* caller = 0)
            : Base( MethodBasePtr(new internal::RemoteMethod<Signature>(orp,caller) ) ),
              mname(name)
        {}
#endif

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
        std::string const& getName() const {return mname;}

        bool setImplementation(boost::shared_ptr<base::DisposableInterface> implementation, ExecutionEngine* caller = 0) {
            *this = implementation;
            if ( this->impl ) {
                this->mcaller = caller;
                this->impl->setCaller(caller);
            }
            return ready();
        }

        bool setImplementation(interface::OperationRepositoryPart* orp, ExecutionEngine* caller = 0) {
#ifdef ORO_REMOTING
            this->mcaller = caller;
            *this = new RemoteMethod<Signature>( orp, caller);
            return ready();
#else
            log(Error) <<"Could not set implementation of Method "<< mname <<": ORO_REMOTING disabled at compile time."<<endlog();
            return false;
#endif
        }

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

#ifdef ORO_TEST_METHOD
    /**
     * Create a Method which executes a function locally.
     *
     * @param name The name of the resulting Method object
     * @param method A pointer to a member function to be executed.
     * @param object A pointer to the object which has the above member function.
     */
    template<class F, class O>
    Method< typename internal::UnMember<F>::type > method(std::string name, F method, O object, ExecutionEngine* ee = 0, ExecutionEngine* caller = 0) {
        return Method<  typename internal::UnMember<F>::type >(name, method, object, ee, caller);
    }

    /**
     * Create a Method which executes a function locally.
     *
     * @param name The name of the resulting Method object
     * @param method A pointer to a function to be executed.
     */
    template<class F>
    Method<F> method(std::string name, F method, ExecutionEngine* ee = 0, ExecutionEngine* caller = 0) {
        return Method<F>(name, method, ee, caller);
    }
    /**
     * Create a Method which executes a function locally.
     *
     * @param name The name of the resulting Method object
     * @param method A pointer to a function to be executed.
     */
    template<class F>
    Method< typename internal::ArgMember<F>::type > method_ds(std::string name, F method, ExecutionEngine* ee = 0, ExecutionEngine* caller = 0) {
        return Method<  typename internal::ArgMember<F>::type >(name, method, ee, caller);
    }
#endif
}

#endif
