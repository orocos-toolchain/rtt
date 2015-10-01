/***************************************************************************
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


#ifndef ORO_READ_POLICY_HPP
#define ORO_READ_POLICY_HPP

#include "rtt-config.h"
#include <iosfwd>

namespace RTT {

    /**
     * A read policy describes how a read operation should select from
     * which channel/connection it reads if multiple channels are available.
     *
     * Possible values:
     * - ReadShared:              The input buffer is shared among all connections.
     * - ReadUnordered:           The InputPort polls all connections in the order the connections have been
     *                            established, but it prefers the last connection where data has been successfully
     *                            read from. In fact there are no guarantees for the order in which samples from
     *                            different connections are read.
     *
     * Special values:
     * - UnspecifiedReadPolicy:   No specific ReadPolicy was set yet. This is the default ReadPolicy for an
     *                            unconnected InputPort. Do not use this value in \ref ConnPolicy.
     *
     * @ingroup Ports
     */
    typedef enum {
        UnspecifiedReadPolicy,
        ReadShared,
        ReadUnordered,
    } ReadPolicy;

    extern ReadPolicy ReadPolicyDefault;

    std::ostream &operator<<(std::ostream &os, const ReadPolicy &rp);
    std::istream &operator>>(std::istream &is, ReadPolicy &rp);
}

#endif
