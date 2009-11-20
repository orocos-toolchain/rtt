/***************************************************************************
  tag: FMTC  do nov 2 13:05:56 CET 2006  EventBase.hpp

                        EventBase.hpp -  description
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



#ifndef ORO_CORELIB_EVENT_BASE_HPP
#define ORO_CORELIB_EVENT_BASE_HPP

#include "../internal/Invoker.hpp"
#include "../internal/Subscriber.hpp"
#include "ActionInterface.hpp"

namespace RTT
{
    namespace base
    {
        /**
         * The base class of Event implementations.
         */
        template<
            typename Signature
        >
        class EventBase
            : public internal::InvokerBase<Signature>,
              public internal::SubscriberBase<Signature>,
              public ActionInterface
        {
        public:
            typedef typename internal::SubscriberBase<Signature>::SlotFunction SlotFunction;

            void readArguments() {}
            bool execute() {return false;}

            virtual int arity() const = 0;

            /**
             * Clone this implementation.
             */
            virtual EventBase<Signature>* cloneI() const = 0;
        };
    }
}

#endif
