/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  ChannelElementBase.hpp

                        ChannelElementBase.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Sylvain Joyeux
    email                : sylvain.joyeux@m4x.org

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


#ifndef ORO_CHANNEL_BASE_HPP
#define ORO_CHANNEL_BASE_HPP

#include "../os/oro_arch.h"
#include <utility>
#include <boost/intrusive_ptr.hpp>
#include <boost/call_traits.hpp>

#include "../rtt-fwd.hpp"
#include "rtt-base-fwd.hpp"
#include "../internal/rtt-internal-fwd.hpp"
#include "../BufferPolicy.hpp"
#include "../os/Mutex.hpp"

#include <list>
#include <map>

namespace RTT { namespace base {

    /** In the data flow implementation, a channel is created by chaining
     * ChannelElementBase objects.
     *
     * ChannelElementBase objects are refcounted. In the chain, an element
     * maintains the refcount for its successor, and holds a simple pointer to
     * its predecessor.
     */
    class RTT_API ChannelElementBase
    {
    public:
        typedef boost::intrusive_ptr<ChannelElementBase> shared_ptr;

    private:
        oro_atomic_t refcount;
        friend void RTT_API intrusive_ptr_add_ref( ChannelElementBase* e );
        friend void RTT_API intrusive_ptr_release( ChannelElementBase* e );

    protected:
        shared_ptr input;
        shared_ptr output;

        mutable RTT::os::SharedMutex input_lock;
        mutable RTT::os::SharedMutex output_lock;

    protected:
        /** Increases the reference count */
        void ref();
        /** Decreases the reference count, and deletes the object if it is zero
         */
        void deref();

    public:
        /**
         * A default constructed ChannelElementBase has no input nor output
         * configured. The only way to set an input or output is to use
         * connectTo() or connectFrom().
         */
        ChannelElementBase();
        virtual ~ChannelElementBase();

        /**
         * Return a pointer to the typed instance of a ChannelElementBase
         */
        template <typename T> static ChannelElement<T> *narrow(ChannelElementBase *e)
        {
            return dynamic_cast<ChannelElement<T> *>(e);
        }

        /**
         * Return a pointer to the typed variant of this ChannelElementBase
         */
        template <typename T> ChannelElement<T> *narrow()
        {
            return dynamic_cast<ChannelElement<T> *>(this);
        }

        /**
         * Returns the current input channel element.
         * This will only return a valid channel element if
         * another element has received this object as an argument
         * to connectTo().
         * @return
         */
        shared_ptr getInput();

        /**
         * Returns the first input channel element of this connection.
         * Will return the channel element the furthest away from the input port,
         * or \a this if none.
         * @return getInput() ? getInput()->getInputEndPoint() : this
         */
        virtual shared_ptr getInputEndPoint();


        /** Returns the next channel element in the channel's propagation
         * direction
         */
        shared_ptr getOutput();

        /**
         * Returns the last output channel element of this connection.
         * Will return the channel element the furthest away from the output port,
         * or \a this if none.
         * @return getOutput() ? getOutput()->getOutputEndPoint() : this
         */
        virtual shared_ptr getOutputEndPoint();

        /**
         * Connects a new output to this element.
         * @param output the next element in chain.
         * @param mandatory whether the added output is mandatory for a write operation to succeed
         */
        virtual bool connectTo(ChannelElementBase::shared_ptr const& output, bool mandatory = true);

        /**
         * Connects a new input to this element.
         * @param input the previous element in chain.
         */
        virtual bool connectFrom(ChannelElementBase::shared_ptr const& input);

        /**
         * Returns true, if this channel element is connected on the input or output side.
         */
        virtual bool connected();

        /** Signals that there is new data available on this channel
         * By default, the channel element forwards the call to its output
         *
         * @returns false if an error occured that requires the channel to be invalidated. In no ways it indicates that the sample has been received by the other side of the channel.
         */
        virtual bool signal();

        /** Signals that there is new data available on this channel
         * Forwards to \ref signal() unless overwritten in a derived class.
         */
        virtual bool signalFrom(ChannelElementBase *) { return signal(); }

        /**
         * This is called on the output half of a new connection by the connection
         * factory in order to notify the output side of the new connection and check if
         * it is ready to receive data.
         * Each channel element has the responsibility to pass this notification
         * on to the next, in the direction of the input. The ConnOutputEndPoint then calls
         * back the \ref inputReady() method in reverse direction to notify the output that
         * the connection was successfully established.
         * @return false if a fatal connection failure was encountered and
         * the channel needs to be destroyed.
         */
        virtual bool channelReady(ChannelElementBase::shared_ptr const& caller, ConnPolicy const& policy, internal::ConnID *conn_id = 0);

