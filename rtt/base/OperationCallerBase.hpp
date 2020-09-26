/***************************************************************************
  tag: FMTC  do nov 2 13:06:11 CET 2006  OperationCallerBase.hpp

                        OperationCallerBase.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
    email                : peter.soetens@fmtc.be

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


#ifndef ORO_OPERATION_CALLER_BASE_HPP
#define ORO_OPERATION_CALLER_BASE_HPP

#include "rtt/rtt-fwd.hpp"
#include "rtt/internal/Invoker.hpp"
#include "rtt/base/OperationCallerInterface.hpp"
#include <boost/shared_ptr.hpp>

namespace RTT
{
    namespace base
    {
        /**
         * The base class for all method implementations. Both local and remote
         * method implementations must inherit from this class.
         */
        template<class F>
        struct OperationCallerBase
            : public internal::InvokerBase<F>,
              public OperationCallerInterface
        {
            typedef boost::shared_ptr<OperationCallerBase<F> > shared_ptr;
            virtual ~OperationCallerBase() {}

            /**
             * Creates a new clone of this method using operator new().
             * @param caller The ExecutionEngine of the task that wishes
             * to call this method.
             * @return a clone.
             * @nrt
             */
            virtual OperationCallerBase<F>* cloneI(ExecutionEngine* caller) const = 0;

        };
    }
}

#endif
