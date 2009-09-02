#ifndef ORO_CONN_POLICY_HPP
#define ORO_CONN_POLICY_HPP

namespace RTT { namespace internal {

    /** A connection policy object describes how a given connection should
     * behave. Various parameters are available:
     *
     * <ul>
     *  <li> the connection type. On a data connection, the reader will have
     *       only access to the last written value. On a buffered connection, a
     *       specified number of elements can be stored until the reader reads
     *       them.
     *  <li> the locking policy. This defines how locking is done in the
     *       connection. For now, only two policies are available. LOCKED uses
     *       mutexes, while LOCK_FREE uses a lock free method. The latter should
     *       be used only when the probability of contention (simultaneous write-read)
     *       is low.
     *  <li> if, upon connection, the last value that has been written on the
     *       writer end should be written on the connection as well to
     *       initialize it. This flag has an effect only if the writer has
     *       keepsLastWrittenValue() set to true (see
     *       WritePortInterface::keepLastWrittenValue()).
     *  <li> if the data is pushed or pulled on the connection. This has an
     *       effect only on multi-process communication. In the pushed case (the
     *       default), new data is actively pushed to the reader's process. In
     *       the pulled case, data must be requested by the reader.
     * </ul>
     */
    class ConnPolicy
    {
    public:
        static const int DATA   = 0;
        static const int BUFFER = 1;

        static const int LOCKED    = 0;
        static const int LOCK_FREE = 1;
        static const int UNSYNC    = 2;

        static ConnPolicy buffer(int size, int lock_policy = LOCK_FREE, bool init_connection = false, bool pull = false)
        {
            ConnPolicy result(BUFFER, lock_policy);
            result.init = init_connection;
            result.pull = pull;
            result.size = size;
            return result;
        }
        static ConnPolicy data(int lock_policy = LOCK_FREE, bool init_connection = true, bool pull = false)
        {
            ConnPolicy result(DATA, lock_policy);
            result.init = init_connection;
            result.pull = pull;
            return result;
        }

        explicit ConnPolicy(int type = DATA, int lock_policy = LOCK_FREE)
            : type(type), init(false), lock_policy(lock_policy), pull(false), size(0), transport(0) {}

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
         * The name of this connection. May be used by transports to define a 'topic' or
         * lookup name to connect two data streams. If you leave this empty (recommended),
         * the protocol will choose an appropriate name itself. Only specify a name to
         * work around name clashes or if the transport protocol documents to do so.
         */
        mutable std::string name_id;
    };
}}

#endif

