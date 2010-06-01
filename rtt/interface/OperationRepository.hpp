#ifndef ORO_OPERATION_REPOSITORY_HPP
#define ORO_OPERATION_REPOSITORY_HPP

#include <string>
#include <vector>
#include <map>

#include "rtt-config.h"
#include "../base/DataSourceBase.hpp"
#include "../internal/DataSource.hpp"
#include "ArgumentDescription.hpp"
#include "FactoryExceptions.hpp"


namespace RTT
{
    namespace interface
    {

        /**
         * @internal
         * @defgroup OperationRepositoryPart Base Classes for scriptable operations.
         * @brief Factories for operations.
         *
         * Each operation in the OperationRepository will
         * be linked with one OperationRepositoryPart that knows how to produce
         * that operation
         *
         *@{
         */
        class OperationRepositoryPart
        {
        public:
            RTT_API virtual ~OperationRepositoryPart();

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
             */
            virtual base::DataSourceBase::shared_ptr produceSend(const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller) const = 0;

            /**
             * Create an empty SendHandle object for this operation.
             * @return a DataSource which contains an empty SendHandle.
             * @throw wrong_number_of_args_exception
             * @throw wrong_types_of_args_exception
             * @throw name_not_found_exception
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
             */
            virtual base::DataSourceBase::shared_ptr produceCollect(const std::vector<base::DataSourceBase::shared_ptr>& args, internal::DataSource<bool>::shared_ptr blocking) const = 0;

            /**
             * Returns any local operation associated with this operation.
             * @return null if no such operation exists, the operation's implementation
             * otherwise.
             */
            RTT_API virtual boost::shared_ptr<base::DisposableInterface> getLocalOperation() const;
        };
        /**
         * @}
         */

        /**
         * @brief Holds all exported operations of a component and is
         * able to produce callers for these operations.
         */
        class OperationRepository
        {
        protected:
            typedef std::map<std::string, OperationRepositoryPart*> map_t;
            map_t data;
        public:
            /**
             * The arguments for an operation.
             */
            typedef std::vector<base::DataSourceBase::shared_ptr> Arguments;

            /**
             * Remove and delete all added operations.
             */
            RTT_API void clear();

            /**
             * Get a list of all the names of the added operations.
             */
            RTT_API std::vector<std::string> getNames() const;

            /**
             * Query if an operation is present.
             */
            RTT_API bool hasMember(const std::string& name) const;

            /**
             * Query the number of arguments of an operation.
             * These are the number of function arguments, not counting the return value.
             *
             * @param name The name of the operation
             *
             * @return The arity, or -1 if \a name is not found.
             */
            RTT_API int getArity(const std::string& name) const;

            /**
             * Query the collectable number of arguments of an operation.
             * These are the return value (if non void) and each non-const reference argument
             * of the operation.
             *
             * @param name The name of the operation
             *
             * @return The arity, or -1 if \a name is not found.
             */
            RTT_API int getCollectArity(const std::string& name) const;

            /**
             * Produce a DataSource that call()s an operation.
             * The DataSource will return the result of call().
             *
             * @param name The name of the operation
             * @param args The arguments filled in as data sources.
             *
             * @return a new object
             * @throw wrong_number_of_args_exception
             * @throw wrong_types_of_args_exception
             * @throw name_not_found_exception
             */
            RTT_API base::DataSourceBase::shared_ptr produce(const std::string& name, const Arguments& args, ExecutionEngine* caller) const;

            /**
             * Produce a DataSource that send()s an operation.
             * The DataSource will return the SendHandle of that operation.
             *
             * @param name The name of the operation
             * @param args The arguments filled in as data sources.
             *
             * @return a new object
             * @throw wrong_number_of_args_exception
             * @throw wrong_types_of_args_exception
             * @throw name_not_found_exception
             */
            RTT_API base::DataSourceBase::shared_ptr produceSend(const std::string& name, const Arguments& args, ExecutionEngine* caller) const;

            /**
             * Produce an AssignableDataSource that contains a SendHandle,
             * fit for the operation.
             * The DataSource will return the SendHandle.
             *
             * @param name The name of the operation
             * @param args The arguments filled in as data sources.
             *
             * @return a new object
             * @throw wrong_number_of_args_exception
             * @throw wrong_types_of_args_exception
             * @throw name_not_found_exception
             */
            RTT_API base::DataSourceBase::shared_ptr produceHandle(const std::string& name) const;

            /**
             * Produce a DataSource that collects a sent operation,
             * The DataSource will return the SendStatus and store
             * the results in the presented arguments. Note that this
             * function takes most of the time less arguments than its companions.
             *
             * @param name The name of the operation
             * @param args The arguments filled in as data sources.
             * @param blocking Set to true to block on the result.
             *
             * @return a new object
             * @throw wrong_number_of_args_exception
             * @throw wrong_types_of_args_exception
             * @throw name_not_found_exception
             */
            RTT_API base::DataSourceBase::shared_ptr produceCollect(const std::string& name, const Arguments& args, internal::DataSource<bool>::shared_ptr blocking) const;

            /**
             * The descriptions of an argumentlist.
             */
            typedef std::vector<ArgumentDescription> Descriptions;

            /**
             * Get the names and descriptions of all arguments of an operation.
             *
             * @param name The name of the operation
             *
             * @return A list of descriptions.
             * @throw name_not_found_exception
             */
            RTT_API Descriptions getArgumentList(const std::string& name) const;

            /**
             * Get the type name of the result type of an operation.
             *
             * @param name The name of the operation
             *
             * @return A name of a data type.
             * @throw name_not_found_exception
             */
            RTT_API std::string getResultType(const std::string& name) const;

            /**
             * Get the description of an operation
             *
             * @param name The name of the operation
             *
             * @return A user readable description.
             * @throw name_not_found_exception
             */
            RTT_API std::string getDescription(const std::string& name) const;

            /**
             * Add a new operation to the interface or replace an existing one.
             *
             * @param name The name of the operation
             * @param part A part which creates the operation.
             */
            RTT_API void add(const std::string& name, OperationRepositoryPart* part);

            /**
             * Remove an added operation from the interface
             *
             * @param name The name of the operation
             */
            RTT_API void remove(const std::string& name);

            /**
             * Get a previously added part of this factory.
             * @param name
             * @return
             */
            RTT_API OperationRepositoryPart* getPart(const std::string& name);
        };
    }
}

#endif