        /**
         * This is called by an input port when it is ready to receive data.
         * Each channel element has the responsibility to pass this notification
         * on to the next, in the direction of the output.
         * @return false if a fatal connection failure was encountered and
         * the channel needs to be destroyed.
         */
        virtual bool inputReady(ChannelElementBase::shared_ptr const& caller);

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

        /** Performs a disconnection of a single input or output endpoint. If
         * \a forward is true, then the disconnection is initiated by the input
         * endpoint and channel must be a connected input. Otherwise, it has been
         * initiated by the output endpoint and channel must be a connected output.
         * The disconnect call is only forwarded to the opposite side of the channel
         * element after the last input/output element on either side has been removed.
         *
         * The ChannelElementBase implementation ignores the given channel and disconnects
         * unconditinally.
         */
        virtual bool disconnect(ChannelElementBase::shared_ptr const& channel, bool forward);

        /**
         * Gets the port this channel element is connected to.
         * @return null if no port is connected to this element, the
         * port (or a proxy representing the port) otherwise.
         */
        virtual PortInterface* getPort() const;

        /**
         * Get a pointer to the connection policy used to build this channel element, if available.
         * This method will be overwritten for data and buffer elements.
         * @return null if there is no ConnPolicy associated with this element, a pointer to an
         * instance of ConnPolicy otherwise.
         */
        virtual const ConnPolicy* getConnPolicy() const;

        RTT_DEPRECATED void setOutput(const ChannelElementBase::shared_ptr &output)
        {
            assert(false && "ChannelElementBase::setOutput() is deprecated! You should use ChannelElementBase::connectTo() instead.");
            (void) connectTo(output);
        }

        RTT_DEPRECATED void setInput(const ChannelElementBase::shared_ptr &input)
        {
            assert(false && "ChannelElementBase::setInput() is deprecated! You should use ChannelElementBase::addInput() instead.");
            (void) addInput(input);
        }

        /**
         * This function may be used to identify, 
         * if the current element uses a network
         * transport, to send the data to the next
         * Element in the logical chain.
         * 
         * @return true if a network transport is used.
         * */
        virtual bool isRemoteElement() const;

        /**
         * This function returns the URI of the
         * next channel element in the logical
         * chain. The URI must be unique.
         * E.g: In the local case output->getLocalURI()
         * In the remote case the URI of the remote
         * channel element.
         * 
         * @return URI of the next element.
         * */
        virtual std::string getRemoteURI() const;

        /**
         * This function return the URI of this
         * element. The URI must be unique.
         * @return URI of this element.
         * */
        virtual std::string getLocalURI() const;

        /**
         * Returns the class name of this
         * element. This is primary useful 
         * for special case handling in the
         * connection tracking.
         * @return The name of the class of the ChannelElement
         * */
        virtual std::string getElementName() const;

    protected:
        friend class MultipleInputsChannelElementBase;
        friend class MultipleOutputsChannelElementBase;

        /**
         * Sets the new output channel element of this element or adds a channel to the outputs list.
         * @param output the next element in chain.
         * @param mandatory whether the added output is mandatory for a write to succeed
         * @return true if the output was set or false if this element does not support multiple outputs and the output is already set.
         */
        virtual bool addOutput(shared_ptr const& output, bool mandatory = true);

        /**
         * Remove an output from the outputs list.
         * @param output the element to be removed, or null to remove unconditionally
         */
        virtual void removeOutput(shared_ptr const& output);

        /**
         * Sets the new input channel element of this element or adds a channel to the inputs list.
         * @param input the previous element in chain.
         * @return true if the input was set or false if this element does not support multiple inputs and the input is already set.
         */
        virtual bool addInput(shared_ptr const& input);

        /**
         * Remove an input from the inputs list.
         * @param input the element to be removed, or null to remove unconditionally
         */
        virtual void removeInput(shared_ptr const& input);

    private:
        /**
         * Deprecated, argument-less variant of \ref inputReady(shared_ptr).
         * Left here as a fallback for backwards compatibility, if
         * transports deriving from ChannelElementBase implement inputReady() only.
         */
        virtual bool inputReady();
    };

    /**
     * ChannelElementBase variant with multiple input channels.
     */
    class RTT_API MultipleInputsChannelElementBase : virtual public ChannelElementBase
    {
    public:
        typedef boost::intrusive_ptr<MultipleInputsChannelElementBase> shared_ptr;
        typedef std::list<ChannelElementBase::shared_ptr> Inputs;

    protected:
        Inputs inputs;
        mutable RTT::os::SharedMutex inputs_lock;

    public:
        MultipleInputsChannelElementBase();

