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
    public:
        Operation(const std::string& name)
        :OperationBase(name)
        {
        }

        ~Operation()
        {
        }

        Operation<Signature>& doc(const std::string& description) { mdoc(description); return *this; }

        Operation<Signature>& arg(const std::string& name, const std::string& description) { marg(name, description); return *this; }

        Operation& calls(boost::function<Signature> func) {
            // creates a Local Method
        }

        template<class Function, class Object>
        Operation& calls(Function func, Object) {
            // creates a Local Method or sets function
        }

        Operation& signals(boost::function<Signature> func) {
            // attaches a signal to a Local Method

        }

        template<class Function, class Object>
        Operation& signals(Function func, Object) {
            // attaches a signal to a Local Method
        }

        virtual base::DisposableInterface::shared_ptr getImplementation() { return impl; }
        virtual const base::DisposableInterface::shared_ptr getImplementation() const { return impl; }
    };

}

#endif /* OPERATION_HPP_ */
