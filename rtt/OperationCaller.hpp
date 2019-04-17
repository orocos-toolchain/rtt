/***************************************************************************
  tag: FMTC  do nov 2 13:06:07 CET 2006  OperationCaller.hpp

                        OperationCaller.hpp -  description
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


#ifndef ORO_TASK_OPERATION_CALLER_HPP
#define ORO_TASK_OPERATION_CALLER_HPP

#include <string>
#include <boost/function.hpp>
#include "base/OperationCallerBase.hpp"
#ifdef ORO_TEST_OPERATION_CALLER
#include "internal/UnMember.hpp"
#include "internal/LocalOperationCaller.hpp"
#endif
#include "internal/InvokerSignature.hpp"
#include "base/OperationCallerBaseInvoker.hpp"
#include "Logger.hpp"
#include "Service.hpp"
#ifdef ORO_REMOTING
#include "OperationInterface.hpp"
#include "internal/RemoteOperationCaller.hpp"
#endif

namespace RTT
{
    /**
     * @defgroup Operations Operation Interface
     * Invoking synchronous and asynchronous methods.
     * @ingroup RTTComponentInterface
     */


    /**
     * A OperationCaller serves as a placeholder (aka 'proxy') for a remote
     * Operation. If you want to call an operation, you need this class to do it.
     *
     * The OperationCaller has the exact same signature template argument as the Operation
     * it wishes to call.
     *
     * OwnThread operations need the caller's ExecutionEngine to be able to inform
     * it of completion . In case the caller has no ExecutionEngine, the
     * GlobalEngine is used instead, and the GlobalEngine will block until the call completes.
     *
     * @ingroup RTTComponentInterface
     * @ingroup Operations
     */
    template<class SignatureT>
    class OperationCaller
        : public internal::InvokerSignature<boost::function_traits<SignatureT>::arity,
                                          SignatureT,
                                          boost::shared_ptr< base::OperationCallerBase<SignatureT> > >,
          public base::OperationCallerBaseInvoker
    {
        std::string mname;
        ExecutionEngine* mcaller;
    public:
        typedef SignatureT Signature;
        typedef internal::InvokerSignature<boost::function_traits<Signature>::arity,
                                         Signature,
                                         boost::shared_ptr< base::OperationCallerBase<Signature> > > Base;
        typedef typename boost::function_traits<Signature>::result_type result_type;
        typedef boost::function_traits<Signature> traits;
        typedef boost::shared_ptr< base::OperationCallerBase<Signature> > OperationCallerBasePtr;

        /**
         * Create an empty OperationCaller object.
         * Use assignment to initialise it.
         */
        OperationCaller()
            : Base(), mname(), mcaller(0)
        {}

        /**
         * Create an OperationCaller object with a name and optional caller.
         * @param name The name of the operation that will be called.
         * @param name The ExecutionEngine of the TaskContext calling the operation.
         */
        OperationCaller(std::string name, ExecutionEngine* caller = 0)
            : Base(), mname(name), mcaller(caller)
        {}

        /**
         * OperationCaller objects may be copied. A deep copy is made
         * of the implementation object such that using this
         * object does not interfere when using the original, m.
         *
         * @param m the original
         */
        OperationCaller(const OperationCaller& m)
            : Base(m.impl ? boost::shared_ptr< base::OperationCallerBase<Signature> >(m.impl->cloneI(m.mcaller)) : m.impl ),
              mname(m.mname), mcaller(m.mcaller)
        {}

        /**
         * OperationCaller objects may be assigned. A deep copy is made
         * of the implementation object such that using this
         * object does not interfere when using the original, m.
         *
         * @param m the original
         *
         * @return *this
         */
        OperationCaller& operator=(const OperationCaller& m)
        {
            if ( this == &m )
                return *this;
            mname = m.mname;
            mcaller = m.mcaller;
            if (m.impl)
                this->impl.reset( m.impl->cloneI(mcaller) );
            else
                this->impl.reset();
            return *this;
        }

        /**
         * Initialise a nameless OperationCaller object from a local Operation.
         *
         * @param implementation The implementation of the operation which is to be used
         * by the OperationCaller object. This object will be cloned such that the method uses
         * its own implementation.
         * @param caller The ExecutionEngine which will be used to call us
         * back in case of asynchronous communication. If zero, the global Engine is used.
         */
        OperationCaller(boost::shared_ptr<base::DisposableInterface> implementation, ExecutionEngine* caller = 0)
            : Base( boost::dynamic_pointer_cast< base::OperationCallerBase<Signature> >(implementation) ),
              mname(), mcaller(caller)
        {
            if ( !this->impl && implementation ) {
                log(Error) << "Tried to construct OperationCaller from incompatible local operation."<< endlog();
            } else {
                if (this->impl) {
                    this->impl.reset( this->impl->cloneI(mcaller) );
                }
            }
        }

        /**
         * Initialise a nameless OperationCaller object from an operation factory.
         *
         * @param part The OperationInterfacePart which is used
         * by the OperationCaller object to get the implementation.
         * @param caller The ExecutionEngine which will be used to call us
         * back in case of asynchronous communication. If zero, the global Engine is used.
         */
        OperationCaller(OperationInterfacePart* part, ExecutionEngine* caller = 0)
            : Base(),
              mname(), mcaller(caller)
        {
            if (part) {
                 mname = part->getName();
                 this->impl = boost::dynamic_pointer_cast< base::OperationCallerBase<Signature> >( part->getLocalOperation() );
                 setupOperationCaller( part );
            }
        }

        /**
         * Initialise a named OperationCaller object from a Service.
         *
         * @param name The name of the operation to look for.
         * @param service The Service where the operation will be looked up.
         * @param caller The ExecutionEngine which will be used to call us
         * back in case of asynchronous communication. If zero, the global Engine is used.
         */
        OperationCaller(const std::string& name, ServicePtr service, ExecutionEngine* caller = 0)
            : Base(),
              mname(name), mcaller(caller)
        {
            if (service) {
                 this->impl = boost::dynamic_pointer_cast< base::OperationCallerBase<Signature> >( service->getLocalOperation(name) );
                 if (service->hasMember(name))
                     setupOperationCaller( service->getOperation(name) );
            }
        }

        /**
         * OperationCaller objects may be assigned to an implementation.
         * This variant is used when a local implementation is available.
         *
         * @param implementation the implementation.
         *
         * @return *this
         */
        OperationCaller& operator=(boost::shared_ptr<base::DisposableInterface> implementation)
        {
            if (this->impl && this->impl == implementation)
                return *this;
            OperationCaller<Signature> tmp(implementation, mcaller);
            *this = tmp;
            return *this;
        }

        /**
         * OperationCaller objects may be assigned to a part responsible for production
         * of an implementation.
         * This variant is used when a only a remote implementation is available.
         *
         * @param part The part used by the OperationCaller to produce an implementation.
         *
         * @return *this
         */
        OperationCaller& operator=(OperationInterfacePart* part)
        {
            if (part == 0) {
                log(Warning) << "Assigning OperationCaller from null part."<<endlog();
                this->impl.reset();
            }
            if (this->impl && this->impl == part->getLocalOperation() )
                return *this;
            OperationCaller<Signature> tmp(part, mcaller);
            *this = tmp;
            return *this;
        }

        /**
         * Named OperationCaller objects may be looked up in a Service.
         *
         * @param service The Service where the operation will be looked up.
         *
         * @return *this
         */
        OperationCaller& operator=(ServicePtr service)
        {
            if ( !service ) {
                log(Warning) << "Assigning OperationCaller from null service."<<endlog();
                this->impl.reset();
            }
            if (this->mname.empty()) {
                log(Error) << "Can't initialise unnamed OperationCaller from service '"<<service->getName() <<"'."<<endlog();
                return *this;
            }
            OperationCaller<Signature> tmp(mname, service, mcaller);
            *this = tmp;
            return *this;
        }

#ifdef ORO_TEST_OPERATION_CALLER
        /**
         * Construct a OperationCaller from a class member pointer and an
         * object of that class.
         *
         * @param name The name of this method
         * @param meth A pointer to a class member function
         * @param object An object of the class which has \a meth as member function.
         * @note FOR INTERNAL TESTING ONLY. Don't use in application code.
         */
        template<class M, class ObjectType>
        OperationCaller(std::string name, M meth, ObjectType object, ExecutionEngine* ee = 0, ExecutionEngine* caller = 0, ExecutionThread et = ClientThread)
            : Base( OperationCallerBasePtr(new internal::LocalOperationCaller<Signature>(meth, object, ee, caller, et) ) ),
              mname(name), mcaller(caller)
        {}

        /**
         * Construct a OperationCaller from a function pointer or function object.
         *
         * @param name the name of this method
         * @param meth an pointer to a function or function object.
         * @note FOR INTERNAL TESTING ONLY. Don't use in application code.
         */
        template<class M>
        OperationCaller(std::string name, M meth, ExecutionEngine* ee = 0, ExecutionEngine* caller = 0, ExecutionThread et = ClientThread)
            : Base( OperationCallerBasePtr(new internal::LocalOperationCaller<Signature>(meth,ee,caller, et) ) ),
              mname(name), mcaller(caller)
        {}
#endif
#ifdef ORO_REMOTING
        /**
         * Construct a OperationCaller from an operation interface part.
         *
         * @param name the name of this method
         * @param meth an pointer to a function or function object.
         */
        template<class M>
        OperationCaller(std::string name, OperationInterfacePart* orp, ExecutionEngine* caller = 0)
            : Base( OperationCallerBasePtr(new internal::RemoteOperationCaller<Signature>(orp,caller) ) ),
              mname(name)
        {}
#endif

        /**
         * Clean up the OperationCaller object.
         */
        ~OperationCaller()
        {
        }

        /**
         * Check if this OperationCaller is ready for execution.
         *
         * @return true if so.
         */
        bool ready() const {
            return this->impl && this->impl->ready();
        }

        /**
         * Get the name of this OperationCaller.
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

        bool setImplementationPart(OperationInterfacePart* orp, ExecutionEngine* caller = 0) {
            OperationCaller<Signature> tmp(orp, caller);
            if (tmp.ready()) {
                *this = tmp;
                return true;
            }
            return false;
        }

        /**
         * Returns the internal implementation of the OperationCaller object.
         */
        const OperationCallerBasePtr getOperationCallerImpl() const {
            return this->impl;
        }

        /**
         * Sets the internal implementation of the OperationCaller object.
         */
        void setOperationCallerImpl( OperationCallerBasePtr new_impl) {
            this->impl = new_impl;
        }

        void setCaller(ExecutionEngine* caller) {
            this->mcaller = caller;
            if (this->impl)
                this->impl->setCaller(caller);
        }

        void disconnect()
        {
            this->impl.reset();
        }
    protected:
        /**
         * If no local implementation of an operation could be found,
         * this method tries it using the operationrepository factories.
         * @param part
         */
        void setupOperationCaller(OperationInterfacePart* part) {
            if ( !this->impl ) {
#ifdef ORO_REMOTING
                // try differently
                try {
                    this->impl.reset( new internal::RemoteOperationCaller<Signature>( part, mname, mcaller ));
                } catch( std::exception& e ) {
                    log(Error) << "Constructing RemoteOperationCaller for "<< mname <<" was not possible."<<endlog();
                    log(Error) << "Probable cause: " << e.what() <<endlog();
                    return;
                }
                if (this->impl->ready()) {
                    log(Debug) << "Constructed OperationCaller from remote implementation '"<< mname<<"'."<< endlog();
                } else {
                    this->impl.reset(); // clean up.
                    log(Error) << "Tried to construct OperationCaller from incompatible operation '"<< mname<<"'."<< endlog();
                }
#else
                log(Error) << "Tried to construct remote OperationCaller but ORO_REMOTING was disabled."<< endlog();
#endif
            } else {
                // finally clone and set caller on clone.
                this->impl.reset( this->impl->cloneI(mcaller) );
            }
        }

    };

