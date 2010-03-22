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
#include "interface/ServiceProvider.hpp"
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
     * the completion message. In case the caller has no ExecutionEngine, the
     * GlobalExecutionEngine is used instead.
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
         * Initialise a nameless Method object from a local Operation.
         *
         * @param implementation The implementation of the operation which is to be used
         * by the Method object.
         * @param caller The ExecutionEngine which will be used to call us
         * back in case of asynchronous communication. If zero, the global Engine is used.
         */
        Method(boost::shared_ptr<base::DisposableInterface> implementation, ExecutionEngine* caller = 0)
            : Base( boost::dynamic_pointer_cast< base::MethodBase<Signature> >(implementation) ),
              mname(), mcaller(caller)
        {
            if ( !this->impl && implementation ) {
                log(Error) << "Tried to construct Method from incompatible local operation."<< endlog();
            } else
                if (this->impl)
                    this->impl->setCaller(mcaller);
        }

        /**
         * Initialise a nameless Method object from an operation factory.
         *
         * @param part The OperationRepositoryPart which is used
         * by the Method object to get the implementation.
         * @param caller The ExecutionEngine which will be used to call us
         * back in case of asynchronous communication. If zero, the global Engine is used.
         */
        Method(interface::OperationRepositoryPart* part, ExecutionEngine* caller = 0)
            : Base(),
              mname(), mcaller(caller)
        {
            if (part) {
                 this->impl = boost::dynamic_pointer_cast< base::MethodBase<Signature> >( part->getLocalOperation() );
                 setupMethod( part );
            }
        }

        /**
         * Initialise a named Method object from a Service.
         *
         * @param name The name of the operation to look for.
         * @param service The ServiceProvider where the operation will be looked up.
         * @param caller The ExecutionEngine which will be used to call us
         * back in case of asynchronous communication. If zero, the global Engine is used.
         */
        Method(const std::string& name, interface::ServiceProviderPtr service, ExecutionEngine* caller = 0)
            : Base(),
              mname(name), mcaller(caller)
        {
            if (service) {
                 this->impl = boost::dynamic_pointer_cast< base::MethodBase<Signature> >( service->getLocalOperation(name) );
                 if (service->hasMember(name))
                     setupMethod( service->getOperation(name) );
            }
        }

        /**
         * Method objects may be assigned to an implementation.
         *
         * @param implementation the implementation.
         *
         * @return *this
         */
        Method& operator=(boost::shared_ptr<base::DisposableInterface> implementation)
        {
            if (this->impl && this->impl == implementation)
                return *this;
            Method<Signature> tmp(implementation);
            if (tmp.ready())
                *this = tmp;
            return *this;
        }

        /**
         * Method objects may be assigned to a part responsible for production
         * of an implementation.
         *
         * @param part The part used by the Method to produce an implementation.
         *
         * @return *this
         */
        Method& operator=(interface::OperationRepositoryPart* part)
        {
            if (this->impl && this->impl == part->getLocalOperation() )
                return *this;
            Method<Signature> tmp(part);
            if (tmp.ready())
                *this = tmp;
            return *this;
        }

        /**
         * Named Method objects may be looked up in a Service.
         *
         * @param service The ServiceProvider where the operation will be looked up.
         *
         * @return *this
         */
        Method& operator=(interface::ServiceProviderPtr service)
        {
            if (this->mname.empty()) {
                log(Error) << "Can't initialise unnamed Method from service '"<<service->getName() <<"'."<<endlog();
                return *this;
            }
            Method<Signature> tmp(mname,service);
            if (tmp.ready())
                *this = tmp;
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

        bool setImplementationPart(interface::OperationRepositoryPart* orp, ExecutionEngine* caller = 0) {
            Method<Signature> tmp(orp, caller);
            if (tmp.ready())
                *this = tmp;
            return tmp.ready();
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

        void setCaller(ExecutionEngine* caller) {
            this->mcaller = caller;
            if (this->impl)
                this->impl->setCaller(caller);
        }
    protected:
        /**
         * If no local implementation of an operation could be found,
         * this method tries it using the operationrepository factories.
         * @param part
         */
        void setupMethod(interface::OperationRepositoryPart* part) {
            if ( !this->impl ) {
#ifdef ORO_REMOTING
                // try differently
                this->impl.reset( new internal::RemoteMethod<Signature>( part, mname, mcaller ));
                if (this->impl->ready())
                    this->impl->setCaller(mcaller);
                else {
                    this->impl.reset(); // clean up.
                    log(Error) << "Tried to construct Method from incompatible operation '"<< mname<<"'."<< endlog();
                }
#else
                log(Error) << "Tried to construct remote Method but ORO_REMOTING was disabled."<< endlog();
#endif
            } else
                this->impl->setCaller(mcaller);
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
