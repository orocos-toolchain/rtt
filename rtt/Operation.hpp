#ifndef ORO_RTT_OPERATION_HPP_
#define ORO_RTT_OPERATION_HPP_

#include <vector>
#include <string>
#include <boost/fusion/include/vector.hpp>
#include "base/OperationBase.hpp"
#include "internal/LocalOperationCaller.hpp"
#include "internal/Signal.hpp"
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
         */
        Operation(const std::string& name)
        :OperationBase(name)
        {
            // set null implementation such that we can
            ExecutionEngine* null_e = 0;
            impl = boost::make_shared<internal::LocalOperationCaller<Signature> >( boost::function<Signature>(), this->mowner, null_e, OwnThread);
        }

        /**
         * Create an operation object with a name and function to execute.
         * @param name The name of this instance.
         * @param func The function to execute, for example &foo.
         * @param et The thread that should execute the function when the operation is invoked.
         */
        Operation(const std::string& name, boost::function<Signature> func, ExecutionThread et = ClientThread )
        :OperationBase(name)
        {
            this->calls(func,et);
        }

        /**
         * Create an operation object with a name and function to execute and add
         * it to a service.
         * This constructor takes an owner which is a pointer (or shared pointer) to
         * a service or object to which this operation must be added.
         * @param name The name of this instance.
         * @param func The function to execute, for example &foo.
         * @param et The thread that should execute the function when the operation is invoked.
         * @param owner The owner object that has a method addOperation(OperationBase*)
         */
        template<class Owner>
        Operation(const std::string& name, boost::function<Signature> func, ExecutionThread et, Owner owner )
        :OperationBase(name)
        {
            this->calls(func,et);
            owner->addOperation(this);
        }

        /**
         * Create an operation object with a name and class member function to execute.
         * @param name The name of this instance.
         * @param func The function to execute, for example &Bar::foo
         * @param o The object that has this function, for example &bar
         * @param et The thread that should execute the function when the operation is invoked.
         */
        template<class Function, class Object>
        Operation(const std::string& name, Function func, Object o, ExecutionThread et = ClientThread )
        :OperationBase(name)
        {
            this->calls(func, o, et);
        }

        /**
         * Create an operation object with a name and class member function to execute and add
         * it to a service.
         * This constructor takes an owner which is a pointer (or shared pointer) to
         * a service or object to which this operation must be added.
         * @param name The name of this instance.
         * @param func The function to execute, for example &Bar::foo
         * @param o The object that has this function, for example &bar
         * @param et The thread that should execute the function when the operation is invoked.
         */
        template<class Function, class Object, class Owner>
        Operation(const std::string& name, Function func, Object o, ExecutionThread et, Owner owner )
        :OperationBase(name)
        {
            this->calls(func, o, et);
            owner->addOperation(this);
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
         * @return A reference to this object.
         */
        Operation& calls(boost::function<Signature> func, ExecutionThread et = ClientThread ) {
            // creates a Local OperationCaller
            ExecutionEngine* null_e = 0;
            impl = boost::make_shared<internal::LocalOperationCaller<Signature> >(func, this->mowner, null_e, et);
            if (signal)
                impl->setSignal(signal);
            return *this;
        }

        /**
         * Indicate that this operation calls a given class member function.
         * This will replace any previously registered function present in this operation.
         * @param func The function to call when the operation is invoked, for example &Bar::foo
         * @param o The object that has this function, for example &bar
         * @param et The thread that should execute the function when the operation is invoked.
         * @return A reference to this object.
         */
        template<class Function, class Object>
        Operation& calls(Function func, Object o, ExecutionThread et = ClientThread ) {
            // creates a Local OperationCaller or sets function
            ExecutionEngine* null_e = 0;
            impl = boost::make_shared<internal::LocalOperationCaller<Signature> >(func, o, this->mowner, null_e, et);
            if (signal)
                impl->setSignal(signal);
            return *this;
        }

        /**
         * Indicate that this operation signals a given function.
         * @param func The function to call after the operation is executed
         * @return A reference to this object.
         */
        Handle signals(boost::function<Signature> func) {
            // attaches a signal to a Local OperationCaller
            ExecutionEngine* null_e = 0;
            if (!impl)
                impl = boost::make_shared<internal::LocalOperationCaller<Signature> >( boost::function<Signature>(), this->mowner, null_e, OwnThread);
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

        virtual base::DisposableInterface::shared_ptr getImplementation() { return impl; }
        virtual const base::DisposableInterface::shared_ptr getImplementation() const { return impl; }

        virtual typename base::OperationCallerBase<Signature>::shared_ptr getOperationCaller() { return impl; }
        virtual const typename base::OperationCallerBase<Signature>::shared_ptr getOperationCaller() const { return impl; }

        /**
         * You can signal this operation instead of calling it.
         * This triggers all attached signal handlers added with signals().
         * @note Since signal is a shared_ptr, it may be null, so you need
         * to check it first.
         */
        typename internal::Signal<Signature>::shared_ptr signal;
    private:
        typename internal::LocalOperationCaller<Signature>::shared_ptr impl;
        virtual void ownerUpdated() {
            impl->setExecutor( this->mowner );
        }
    };

}

#endif /* OPERATION_HPP_ */
