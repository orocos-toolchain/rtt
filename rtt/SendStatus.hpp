/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  SendStatus.hpp

                        SendStatus.hpp -  description
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



#ifndef SENDSTATUS_HPP_
#define SENDSTATUS_HPP_

#include "rtt-config.h"

#include <ostream>
#include <istream>

namespace RTT {

/**
 * Returns the status of a send() or collect() invocation.
 */
enum SendStatus {
    CollectFailure = -2, /** Returned when the result of the send() could not be collected. This happens when the 'caller' has not been set in the OperationCaller. */
    SendFailure = -1,    /** Returned when the send() failed to deliver the operation call to the receiving component. This happens when the operation's component is in FatalError or has a full message queue. */
    SendNotReady = 0,    /** Returned when the send() succeeded, but the operation has not yet been executed by the receiving component. */
    SendSuccess = 1      /** Returned when the send() was a success and the results can be collected. */
};

RTT_API std::ostream& operator<<(std::ostream& os, SendStatus fs);
RTT_API std::istream& operator>>(std::istream& os, SendStatus& fs);

}

#endif /* SENDSTATUS_HPP_ */
