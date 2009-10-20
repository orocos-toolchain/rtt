#ifndef ORO_CONN_POLICY_HPP
#define ORO_CONN_POLICY_HPP

#include <string>
#include "rtt-fwd.hpp"

namespace RTT {

    /** A connection policy object describes how a given connection should
     * behave. Various parameters are available:
     *
     * <ul>
     *  <li> the connection type: DATA or BUFFER. On a data connection, the reader will have
     *       only access to the last written value. On a buffered connection, a
     *       \a size number of elements can be stored until the reader reads
     *       them.
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
     *  <li> the transport type. Can be used to force a certain kind of transports.
     *       The number is a RTT transport id. When the transport type is zero,
     *       local in-process communication is used, unless one of the ports is
     *       remote. If the transport type deviates from the default remote transport
     *       of one of the ports, an out-of-band transport is setup using that type.
     *  <li> the data size. Some protocols require a hint on big the data will be,
     *       especially if the data is dynamically sized (like std::vector<double>).
     *       If you leave this empty (recommended), the protocol will try to guess it.
     *       The unit of data size is protocol dependent.
     *  <li> the name of the connection. Can be used to coordinate out of band
     *       transport such that they can find each other by name. In practice,
     *       the name contains a port number or file descriptor to be opened.
     *       You only need to provide a name_id if you're using out-of-band transports
     *       without supervisor, for example, when using MQueues without Corba.
     * </ul>
     */
    class ConnPolicy
    {
    public:
        static const int DATA   = 0;
        static const int BUFFER = 1;

        static const int UNSYNC    = 0;
        static const int LOCKED    = 1;
        static const int LOCK_FREE = 2;

        /**
         * Create a policy for a (lock-free) buffer connection of a given size.
         * @param size The size of the buffer in this connection
         * @param lock_policy The locking policy
         * @param init_connection If an initial sample should be pushed into the buffer upon creation.
         * @param pull In inter-process cases, should the consumer pull itself ?
         * @return the specified policy.
         */
        static ConnPolicy buffer(int size, int lock_policy = LOCK_FREE, bool init_connection = false, bool pull = false)
        {
            ConnPolicy result(BUFFER, lock_policy);
            result.init = init_connection;
            result.pull = pull;
            result.size = size;
            return result;
        }
        /**
         * Create a policy for a (lock-free) shared data connection of a given size.
         * @param lock_policy The locking policy
         * @param init_connection If the data object should be initialised with the last value of the OutputPort upon creation.
         * @param pull In inter-process cases, should the consumer pull data itself ?
         * @return the specified policy.
         */
        static ConnPolicy data(int lock_policy = LOCK_FREE, bool init_connection = true, bool pull = false)
        {
            ConnPolicy result(DATA, lock_policy);
            result.init = init_connection;
            result.pull = pull;
            return result;
        }

        /**
         * The default policy is data driven, lock-free and local.
         * It is unsafe to rely on these defaults. It is prefered
         * to use the above buffer() and data() functions.
         * @param type
         * @param lock_policy
         * @return
         */
        explicit ConnPolicy(int type = DATA, int lock_policy = LOCK_FREE)
            : type(type), init(false), lock_policy(lock_policy), pull(false), size(0), transport(0), data_size(0) {}

        /** This is the type for the data holding element in the connection */
        int    type;
        /** If true, one should initialize the connection's value with the last
         * value written on the writer port. This is only possible if the writer
         * port has the keepsLastWrittenValue() flag set (i.e. if it remembers
         * what was the last written value).
         */
        bool   init;
        /** This is the locking policy on the connection */
        int    lock_policy;
        /** If true, then the sink will have to pull data. Otherwise, it is pushed
         * from the source. In both cases, the reader side is notified that new
         * data is available by base::ChannelElementBase::signal()
         */
        bool   pull;
        /** If the connection is a buffered connection, the size of the buffer */
        int    size;
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
    };

    bool composeProperty(const PropertyBag& bag, ConnPolicy& result);
    void decomposeProperty(const ConnPolicy& cp, PropertyBag& targetbag);
}

#endif

