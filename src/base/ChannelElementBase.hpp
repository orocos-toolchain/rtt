#ifndef ORO_CHANNEL_BASE_HPP
#define ORO_CHANNEL_BASE_HPP

#include "../os/oro_atomic.h"
#include <utility>
#include <boost/intrusive_ptr.hpp>
#include <boost/call_traits.hpp>

namespace RTT { namespace base {

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
        /**
         * A default constructed ChannelElementBase has no input nor output
         * configured. The only way to set an input or output is to use
         * setOutput().
         */
        ChannelElementBase();
        virtual ~ChannelElementBase() {}

        /**
         * Removes the input channel (if any).
         * This call may delete channels from memory.
         */
        void removeInput();
        /**
         * Returns the current input channel element.
         * This will only return a valid channel element if
         * another element has received this object as an argument
         * to setOutput().
         * @return
         */
        ChannelElementBase::shared_ptr getInput();

        /**
         * Removes the output channel (if any).
         * This call may delete channels from memory.
         */
        void removeOutput();

        ChannelElementBase::shared_ptr getOutput();

        /**
         * Sets the output of this channel element to \a output and sets the input of \a output to this.
         * This implies that this channel element becomes the input of \a output.
         * There is no setInput function since this function does both setting input and output of
         * \a this and \a output.
         * @param output the next element in chain.
         */
        void setOutput(shared_ptr output);

        /** Signals that there is new data available on this channel
         * By default, the channel element forwards the call to its output
         *
         * @returns false if an error occured that requires the channel to be invalidated. In no ways it indicates that the sample has been received by the other side of the channel.
         */
        virtual bool signal();

        /**
         * This is called by an input port when it is ready to receive data.
         * Each channel element has the responsibility to pass this notification
         * on to the next, in the direction of the output.
         * @return false if a fatal connection failure was encountered and
         * the channel needs to be destroyed.
         */
        virtual bool inputReady();

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

    void intrusive_ptr_add_ref( ChannelElementBase* e );
    void intrusive_ptr_release( ChannelElementBase* e );

}}

#endif

