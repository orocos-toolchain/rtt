/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  OperationCallerBaseInvoker.hpp

                        OperationCallerBaseInvoker.hpp -  description
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


#ifndef ORO_METHODBASEINVOKER_HPP_
#define ORO_METHODBASEINVOKER_HPP_

#include <string>
#include "DisposableInterface.hpp"
#include "../rtt-fwd.hpp"

namespace RTT
{
    namespace base {
        /**
         * An invoker can be given an implementation and is identified by a name.
         */
        struct OperationCallerBaseInvoker
        {
            /**
             * Returns true if this OperationCaller has received a correct implementation.
             */
            virtual bool ready() const = 0;
            /**
             * Sets a new implementation for this method.
             * @param impl An implementation object that can be upcast to the OperationCallerBase
             * class type of this method.
             * @param caller The engine of the calling TaskContext.
             * @return true if the impl could be upcast to a matching OperationCallerBase type, false otherwise.
             */
            virtual bool setImplementation(boost::shared_ptr<base::DisposableInterface> impl, ExecutionEngine* caller = 0) = 0;
            /**
             * Sets a new implementation for this method by using a service part.
             * @param orp A part obtained from a service's OperationInterface.
             * @param caller The engine of the calling TaskContext.
             * @return true if the part could produce the correct OperationCallerBase type, false otherwise.
             */
            virtual bool setImplementationPart(OperationInterfacePart* orp, ExecutionEngine* caller = 0) = 0;
            /**
             * Sets the caller of this method after the implementation was set.
             * @param caller
             */
            virtual void setCaller(ExecutionEngine* caller) = 0;
            /**
             * Returns the name of this method instance.
             */
            virtual const std::string& getName() const = 0;
        };

    }
}

#endif /* ORO_METHODBASEINVOKER_HPP_ */
