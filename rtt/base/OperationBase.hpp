/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  OperationBase.hpp

                        OperationBase.hpp -  description
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


#ifndef ORO_OPERATIONBASE_HPP_
#define ORO_OPERATIONBASE_HPP_

#include "../rtt-config.h"
#include "../rtt-fwd.hpp"
#include "rtt-base-fwd.hpp"
#include <string>
#include <vector>
#include "DisposableInterface.hpp"

namespace RTT
{
    template<class T>
    class Operation;

    /**
     * Users can choose if an operation is executed in the component's thread
     * or in the thread of the caller.
     */
    enum ExecutionThread { OwnThread, ClientThread };

    namespace base
    {
        /**
         * This base class serves as a template-less handle for operation
         * objects and also stores the name and documentation of an operation.
         */
        class OperationBase
        {
        protected:
            std::string mname;
            std::vector<std::string> descriptions;
            ExecutionEngine* mowner;
            RTT_API void mdoc(const std::string& description);
            RTT_API void marg(const std::string& name, const std::string& description);
            virtual void ownerUpdated() = 0;
        public:
            RTT_API OperationBase(const std::string& name);
            RTT_API virtual ~OperationBase();

            /**
             * Returns the name of this operation.
             * @return The name.
             */
            const std::string& getName() { return mname; }

            /**
             * Returns all the \b documented descriptions of this operation, as a single list,
             * starting with the operation's description, followed by the name,
             * description pairs of each argument. In case no argument was documented,
             * only the description of the operation is returned.
             * @return A vector with all documented descriptions.
             */
            const std::vector<std::string>& getDescriptions() { return descriptions; }

            /**
             * Returns the implementation object of this operation.
             * @return
             */
            virtual DisposableInterface::shared_ptr getImplementation() = 0;

            virtual const DisposableInterface::shared_ptr getImplementation() const = 0;

            /**
             * An operation is ready if it has an implementation and is
             * present in a TaskContext.
             * @return true if ready.
             */
            bool ready() const {
                return mowner && getImplementation();
            }

            /**
             * Updates the owning ExecutionEngine that might execute this operation.
             * @param ee
             */
            void setOwner(ExecutionEngine* ee) {
                mowner = ee;
                // inform Operation<T> to update the implementation.
                this->ownerUpdated();
            }
        };
    }
}

#endif /* ORO_OPERATIONBASE_HPP_ */
