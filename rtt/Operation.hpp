#ifndef ORO_RTT_OPERATION_HPP_
#define ORO_RTT_OPERATION_HPP_

#include <vector>
#include <string>
#include <boost/fusion/include/vector.hpp>
#include "base/OperationBase.hpp"
#include "internal/LocalMethod.hpp"

namespace RTT
{
    namespace bf=boost::fusion;
    /**
     * The operation ties a C or C++ function into a component interface.
     * An operation consists of a name and a Signature.
     */
    template<class Signature>
    class Operation
    : public base::OperationBase
    {
        typename internal::LocalMethod<Signature>::shared_ptr impl;
        virtual void ownerUpdated() {
            impl->setExecutor( this->mowner );
        }
    public:
        Operation(const std::string& name)
        :OperationBase(name)
        {
        }

        Operation(const std::string& name, boost::function<Signature> func, base::OperationBase::ExecutionThread et = base::OperationBase::ClientThread )
        :OperationBase(name)
        {
            this->calls(func,et);
        }

        template<class Function, class Object>
        Operation(const std::string& name, Function func, Object o, base::OperationBase::ExecutionThread et = base::OperationBase::ClientThread )
        :OperationBase(name)
        {
            this->calls(func, o, et);
        }

        ~Operation()
        {
        }

        Operation<Signature>& doc(const std::string& description) { mdoc(description); return *this; }

        Operation<Signature>& arg(const std::string& name, const std::string& description) { marg(name, description); return *this; }

        Operation& calls(boost::function<Signature> func, base::OperationBase::ExecutionThread et = base::OperationBase::ClientThread ) {
            // creates a Local Method
            impl.reset( new internal::LocalMethod<Signature>(func, this->mowner,0, et) );
            return *this;
        }

        template<class Function, class Object>
        Operation& calls(Function func, Object o, base::OperationBase::ExecutionThread et = base::OperationBase::ClientThread ) {
            // creates a Local Method or sets function
            impl.reset( new internal::LocalMethod<Signature>(func,o, this->mowner,0, et) );
            return *this;
        }

        Operation& signals(boost::function<Signature> func) {
            // attaches a signal to a Local Method
            if (!impl)
                impl.reset( new internal::LocalMethod<Signature>() );
            //impl->signal(func);
            return *this;
        }

        template<class Function, class Object>
        Operation& signals(Function func, Object o) {
            // attaches a signal to a Local Method
            if (!impl)
                impl.reset( new internal::LocalMethod<Signature>() );
            //impl->signal(func, o);
            return *this;
        }

        virtual base::DisposableInterface::shared_ptr getImplementation() { return impl; }
        virtual const base::DisposableInterface::shared_ptr getImplementation() const { return impl; }

        virtual typename base::MethodBase<Signature>::shared_ptr getMethod() { return impl; }
        virtual const typename base::MethodBase<Signature>::shared_ptr getMethod() const { return impl; }
};

}

#endif /* OPERATION_HPP_ */
