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


#ifndef ORO_BUFFER_POLICY_HPP
#define ORO_BUFFER_POLICY_HPP

#include "rtt-config.h"
#include <iosfwd>

namespace RTT {

    /**
     * The BufferPolicy controls how multiple connections to the
     * same input or output port are handled in case of concurrent or subsequent read
     * and write operations.
     *
     * Possible values:
     *
     * - PerConnection:
     *   Buffers (or data objects) will be installed per connection
     *   (per pair of output and input port/stream). Input ports with multiple
     *   connections first try to read from the last read channel first, then
     *   poll all connections in the order they have been made (which in practice
     *   means there are no guarantees on the order if multiple writers write
     *   concurrently). This is the default buffer policy.
     *
     * - PerInputPort:
     *   Every input port has a single input buffer (or data object) and all
     *   connected output ports/streams will write to the same buffer. This
     *   policy requires that pull == false for remote connections or input streams.
     *
     * - PerOutputPort:
     *   Every output has a single output buffer (or data object) and all connected
     *   readers "consume" elements from this buffer. Exactly one connected input port
     *   or output stream will see every written sample as NewData. This policy
     *   requires that pull == true for remote connections or output streams.
     *
     * - Shared:
     *   The buffer (or data object) is shared between all connected input and output
     *   ports. It can have an arbitrary number of writers and readers. Ports can be
     *   connected to an existing shared connection instance either by connecting it
     *   to a port that is already part of the shared connection group, or with a special
     *   createConnection() method. This was the default buffer policy in the days of
     *   RTT v1.
     *
     * @ingroup Ports
     */
    typedef enum {
        UnspecifiedBufferPolicy,
        PerConnection,
        PerInputPort,
        PerOutputPort,
        Shared,
    } BufferPolicy;

    std::ostream &operator<<(std::ostream &os, const BufferPolicy &bp);
    std::istream &operator>>(std::istream &is, BufferPolicy &bp);
}

#endif