#ifdef ORO_TEST_OPERATION_CALLER
    /**
     * Create a OperationCaller which executes a function locally.
     *
     * @param name The name of the resulting OperationCaller object
     * @param method A pointer to a member function to be executed.
     * @param object A pointer to the object which has the above member function.
     * @note FOR INTERNAL TESTING ONLY. Don't use in application code.
     */
    template<class F, class O>
    OperationCaller< typename internal::UnMember<F>::type > method(std::string name, F method, O object, ExecutionEngine* ee = 0, ExecutionEngine* caller = 0) {
        return OperationCaller<  typename internal::UnMember<F>::type >(name, method, object, ee, caller);
    }

    /**
     * Create a OperationCaller which executes a function locally.
     *
     * @param name The name of the resulting OperationCaller object
     * @param method A pointer to a function to be executed.
     * @note FOR INTERNAL TESTING ONLY. Don't use in application code.
     */
    template<class F>
    OperationCaller<F> method(std::string name, F method, ExecutionEngine* ee = 0, ExecutionEngine* caller = 0) {
        return OperationCaller<F>(name, method, ee, caller);
    }
    /**
     * Create a OperationCaller which executes a function locally.
     *
     * @param name The name of the resulting OperationCaller object
     * @param method A pointer to a function to be executed.
     * @note FOR INTERNAL TESTING ONLY. Don't use in application code.
     */
    template<class F>
    OperationCaller< typename internal::ArgMember<F>::type > method_ds(std::string name, F method, ExecutionEngine* ee = 0, ExecutionEngine* caller = 0) {
        return OperationCaller<  typename internal::ArgMember<F>::type >(name, method, ee, caller);
    }
#endif
}

#endif