        /**
         * Returns true, if this channel element has at least one input, independent of whether is has an
         * output connection or not.
         */
        virtual bool connected();

        /**
         * Overridden implementation of \ref ChannelElementBase::inputReady().
         * Forwards the inputReady() call to all inputs and only returns true if all inputs returned true.
         */
        virtual bool inputReady(ChannelElementBase::shared_ptr const& caller);

        /**
         * Overridden implementation of \ref ChannelElementBase::clear().
         * Forwards the clear() call to all inputs.
         */
        virtual void clear();

        using ChannelElementBase::disconnect;

        /**
         * Overridden implementation of \ref ChannelElementBase::disconnect(forward, channel).
         */
        virtual bool disconnect(ChannelElementBase::shared_ptr const& channel, bool forward = true);

        /**
         * Overridden implementation of \ref ChannelElementBase::signalFrom(caller) which remembers the caller who last signalled this channel element.
         */
        bool signalFrom(ChannelElementBase *caller);

    protected:
        /**
         * Sets the new input channel element of this element or adds a channel to the inputs list.
         * @param input the previous element in chain.
         * @return true if the input was set or false if this element does not support multiple inputs and the input is already set.
         */
        virtual bool addInput(ChannelElementBase::shared_ptr const& input);

        /**
         * Remove an input from the inputs list.
         * @param input the element to be removed
         */
        virtual void removeInput(ChannelElementBase::shared_ptr const& input);
    };

    /**
     * ChannelElementBase variant with multiple output channels.
     */
    class RTT_API MultipleOutputsChannelElementBase : virtual public ChannelElementBase
    {
    public:
        typedef boost::intrusive_ptr<MultipleOutputsChannelElementBase> shared_ptr;
        struct Output {
            Output(ChannelElementBase::shared_ptr const &channel, bool mandatory = true);
            bool operator==(ChannelElementBase::shared_ptr const& channel) const;
            ChannelElementBase::shared_ptr channel;
            bool mandatory;
            bool disconnected;
        };
        typedef std::list<Output> Outputs;

    protected:
        Outputs outputs;
        mutable RTT::os::SharedMutex outputs_lock;

    public:
        MultipleOutputsChannelElementBase();

        /**
         * Returns true, if this channel element has at least one output, independent of whether is has an
         * input connection or not.
         */
        virtual bool connected();

        /**
         * Overridden implementation of \ref ChannelElementBase::signal() which forwards the signal to all
         * outputs.
         */
        virtual bool signal();

        /**
         * Overridden implementation of \ref ChannelElementBase::channelReady() which forwards the signal to all
         * outputs.
         */
        virtual bool channelReady(ChannelElementBase::shared_ptr const& caller, ConnPolicy const& policy, internal::ConnID *conn_id = 0);

        using ChannelElementBase::disconnect;

        /**
         * Overridden implementation of \ref ChannelElementBase::disconnect(forward, channel).
         */
        virtual bool disconnect(ChannelElementBase::shared_ptr const& channel, bool forward = false);

    protected:
        /**
         * Sets the new output channel element of this element or adds a channel to the outputs list.
         * @param output the next element in chain.
         * @param mandatory whether the added output is mandatory for a write to succeed
         * @return true if the output was set or false if this element does not support multiple outputs and the output is already set.
         */
        virtual bool addOutput(ChannelElementBase::shared_ptr const& output, bool mandatory = true);

        /**
         * Remove an output from the outputs list.
         * @param output the element to be removed
         */
        virtual void removeOutput(ChannelElementBase::shared_ptr const& output);

        /**
         * Iterate over all output channels and remove the ones that have been marked as disconnected
         * (after a failed write() or data_sample() call).
         */
        void removeDisconnectedOutputs();
    };

    /**
     * ChannelElementBase variant with multiple input and multiple output channels.
     */
    class RTT_API MultipleInputsMultipleOutputsChannelElementBase : virtual public MultipleInputsChannelElementBase, virtual public MultipleOutputsChannelElementBase
    {
    public:
        typedef boost::intrusive_ptr<MultipleInputsMultipleOutputsChannelElementBase> shared_ptr;

        /**
         * Returns true, if this channel element has at least one input AND at least one output.
         */
        virtual bool connected();

        using ChannelElementBase::disconnect;

        /**
         * Overridden implementation of \ref ChannelElementBase::disconnect(forward, channel).
         */
        virtual bool disconnect(ChannelElementBase::shared_ptr const& channel, bool forward);
    };

    void RTT_API intrusive_ptr_add_ref( ChannelElementBase* e );
    void RTT_API intrusive_ptr_release( ChannelElementBase* e );

}}

#endif

