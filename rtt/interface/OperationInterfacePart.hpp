#ifndef ORO_OPERATIONREPOSITORYPART_HPP_
#define ORO_OPERATIONREPOSITORYPART_HPP_

#include <string>
#include <vector>

#include "../base/DataSourceBase.hpp"
#include "../internal/DataSource.hpp"
#include "ArgumentDescription.hpp"


namespace RTT
{
    namespace interface
    {

        /**
         * This class defines the interface for creating operation
         * objects without using C++ templates.
         *
         * The OperationInterface offers the same interface as this class,
         * but with operation names as first argument of each function.
         *
         * It is most used in conjunction with the OperationCallerC/SendHandleC
         * classes to create objects that manage calling/sending methods and
         * collecting results of method invocations.
         *
         * A Part must be interpreted as one single operation of an
         * OperationInterface.
         *
         * This class serves as a base class for the template based classes
         * that implement the produce and query logic for a given operation.
         */
        class OperationInterfacePart
        {
        public:
            RTT_API virtual ~OperationInterfacePart();

            /**
             * Returns the description of this operation.
             * @return
             */
            virtual std::string description() const = 0;

            /**
             * Get a description of the desired arguments in
             * the ArgumentDescription format.
             */
            virtual std::vector<ArgumentDescription> getArgumentList() const = 0;

            /**
             * Return the result (return) type of this operation.
             */
            virtual std::string resultType() const = 0;

            /**
             * Returns the arity (number of arguments) of this operation.
             * @see produce, produceSend
             */
            virtual unsigned int arity() const = 0;

            /**
             * Returns the type information of the n'th argument, with
             * argument zero being the return value.
             * @param arg 0: get return value type, 1..arity(): get n'th argument type.
             * @return A TypeInfo Object or null if \arg is out of range
             */
            virtual const types::TypeInfo* getArgumentType(unsigned int arg) const = 0;

            /**
             * Returns the number of collectable arguments of this operation's function.
             * These consist of the return value and the reference arguments.
             * @see produceCollect
             */
            virtual unsigned int collectArity() const = 0;

            /**
             * Returns the type information of the n'th collectable argument.
             * The numbering of \a arg starts from 1.
             * @param arg 1..collectArity(): get n'th argument type.
             * @return A TypeInfo Object or null if \arg is out of range
             */
            virtual const types::TypeInfo* getCollectType(unsigned int arg) const = 0;

            /**
             * Create a DataSource for a given callable operation.
             * @param args The arguments for the target object's function.
             * The number of arguments must be identical to this->arity()
             * @param caller The Engine that will receive notifications when
             * the method has been executed, in case it runs in the owner's thread.
             * Normally, this is the engine of the caller's TaskContext.
             * @return a DataSource which will return the result of this operation.
             * @throw wrong_number_of_args_exception
             * @throw wrong_types_of_args_exception
             * @throw name_not_found_exception
             */
            virtual base::DataSourceBase::shared_ptr produce(const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller) const = 0;

            /**
             * Create a DataSource for a given send operation.
             * @param args The arguments for the target object's function.
             * The number of arguments must be identical to this->arity()
             * @param caller The Engine that will receive notifications when
             * the method has been executed.
             * Normally, this is the engine of the caller's TaskContext.
             * @return a DataSource which contains the SendHandle of this operation.
             * @see produceHandle
             * @throw wrong_number_of_args_exception
             * @throw wrong_types_of_args_exception
             * @throw name_not_found_exception
             * @throw no_asynchronous_operation_exception
             */
            virtual base::DataSourceBase::shared_ptr produceSend(const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller) const = 0;

            /**
             * Create an empty SendHandle object for this operation.
             * @return a DataSource which contains an empty SendHandle.
             * @throw wrong_number_of_args_exception
             * @throw wrong_types_of_args_exception
             * @throw name_not_found_exception
             * @throw no_asynchronous_operation_exception
             */
            virtual base::DataSourceBase::shared_ptr produceHandle() const = 0;

            /**
             * Create a DataSource for collecting the results of a Send.
             * @param args A vector of data sources of which the first element
             * contains a properly initialised sendhandle and the remainder of the
             * elements contains datasources for collecting the return value and
             * reference arguments. The total number of items in args must be collectArity() + 1
             * @param blocking Set to true to do a blocking collect, false for a polling version.
             * @return A DataSource which collects the results when evaluated and which returns
             * the SendStatus.
             * @throw wrong_number_of_args_exception
             * @throw wrong_types_of_args_exception
             * @throw name_not_found_exception
             * @throw no_asynchronous_operation_exception
             */
            virtual base::DataSourceBase::shared_ptr produceCollect(const std::vector<base::DataSourceBase::shared_ptr>& args, internal::DataSource<bool>::shared_ptr blocking) const = 0;

            /**
             * Attach a Signal Handle to this operation which fills in the given data sources and
             * executes a given function.
             * @param func The function to execute
             * @param args The data sources to fill in with the arguments the operation was given.
             * The number of elements and their type must match the signature of the operation.
             * @return A valid Signal Handle if the arguments were valid.
             * @throw no_asynchronous_operation_exception
             */
            virtual Handle produceSignal( base::ActionInterface* func, const std::vector<base::DataSourceBase::shared_ptr>& args) const = 0;

            /**
             * Returns any local operation associated with this operation.
             * @return null if no such operation exists, the operation's implementation
             * otherwise.
             */
            RTT_API virtual boost::shared_ptr<base::DisposableInterface> getLocalOperation() const;
        };
    }
}
#endif /* ORO_OPERATIONREPOSITORYPART_HPP_ */
