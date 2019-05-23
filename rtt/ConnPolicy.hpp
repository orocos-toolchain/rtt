/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  ConnPolicy.hpp

                        ConnPolicy.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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


#ifndef ORO_CONN_POLICY_HPP
#define ORO_CONN_POLICY_HPP

#include <string>
#include <iosfwd>
#include "rtt-fwd.hpp"
#include "rtt-config.h"
#include "BufferPolicy.hpp"

namespace RTT {

    /** A connection policy object describes how a given connection should
     * behave. Various parameters are available:
     *
     * <ul>
     *  <li> the connection type: DATA, BUFFER, CIRCULAR_BUFFER or UNBUFFERED.
     *       On a data connection, the reader will have
     *       only access to the last written value. On a buffered connection, a
     *       \a size number of elements can be stored until the reader reads
     *       them. BUFFER drops newer samples on full, CIRCULAR_BUFFER drops older samples on full.
     *       UNBUFFERED is only valid for output streaming connections.
     *
     *  <li> the locking policy: LOCKED, LOCK_FREE or UNSYNC. This defines how locking is done in the
     *       connection. For now, only three policies are available. LOCKED uses
     *       mutexes, LOCK_FREE uses a lock free method and UNSYNC means there's no
     *       synchronisation at all (not thread safe). The latter should
     *       be used only when there is no contention (simultaneous write-read).
     *
     *  <li> if, upon connection, the last value that has been written on the
     *       writer end should be written on the connection as well to
     *       initialize it. This flag has an effect only if the writer has
     *       keepsLastWrittenValue() set to true (see
     *       OutputPortInterface::keepLastWrittenValue()).
     *
     *  <li> if the data is pushed or pulled on the connection. This has an
     *       effect only on multi-process communication. In the pushed case (the
     *       default), new data is actively pushed to the reader's process. In
     *       the pulled case, data must be requested by the reader.
     *
     *  <li> the buffer policy, which controls how multiple connections to the
     *       same input or output port are handled in case of concurrent or subsequent read
     *       and write operations. See \ref BufferPolicy to see all available options.
     *       Not all combinations of buffer policies and the pull flag are valid and non-standard transports
     *       can have additional restrictions.
     *
     *  <li> if the connection is mandatory. Mandatory connections will let the write()
     *       call fail if the new sample cannot be successfully written. Default connections
     *       are not mandatory.
     *
     *  <li> the transport type. Can be used to force a certain kind of transports.
     *       The number is a RTT transport id. When the transport type is zero,
     *       local in-process communication is used, unless one of the ports is
     *       remote. If the transport type deviates from the default remote transport
     *       of one of the ports, an out-of-band transport is setup using that type.
     *
     *  <li> the data size. Some protocols require a hint on big the data will be,
     *       especially if the data is dynamically sized (like std::vector<double>).
     *       If you leave this empty (recommended), the protocol will try to guess it.
     *       The unit of data size is protocol dependent.
     *
     *  <li> the name of the connection. Can be used to coordinate out of band
     *       transport such that they can find each other by name. In practice,
     *       the name contains a port number or file descriptor to be opened.
     *       You only need to provide a name_id if you're using out-of-band transports
     *       without supervisor, for example, when using MQueues without Corba.
     * </ul>
     * @ingroup Ports
     */
    class RTT_API ConnPolicy
    {
    public:
        static const int UNBUFFERED = -1;
        static const int DATA   = 0;
        static const int BUFFER = 1;
        static const int CIRCULAR_BUFFER = 2;

        static const int UNSYNC    = 0;
        static const int LOCKED    = 1;
        static const int LOCK_FREE = 2;

        static const bool PUSH = false;
        static const bool PULL = true;

        /**
         * Returns the process-wide default ConnPolicy that serves as a template for new ConnPolicy instances.
         *
         * This method returns a non-const reference and you can change the defaults. This is not thread-safe
         * and should only be done very early in the deployment phase, before the first component is loaded and
         * before connecting ports.
         */
        static ConnPolicy &Default();

        /**
         * Create a policy for a (lock-free) fifo buffer connection of a given size.
         * @param size The size of the buffer in this connection
         * @param lock_policy The locking policy
         * @param init_connection If an initial sample should be pushed into the buffer upon creation.
         * @param pull In inter-process cases, should the consumer pull itself ?
         * @return the specified policy.
         */
        static ConnPolicy buffer(int size, int lock_policy = LOCK_FREE, bool init_connection = false, bool pull = false);

