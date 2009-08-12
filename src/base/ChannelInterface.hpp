#ifndef ORO_EXECUTION_CHANNEL_INTERFACE_HPP
#define ORO_EXECUTION_CHANNEL_INTERFACE_HPP

#include "../os/oro_atomic.h"
#include <utility>
#include <boost/intrusive_ptr.hpp>
#include <boost/call_traits.hpp>

namespace RTT {
    class ChannelElementBase;

    template <typename T> class DataObjectInterface;
    template <typename T> class BufferInterface;

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
            : type(type), init(false), lock_policy(lock_policy), pull(false), size(0) {}

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
         * data is available by ChannelElementBase::signal()
         */
        bool   pull;
        /** If the connection is a buffered connection, the size of the buffer */
        int    size;
    };

    class ChannelElementBase;
    void intrusive_ptr_add_ref( ChannelElementBase* e );
    void intrusive_ptr_release( ChannelElementBase* e );

    /** In the data flow implementation, a channel is created by chaining
     * ChannelElementBase objects.
     *
     * ChannelElementBase objects are refcounted. In the chain, an element
     * maintains the refcount for its successor, and holds a simple pointer to
     * its predecessor.
     */
    class ChannelElementBase
    {
    public:
        typedef boost::intrusive_ptr<ChannelElementBase> shared_ptr;

    private:
        oro_atomic_t refcount;
        friend void intrusive_ptr_add_ref( ChannelElementBase* e );
        friend void intrusive_ptr_release( ChannelElementBase* e );

    protected:
        ChannelElementBase* input;
        shared_ptr          output;

        /** Increases the reference count */
        void ref();
        /** Decreases the reference count, and deletes the object if it is zero
         */
        void deref();

    public:
        ChannelElementBase();
        virtual ~ChannelElementBase() {}

        void removeInput();
        ChannelElementBase::shared_ptr getInput();
        void removeOutput();
        ChannelElementBase::shared_ptr getOutput();
        void setOutput(shared_ptr output);

        /** Signals that there is new data available on this channel
         * By default, the channel element forwards the call to its output
         *
         * @returns false if an error occured that requires the channel to be invalidated. In no ways it indicates that the sample has been received by the other side of the channel.
         */
        virtual bool signal() const;

        /** Clears any data stored by the channel. It means that
         * ChannelElement::read() will return false afterwards (provided that no
         * new data has been written on the meantime of course)
         *
         * By default, the channel element forwards the calls to its input
         */
        virtual void clear();

        /** Performs a disconnection of this channel's endpoints. If
         * \a forward is true, then the disconnection is initiated by the input
         * endpoint. Otherwise, it has been initiated by the output endpoint.
         */
        virtual void disconnect(bool forward);
    };

    /** A typed version of ChannelElementBase. It defines generic methods that are
     * type-specific (like write and read)
     */
    template<typename T>
    class ChannelElement : public ChannelElementBase
    {
    public:
        typedef T value_t;
        typedef boost::intrusive_ptr< ChannelElement<T> > shared_ptr;
        typedef typename boost::call_traits<T>::param_type param_t;
        typedef typename boost::call_traits<T>::reference reference_t;

        /** Writes a new sample on this connection. \a sample is the sample to
         * write. 
         *
         * @returns false if an error occured that requires the channel to be invalidated. In no ways it indicates that the sample has been received by the other side of the channel.
         */
        virtual bool write(param_t sample)
        {
            typename ChannelElement<T>::shared_ptr output = boost::static_pointer_cast< ChannelElement<T> >(this->output);
            if (output)
                return output->write(sample);
            return false;
        }

        /** Reads a sample from the connection. \a sample is a reference which
         * will get updated if a sample is available. The method returns true
         * if a sample was available, and false otherwise. If false is returned,
         * then \a sample is not modified by the method
         */
        virtual bool read(reference_t sample)
        {
            typename ChannelElement<T>::shared_ptr input = static_cast< ChannelElement<T>* >(this->input);
            if (input)
                return input->read(sample);
            else return false;
        }
    };
}

#endif

