/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  Operation.hpp

                        Operation.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#ifndef ORO_RTT_OPERATION_HPP_
#define ORO_RTT_OPERATION_HPP_

#include <vector>
#include <string>
#include <boost/fusion/include/vector.hpp>
#include "base/OperationBase.hpp"
#include "internal/LocalOperationCaller.hpp"
#ifdef ORO_SIGNALLING_OPERATIONS
#include "internal/Signal.hpp"
#endif
#include "internal/OperationCallerBinder.hpp"

namespace RTT
{
    /**
     * The operation ties a C or C++ function into a component interface.
     * An operation consists of a name and a Signature.
     *
     * For example, if you want his operation to call a function:
     * bool foo(int a, double b) {
     *    return a < b;
     * }
     * Then the function Signature looks like: 'bool(int,double)' which is the signature type of
     * 'bool foo(int a, double b)'. So a function signature is written as the function
     * declaration, but without the function and argument names.
     *
     * @param Signature A template parameter that contains the function signature type
     * of the function this operation object will call.
     * @ingroup Operations
     */
    template<class Signature>
    class Operation
    : public base::OperationBase
    {
    public:
        /**
         * Create an operation object with a name.
         * The resulting object will not call any function but can be
         * used to attach Signal handlers to (using signals() ) or
         * set the function to call later on with calls().
         * @param name The name of this instance.
         * @post The resulting operation will have ClientThread execution semantics.
         */
        Operation(const std::string& name)
        :OperationBase(name)
        {
            // set null implementation such that we can already add it to the interface and register signals.
            ExecutionEngine* null_e = 0;
            impl = boost::make_shared<internal::LocalOperationCaller<Signature> >( boost::function<Signature>(), null_e, null_e, ClientThread);
        }

        /**
         * Create an operation object with a name and function to execute.
         * @param name The name of this instance.
         * @param func The function to execute, for example &foo.
         * @param et The thread that should execute the function when the operation is invoked.
         * @param ownerEngine the execution engine of the owner of this operation if any. Will be automatically set when you use Service::addOperation().
         */
        Operation(const std::string& name, boost::function<Signature> func, ExecutionThread et = ClientThread, ExecutionEngine* ownerEngine = NULL )
        :OperationBase(name)
        {
            this->calls(func, et, ownerEngine);
        }

        /**
         * Create an operation object with a name and class member function to execute.
         * @param name The name of this instance.
         * @param func The function to execute, for example &Bar::foo
         * @param o The object that has this function, for example &bar
         * @param et The thread that should execute the function when the operation is invoked.
         * @param ownerEngine the execution engine of the owner of this operation if any. Will be automatically set when you use Service::addOperation().
         */
        template<class Function, class Object>
        Operation(const std::string& name, Function func, Object o, ExecutionThread et = ClientThread, ExecutionEngine* ownerEngine = NULL )
        :OperationBase(name)
        {
            this->calls(func, o, et, ownerEngine);
        }

        ~Operation()
        {
        }

        /**
         * Document this operation.
         * @param description A descriptive line of text.
         * @return A reference to this object.
         */
        Operation<Signature>& doc(const std::string& description) { mdoc(description); return *this; }

        /**
         * Document an argument of this operation. Each time arg() is called, it documents
         * the next argument.
         * @param name A single word (identifier) denoting the name of this argument
         * @param description A descriptive line of text for this argument
         * @return A reference to this object.
         */
        Operation<Signature>& arg(const std::string& name, const std::string& description) { marg(name, description); return *this; }

        /**
         * Indicate that this operation calls a given function.
         * This will replace any previously registered function present in this operation.
         * @param func The function to call when the operation is invoked
         * @param et The thread that should execute the function when the operation is invoked.
         * @param ownerEngine the execution engine of the owner of this operation if any.
         * @return A reference to this object.
         */
        Operation& calls(boost::function<Signature> func, ExecutionThread et = ClientThread, ExecutionEngine* ownerEngine = NULL ) {
            // creates a Local OperationCaller
            ExecutionEngine* null_caller = 0;
            impl = boost::make_shared<internal::LocalOperationCaller<Signature> >(func, ownerEngine ? ownerEngine : this->mowner, null_caller, et);
#ifdef ORO_SIGNALLING_OPERATIONS
            if (signal)
                impl->setSignal(signal);
#endif
            return *this;
        }

        /**
         * Indicate that this operation calls a given class member function.
         * This will replace any previously registered function present in this operation.
         * @param func The function to call when the operation is invoked, for example &Bar::foo
         * @param o The object that has this function, for example &bar
         * @param et The thread that should execute the function when the operation is invoked.
         * @param ownerEngine the execution engine of the owner of this operation if any.
         * @return A reference to this object.
         */
        template<class Function, class Object>
        Operation& calls(Function func, Object o, ExecutionThread et = ClientThread, ExecutionEngine* ownerEngine = NULL ) {
            // creates a Local OperationCaller or sets function
            ExecutionEngine* null_caller = 0;
            impl = boost::make_shared<internal::LocalOperationCaller<Signature> >(func, o, ownerEngine ? ownerEngine : this->mowner, null_caller, et);
#ifdef ORO_SIGNALLING_OPERATIONS
            if (signal)
                impl->setSignal(signal);
#endif
            return *this;
        }

#ifdef ORO_SIGNALLING_OPERATIONS

        /**
         * Call this function on an empty Operation object, in order to make it a signalling
         * operation without implementation. This function is automatically called when you
         * add this object to a Service with addEventOperation().
         * If a signal function is already set, this function does nothing.
         */
        void signals() {
            // attaches a signal to a Local OperationCaller
            ExecutionEngine* null_caller = 0;
            if (!impl)
                impl = boost::make_shared<internal::LocalOperationCaller<Signature> >( boost::function<Signature>(), this->mowner, null_caller, ClientThread);
            if (!signal) {
                signal = boost::make_shared<internal::Signal<Signature> >();
                impl->setSignal( signal );
            }
        }

        /**
         * Indicate that this operation signals a given function.
         * @param func The function to call after the operation is executed
         * @return A reference to this object.
         */
        Handle signals(boost::function<Signature> func) {
            // attaches a signal to a Local OperationCaller
            ExecutionEngine* null_caller = 0;
            if (!impl)
                impl = boost::make_shared<internal::LocalOperationCaller<Signature> >( boost::function<Signature>(), this->mowner, null_caller, ClientThread);
            if (!signal) {
                signal = boost::make_shared<internal::Signal<Signature> >();
                impl->setSignal( signal );
            }
            return signal->connect( func );
        }

        /**
         * Indicate that this operation signals a given function.
         * @param func The function to call when the operation is invoked, for example &Bar::foo
         * @param o The object that has this function, for example &bar
         * @return A reference to this object.
         */
        template<class Function, class Object>
        Handle signals(Function func, Object o) {
            return this->signals( internal::OperationCallerBinder<Signature>()(func, o) );
        }
#endif
        virtual base::DisposableInterface::shared_ptr getImplementation() { return impl; }
        virtual const base::DisposableInterface::shared_ptr getImplementation() const { return impl; }

        virtual typename base::OperationCallerBase<Signature>::shared_ptr getOperationCaller() { return impl; }
        virtual const typename base::OperationCallerBase<Signature>::shared_ptr getOperationCaller() const { return impl; }

#ifdef ORO_SIGNALLING_OPERATIONS
        /**
         * You can signal this operation instead of calling it.
         * This triggers all attached signal handlers added with signals().
         * @note Since signal is a shared_ptr, it may be null, so you need
         * to check it first.
         */
        typename internal::Signal<Signature>::shared_ptr signal;
#endif
    private:
        typename internal::LocalOperationCaller<Signature>::shared_ptr impl;
        virtual void ownerUpdated() {
            if (impl)
                impl->setOwner( this->mowner );
        }
    };

}

#endif /* OPERATION_HPP_ */
