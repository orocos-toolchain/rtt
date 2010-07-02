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
#include "OperationRepositoryPart.hpp"


namespace RTT
{
    namespace interface
    {

        /**
         * @brief Holds all exported operations of a component and is
         * able to produce callers for these operations.
         */
        class RTT_API OperationRepository
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
            base::DataSourceBase::shared_ptr produceCollect(const std::string& name, const Arguments& args, internal::DataSource<bool>::shared_ptr blocking) const;

            /**
             * Attach a Signal Handle to this operation which fills in the given data sources and
             * executes a given function.
             *
             * @param name The name of the operation
             * @param func The function to execute
             * @param args The data sources to fill in with the arguments the operation was given.
             * The number of elements and their type must match the signature of the operation.
             *
             * @return a new Signal Handle
             * @throw wrong_number_of_args_exception
             * @throw wrong_types_of_args_exception
             * @throw name_not_found_exception
             */
            Handle produceSignal(const std::string& name, base::ActionInterface* func, const std::vector<base::DataSourceBase::shared_ptr>& args) const;

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
