/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  OperationInterfacePart.hpp

                        OperationInterfacePart.hpp -  description
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


#ifndef ORO_OPERATIONREPOSITORYPART_HPP_
#define ORO_OPERATIONREPOSITORYPART_HPP_

#include <string>
#include <vector>

#include "rtt/base/DataSourceBase.hpp"
#include "rtt/internal/DataSource.hpp"
#include "rtt/ArgumentDescription.hpp"


namespace RTT
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
         * Returns the name of this operation.
         * @return
         */
         virtual std::string getName() const = 0;

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

#ifdef ORO_SIGNALLING_OPERATIONS
        /**
         * Attach a Signal Handle to this operation which fills in the given data sources and
         * executes a given function.
         * @param func The function to execute
         * @param args The data sources to fill in with the arguments the operation was given.
         * The number of elements and their type must match the signature of the operation.
         * @param subscriber The execution engine of the Task which wishes to receive the signal.
         * The action \a func will be executed in this engine. Set to zero to use the engine of
         * the ExecutionType of the Operation (OwnThread or ClientThread).
         * @return A valid Signal Handle if the arguments were valid.
         * @throw no_asynchronous_operation_exception
         */
        virtual Handle produceSignal( base::ActionInterface* func, const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* subscriber) const = 0;
#endif
        /**
         * Returns any local operation associated with this operation.
         * @return null if no such operation exists, the operation's implementation
         * otherwise.
         */
        RTT_API virtual boost::shared_ptr<base::DisposableInterface> getLocalOperation() const;
    };
}
#endif /* ORO_OPERATIONREPOSITORYPART_HPP_ */
