#ifndef ORO_OPERATION_REPOSITORY_HPP
#define ORO_OPERATION_REPOSITORY_HPP

#include <vector>
#include <map>
#include <string>

#include "../base/DataSourceBase.hpp"
#include "ArgumentDescription.hpp"
#include "FactoryExceptions.hpp"
#include "../base/DisposableInterface.hpp"

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
            virtual ~OperationRepositoryPart();

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
             * Returns the number of collectable arguments of this operation.
             * @see produceCollect
             */
            virtual unsigned int collectArity() const = 0;

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
             * reference arguments.
             * @param blocking Set to true to do a blocking collect, false for a polling version.
             * @return A DataSource which collects the results when evaluated and which returns
             * the SendStatus.
             * @throw wrong_number_of_args_exception
             * @throw wrong_types_of_args_exception
             * @throw name_not_found_exception
             */
            virtual base::DataSourceBase::shared_ptr produceCollect(const std::vector<base::DataSourceBase::shared_ptr>& args, bool blocking) const = 0;

            /**
             * Returns any local operation associated with this operation.
             * @return null if no such operation exists, the operation's implementation
             * otherwise.
             */
            virtual boost::shared_ptr<base::DisposableInterface> getLocalOperation() const;
        };
        /**
         * @}
         */

        /**
         * @brief This factory is a template for creating operations.
         * @todo Rename to interface::OperationRepository.
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
            void clear();

            /**
             * Get a list of all the names of the added operations.
             */
            std::vector<std::string> getNames() const;

            /**
             * Query if an operation is present.
             */
            bool hasMember(const std::string& name) const;

            /**
             * Query the number of arguments of an operation.
             * These are the number of function arguments, not counting the return value.
             *
             * @param name The name of the operation
             *
             * @return The arity, or -1 if \a name is not found.
             */
            int getArity(const std::string& name) const;

            /**
             * Query the collectable number of arguments of an operation.
             * These are the return value (if non void) and each non-const reference argument
             * of the operation.
             *
             * @param name The name of the operation
             *
             * @return The arity, or -1 if \a name is not found.
             */
            int getCollectArity(const std::string& name) const;

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
            base::DataSourceBase::shared_ptr produce(const std::string& name, const Arguments& args, ExecutionEngine* caller) const;

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
            base::DataSourceBase::shared_ptr produceSend(const std::string& name, const Arguments& args, ExecutionEngine* caller) const;

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
            base::DataSourceBase::shared_ptr produceHandle(const std::string& name) const;

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
            base::DataSourceBase::shared_ptr produceCollect(const std::string& name, const Arguments& args, bool blocking) const;

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
            Descriptions getArgumentList(const std::string& name) const;

            /**
             * Get the type name of the result type of an operation.
             *
             * @param name The name of the operation
             *
             * @return A name of a data type.
             * @throw name_not_found_exception
             */
            std::string getResultType(const std::string& name) const;

            /**
             * Get the description of an operation
             *
             * @param name The name of the operation
             *
             * @return A user readable description.
             * @throw name_not_found_exception
             */
            std::string getDescription(const std::string& name) const;

            /**
             * Add a new operation to the interface or replace an existing one.
             *
             * @param name The name of the operation
             * @param part A part which creates the operation.
             */
            void add(const std::string& name, OperationRepositoryPart* part);

            /**
             * Remove an added operation from the interface
             *
             * @param name The name of the operation
             */
            void remove(const std::string& name);

            /**
             * Get a previously added part of this factory.
             * @param name
             * @return
             */
            OperationRepositoryPart* getPart(const std::string& name);
        };
    }
}

#endif