        /**
         * Create a policy for a (lock-free) \b circular fifo buffer connection of a given size.
         * @param size The size of the buffer in this connection
         * @param lock_policy The locking policy
         * @param init_connection If an initial sample should be pushed into the buffer upon creation.
         * @param pull In inter-process cases, should the consumer pull itself ?
         * @return the specified policy.
         */
        static ConnPolicy circularBuffer(int size, int lock_policy = LOCK_FREE, bool init_connection = false, bool pull = false);

        /**
         * Create a policy for a (lock-free) shared data connection of a given size.
         * @param lock_policy The locking policy
         * @param init_connection If the data object should be initialised with the last value of the OutputPort upon creation.
         * @param pull In inter-process cases, should the consumer pull data itself ?
         * @return the specified policy.
         */
        static ConnPolicy data(int lock_policy = LOCK_FREE, bool init_connection = true, bool pull = false);

        /**
         * Constructs a new ConnPolicy instance based on the current
         * default settings as returned by ConnPolicy::Default().
         */
        ConnPolicy();

        /**
         * Constructs a new ConnPolicy instance based on the current
         * default settings as returned by ConnPolicy::Default(), but
         * overrides the type.
         * You should not use this contructor anymore and prefer the static
         * methods \ref ConnPolicy::data(), \ref ConnPolicy::buffer(), etc. instead.
         * @param type
         * @deprecated
         */
        explicit ConnPolicy(int type);

        /**
         * Constructs a new ConnPolicy instance based on the current
         * default settings as returned by ConnPolicy::Default(), but
         * overrides the type and lock_policy.
         * You should not use this contructor anymore and prefer the static
         * methods \ref ConnPolicy::data(), \ref ConnPolicy::buffer(), etc. instead.
         * @param type
         * @param lock_policy
         * @deprecated
         */
        explicit ConnPolicy(int type, int lock_policy);

        /** DATA, BUFFER or CIRCULAR_BUFFER */
        int    type;

        /** If the connection is a buffered connection, the size of the buffer */
        int    size;

        /** This is the locking policy on the connection */
        int    lock_policy;

        /** If true, one should initialize the connection's value with the last
         * value written on the writer port. This is only possible if the writer
         * port has the keepsLastWrittenValue() flag set (i.e. if it remembers
         * what was the last written value).
         */
        bool   init;

        /** If true, then the sink will have to pull data. Otherwise, it is pushed
         * from the source. In both cases, the reader side is notified that new
         * data is available by base::ChannelElementBase::signal()
         */
        bool   pull;

        /**
         * The policy on how buffer elements will be installed for this connection, which influences
         * the behavior of reads and writes if the port has muliple connections.
         * See \ref BufferPolicy enum for possible options.
         */
        int    buffer_policy;

        /**
         * The maximum number of threads that will access the connection data or buffer object.
         * This only needs to be specified for lock-free data structures.
         * If 0, the number of threads will be determined by a simple heuristic depending on the
         * read and write policies of the connection.
         */
        int    max_threads;

        /**
         * Whether the connection described by this connection policy is mandatory, which
         * means that write operations will fail if the connection could not be served, e.g. due
         * to a full input buffer or because of a broken remote connection.
         * By default, all connections are mandatory.
         */
        bool   mandatory;

        /**
         * The prefered transport used. 0 is local (in process), a higher number
         * is used for inter-process or networked communication transports.
         */
        int    transport;

        /**
         * Suggest the payload size of the data sent over this channel. Connections
         * can use this value to optimize transmission or prepare the communication
         * channel for real-time communication. This value might be overruled by
         * the transport protocol if it can make a better guess. The interpretation
         * of data_size is transport specific. It may be bytes, it may be something else.
         * Leave this value set to zero, unless the transport documents otherwise.
         */
        mutable int    data_size;

        /**
         * The name of this connection. May be used by transports to define a 'topic' or
         * lookup name to connect two data streams. If you leave this empty (recommended),
         * the protocol will choose an appropriate name itself. Only specify a name to
         * work around name clashes or if the transport protocol documents to do so.
         */
        mutable std::string name_id;

    private:
        struct ConnPolicyDefault;
        ConnPolicy(const ConnPolicyDefault &);
    };

    std::ostream &operator<<(std::ostream &os, const ConnPolicy &cp);
}

#endif

