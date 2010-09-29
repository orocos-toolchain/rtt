/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  OperationInterface.hpp

                        OperationInterface.hpp -  description
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


#ifndef ORO_OPERATION_REPOSITORY_HPP
#define ORO_OPERATION_REPOSITORY_HPP

#include <string>
#include <vector>
#include <map>

#include "rtt-config.h"
#include "base/DataSourceBase.hpp"
#include "internal/DataSource.hpp"
#include "ArgumentDescription.hpp"
#include "FactoryExceptions.hpp"
#include "OperationInterfacePart.hpp"


namespace RTT
{
    /**
     * @brief Holds all exported operations of a component and is
     * able to produce callers for these operations.
     */
    class RTT_API OperationInterface
    {
    protected:
        typedef std::map<std::string, OperationInterfacePart*> map_t;
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
         * Query if a given operation is limited to sychronous invocation (own component thread)
         * only.
         * @return false if name not found or a normal operation, true if name exists but is synchronous.
         */
        bool isSynchronous(const std::string& name) const;

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
         * @throw no_asynchronous_operation_exception
         *
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
         * @throw no_asynchronous_operation_exception
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
         * @throw no_asynchronous_operation_exception
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
         * @throw no_asynchronous_operation_exception
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
        void add(const std::string& name, OperationInterfacePart* part);

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
        OperationInterfacePart* getPart(const std::string& name);
    };
}

#endif
