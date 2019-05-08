/***************************************************************************
  tag: Peter Soetens Sun Oct 24 18:58:27 2010 +0200 OperationCallerInterface.hpp

                        OperationCallerInterface.hpp -  description
                           -------------------
    begin                : Sun Oct 24 2010
    copyright            : (C) 2010 Peter Soetens
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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_OPERATION_CALLER_INTERFACE_HPP
#define ORO_OPERATION_CALLER_INTERFACE_HPP

#include "../rtt-fwd.hpp"
#include "DisposableInterface.hpp"
#include "OperationBase.hpp"

namespace RTT
{
    namespace base
    {
        /**
         * The interface class for operation callers.
         */
        struct RTT_API OperationCallerInterface: public DisposableInterface
        {
            /**
             * Use this type for shared pointer storage of an
             * OperationCallerInterface object.
             */
            typedef boost::shared_ptr<OperationCallerInterface> shared_ptr;

            /**
             * Initializes an empty OperationCallerInterface with owner,
             * executor and caller set to zero and the ExecutionThread type
             * to ClientThread.
             */
            OperationCallerInterface();

            OperationCallerInterface(OperationCallerInterface const& orig);

            virtual ~OperationCallerInterface();

            /**
             * Available such that implementations have a way to
             * expose their ready-ness, ie being able to do the call.
             */
            virtual bool ready() const = 0;

            /**
             * Set the ExecutionEngine of the task which owns this method.
             * @param ee The ExecutionEngine of the component that
             * owns this operation.
             */
            virtual void setOwner(ExecutionEngine* ee);

            /**
             * Sets the caller's engine of this operation.
             * This object will be used to test if asynchronous
             * messaging is required.
             * @param ee The ExecutionEngine of the component that
             * is calling this operation.
             */
            virtual void setCaller(ExecutionEngine* ee);

            /**
             * Sets the Thread execution policy of this object.
             * @param et OwnThread or ClientThread.
             * @param executor The engine of the component owning this
             * operation. In case it is not yet owned by a component,
             * executor may be null.
             * @return false if it may not be modified.
             */
            bool setThread(ExecutionThread et,
                           ExecutionEngine* executor);

            ExecutionThread getThread() const { return met; }

            /**
             * Executed when the operation execution resulted in a
             * C++ exception. Must report the error to the ExecutionEngine
             * of the owner of this operation.
             */
            void reportError();

            /**
             * Helpful function to tell us if this operations is to be sent or not.
             */
            bool isSend() { return met == OwnThread && myengine != caller; }

            ExecutionEngine* getMessageProcessor() const;

        protected:
            ExecutionEngine* myengine;
            ExecutionEngine* caller;
            ExecutionThread met;
        };
    }
}

#endif
