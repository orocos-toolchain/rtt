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

#include <rtt/os/Mutex.hpp>
#include "rtt-fwd.hpp"
#include "rtt-base-fwd.hpp"
#include "../internal/rtt-internal-fwd.hpp"

#include <list>

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

        RTT::os::SharedMutex inout_lock;

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
         * setInput() or setOutput().
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
         * to setOutput().
         * @return
         */
        shared_ptr getInput();

        /**
         * Returns the first input channel element of this connection.
         * Will return the channel element the furthest away from the input port,
         * or \a this if none.
         * @return getInput() ? getInput()->getInputEndPoint() : this
         */
        shared_ptr getInputEndPoint();


        /** Returns the next channel element in the channel's propagation
         * direction
         */
        shared_ptr getOutput();

        /**
         * Returns the last output channel element of this connection.
         * Will return the channel element the furthest away from the output port,
         * or \a this if none.
         * @return getOutput() ? getOutput()->getInputEndPoint() : this
         */
        shared_ptr getOutputEndPoint();

        /**
         * Sets the output of this channel element to \a output and sets the input of \a output to this.
         * This implies that this channel element becomes the input of \a output.
         * @param output the next element in chain.
         */
        virtual void setOutput(shared_ptr output);

        /**
         * Sets the input of this channel element to \a input.
         * @param input the previous element in chain.
         */
        virtual void setInput(shared_ptr input);

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

        /** Performs a disconnection of a single input or output endpoint. If
         * \a forward is true, then the disconnection is initiated by the input
         * endpoint and channel must be a connected input. Otherwise, it has been
         * initiated by the output endpoint and channel must be a connected output.
         * The disconnect call is only forwarded to the opposite side of the channel
         * element after the last input/output element on either side has been removed.
         */
        virtual bool disconnect(bool forward, const ChannelElementBase::shared_ptr& channel);

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
        RTT::os::SharedMutex inputs_lock;

    public:
        /**
         * Adds a new input to this element. This method replaces ChannelElementBase::setInput(),
         * for a MultipleInputsChannelElementBase which can have multiple inputs.
         * @param input the previous element in chain.
         */
        virtual void addInput(ChannelElementBase::shared_ptr input);

        /**
         * Overwritten implementation of \ref ChannelElementBase::setInput() which forwards to \ref addInput()
         * for instances of MultipleInputsChannelElementBase.
         * @param input the previous element in chain.
         */
        virtual void setInput(ChannelElementBase::shared_ptr input);

        /**
         * Returns true, if this channel element has at least one input, independent of whether is has an
         * output connection or not.
         */
        virtual bool connected();

        /**
         * Overwritten implementation of \ref ChannelElementBase::inputReady().
         * Simply returns true if there is at least one input, without forwarding to call.
         */
        virtual bool inputReady();

        /**
         * Overwritten implementation of \ref ChannelElementBase::clear().
         * Forwards the clear() call to all inputs.
         */
        virtual void clear();

        using ChannelElementBase::disconnect;

        /**
         * Overwritten implementation of \ref ChannelElementBase::disconnect(forward, channel).
         */
        virtual bool disconnect(bool forward, const ChannelElementBase::shared_ptr& channel);

    protected:
        /**
         * Remove an input from the inputs list.
         * @param input the element to be removed
         */
        virtual void removeInput(ChannelElementBase *input);
    };

    /**
     * ChannelElementBase variant with multiple output channels.
     */
    class RTT_API MultipleOutputsChannelElementBase : virtual public ChannelElementBase
    {
    public:
        typedef boost::intrusive_ptr<MultipleOutputsChannelElementBase> shared_ptr;
        typedef std::list<ChannelElementBase::shared_ptr> Outputs;

    protected:
        Outputs outputs;
        RTT::os::SharedMutex outputs_lock;

    public:
        /**
         * Adds a new input to this element. This method replaces ChannelElementBase::setOutput(),
         * for a MultipleOutputsChannelElementBase which can have multiple outputs.
         * @param input the previous element in chain.
         */
        virtual void addOutput(ChannelElementBase::shared_ptr output);

        /**
         * Overwritten implementation of \ref ChannelElementBase::setOutput() which forwards to \ref addOutput()
         * for instances of MultipleOutputsChannelElementBase.
         * @param output the next element in chain.
         */
        virtual void setOutput(ChannelElementBase::shared_ptr output);

        /**
         * Returns true, if this channel element has at least one output, independent of whether is has an
         * input connection or not.
         */
        virtual bool connected();

        /**
         * Overwritten implementation of \ref ChannelElementBase::signal() which forwards the signal to all
         * outputs.
         */
        virtual bool signal();

        using ChannelElementBase::disconnect;

        /**
         * Overwritten implementation of \ref ChannelElementBase::disconnect(forward, channel).
         */
        virtual bool disconnect(bool forward, const ChannelElementBase::shared_ptr& channel);

    protected:
        /**
         * Remove an output from the outputs list.
         * @param output the element to be removed
         */
        virtual void removeOutput(ChannelElementBase *output);
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

        /**
         * Overwritten implementation of \ref ChannelElementBase::disconnect(forward, channel).
         */
        virtual bool disconnect(bool forward, const ChannelElementBase::shared_ptr& channel);
    };

    void RTT_API intrusive_ptr_add_ref( ChannelElementBase* e );
    void RTT_API intrusive_ptr_release( ChannelElementBase* e );

}}

#endif

