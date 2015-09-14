/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  ChannelInterface.cpp

                        ChannelInterface.cpp -  description
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


#include "../internal/Channels.hpp"
#include "../os/Atomic.hpp"
#include "../os/MutexLock.hpp"

using namespace RTT;
using namespace RTT::detail;

ChannelElementBase::ChannelElementBase()
    : input(0)

{
    ORO_ATOMIC_SETUP(&refcount,0);
}

ChannelElementBase::~ChannelElementBase()
{
    ORO_ATOMIC_CLEANUP(&refcount);
}

ChannelElementBase::shared_ptr ChannelElementBase::getInput()
{ RTT::os::SharedMutexLock lock(inout_lock);
    return ChannelElementBase::shared_ptr(input);
}

ChannelElementBase::shared_ptr ChannelElementBase::getOutput()
{ RTT::os::SharedMutexLock lock(inout_lock);
    return ChannelElementBase::shared_ptr(output);
}

void ChannelElementBase::setOutput(shared_ptr output)
{
    {
        RTT::os::MutexLock lock(inout_lock);
        if (this->output == output) return;
        this->output = output;
    }

    if (output)
        output->setInput(this);
}

bool ChannelElementBase::addOutput(shared_ptr output)
{
    return false;
}

void ChannelElementBase::setInput(shared_ptr input)
{
    {
        RTT::os::MutexLock lock(inout_lock);
        if (this->input == input) return;
        this->input = input;
    }

    if (input)
        input->setOutput(this);
}

bool ChannelElementBase::addInput(shared_ptr output)
{
    return false;
}

bool ChannelElementBase::connected()
{ RTT::os::SharedMutexLock lock(inout_lock);
    return (this->input || this->output);
}

void ChannelElementBase::disconnect(bool forward)
{
    disconnect(0, forward);
}

bool ChannelElementBase::disconnect(const ChannelElementBase::shared_ptr&, bool forward)
{
    if (forward)
    {
        shared_ptr output = getOutput();
        if (output)
            if (!output->disconnect(this, true))
                return false;
    }
    else
    {
        shared_ptr input = getInput();
        if (input)
            if (!input->disconnect(this, false))
                return false;
    }

    { RTT::os::MutexLock lock(inout_lock);
        this->input = 0;
        this->output = 0;
    }
    return true;
}

ChannelElementBase::shared_ptr ChannelElementBase::getInputEndPoint()
{
    shared_ptr input = getInput();
    return input ? input->getInputEndPoint() : this;
}

ChannelElementBase::shared_ptr ChannelElementBase::getOutputEndPoint()
{
    shared_ptr output = getOutput();
    return output ? output->getOutputEndPoint() : this;
}

bool ChannelElementBase::channelReady(ChannelElementBase::shared_ptr const&, ConnPolicy const& policy, ConnID *conn_id)
{
    // we go in the direction of the data stream
    shared_ptr output = getOutput();
    return output ? output->channelReady(this, policy, conn_id) : false;
}

bool ChannelElementBase::inputReady(ChannelElementBase::shared_ptr const&)
{
    // we go against the data stream
    shared_ptr input = getInput();
    return input ? input->inputReady(this) : false;
}

void ChannelElementBase::clear()
{
    shared_ptr input = getInput();
    if (input)
        input->clear();
}

bool ChannelElementBase::signal()
{
    shared_ptr output = getOutput();
    if (output)
        return output->signal();
    return true;
}

PortInterface* ChannelElementBase::getPort() const {
    return 0;
}

const ConnPolicy* ChannelElementBase::getConnPolicy() const {
    return 0;
}

bool MultipleInputsChannelElementBase::addInput(ChannelElementBase::shared_ptr input)
{
    if (!input) return false;
    {
        RTT::os::MutexLock lock(inputs_lock);
        if (std::find(inputs.begin(), inputs.end(), input) != inputs.end()) return false;
        assert(std::find(inputs.begin(), inputs.end(), input) == inputs.end());
        inputs.push_back(input);
    }

    input->setOutput(this);
    return true;
}

void MultipleInputsChannelElementBase::setInput(ChannelElementBase::shared_ptr input)
{
    (void) addInput(input);
}

bool MultipleInputsChannelElementBase::connected()
{
    RTT::os::SharedMutexLock lock(inputs_lock);
    return inputs.size() > 0;
}

bool MultipleInputsChannelElementBase::inputReady(ChannelElementBase::shared_ptr const&)
{
    RTT::os::SharedMutexLock lock(inputs_lock);
    for (Inputs::const_iterator it = inputs.begin(); it != inputs.end(); ++it) {
        if (!(*it)->inputReady(this)) return false;
    }
    return !inputs.empty();
}

void MultipleInputsChannelElementBase::clear()
{
    RTT::os::SharedMutexLock lock(inputs_lock);
    for (Inputs::const_iterator it = inputs.begin(); it != inputs.end(); ++it) {
        (*it)->clear();
    }
}

void MultipleInputsChannelElementBase::removeInput(ChannelElementBase *input)
{
    inputs.remove(input);
}

bool MultipleInputsChannelElementBase::disconnect(const ChannelElementBase::shared_ptr& channel, bool forward)
{
    if (channel) {
        bool was_last = false;
        {
            // Remove the channel from the inputs list
            RTT::os::MutexLock lock(inputs_lock);
            Inputs::iterator found = std::find(inputs.begin(), inputs.end(), channel);
            if (found == inputs.end()) {
                return false;
            }
            ChannelElementBase::shared_ptr input = *found;

            if (!forward) {
                if (!input->disconnect(this, forward)) {
                    return false;
                }
            }

            removeInput(input.get()); // invalidates input
            was_last = inputs.empty();
        }

        // If the removed input was the last channel and forward is true, disconnect output side, too.
        if (was_last && forward) {
            disconnect(true);
        }

        return true;

    } else if (!forward) {
        // Disconnect and remove all inputs
        RTT::os::MutexLock lock(inputs_lock);
        for (Inputs::iterator it = inputs.begin(); it != inputs.end(); ) {
            const ChannelElementBase::shared_ptr &input = *it++;
            input->disconnect(this, false);
            removeInput(input.get()); // invalidates input
        }
        assert(inputs.empty());
    }

    return ChannelElementBase::disconnect(channel, forward);
}

bool MultipleOutputsChannelElementBase::connected()
{
    RTT::os::SharedMutexLock lock(outputs_lock);
    return outputs.size() > 0;
}

bool MultipleOutputsChannelElementBase::addOutput(ChannelElementBase::shared_ptr output)
{
    if (!output) return false;
    {
        RTT::os::MutexLock lock(outputs_lock);
        if (std::find(outputs.begin(), outputs.end(), output) != outputs.end()) return false;
        assert(std::find(outputs.begin(), outputs.end(), output) == outputs.end());
        outputs.push_back(output);
    }

    output->setInput(this);
    return true;
}

void MultipleOutputsChannelElementBase::setOutput(ChannelElementBase::shared_ptr output)
{
    (void) addOutput(output);
}

bool MultipleOutputsChannelElementBase::signal()
{
    RTT::os::SharedMutexLock lock(outputs_lock);
    for (Outputs::const_iterator output = outputs.begin(); output != outputs.end(); ++output) {
        (*output)->signal();
    }
    return ChannelElementBase::signal();
}

bool MultipleOutputsChannelElementBase::channelReady(ChannelElementBase::shared_ptr const&, ConnPolicy const& policy, internal::ConnID *conn_id)
{
    RTT::os::SharedMutexLock lock(outputs_lock);
    for (Outputs::const_iterator it = outputs.begin(); it != outputs.end(); ++it) {
        if (!(*it)->channelReady(this, policy, conn_id)) return false;
    }
    return !outputs.empty();
}

void MultipleOutputsChannelElementBase::removeOutput(ChannelElementBase *output)
{
    outputs.remove(output);
}

bool MultipleOutputsChannelElementBase::disconnect(const ChannelElementBase::shared_ptr& channel, bool forward)
{
    if (channel) {
        // Remove the channel from the outputs list
        bool was_last = false;
        {
            RTT::os::MutexLock lock(outputs_lock);
            Outputs::iterator found = std::find(outputs.begin(), outputs.end(), channel);
            if (found == outputs.end()) {
                return false;
            }
            ChannelElementBase::shared_ptr output = *found;

            if (forward) {
                if (!output->disconnect(this, forward)) {
                    return false;
                }
            }

            removeOutput(output.get()); // invalidates output
            was_last = outputs.empty();
        }

        // If the removed output was the last channel, disconnect input side, too.
        if (was_last && !forward) {
            disconnect(false);
        }

        return true;
    }

    if (forward) {
        // Disconnect and remove all outputs
        RTT::os::MutexLock lock(outputs_lock);
        for (Outputs::iterator it = outputs.begin(); it != outputs.end(); ) {
            const ChannelElementBase::shared_ptr &output = *it++;
            output->disconnect(this, true);
            removeOutput(output.get()); // invalidates output
        }
        assert(outputs.empty());
    }

    return ChannelElementBase::disconnect(channel, forward);
}

bool MultipleInputsMultipleOutputsChannelElementBase::connected()
{
    return (MultipleInputsChannelElementBase::connected() && MultipleOutputsChannelElementBase::connected());
}

bool MultipleInputsMultipleOutputsChannelElementBase::disconnect(const ChannelElementBase::shared_ptr& channel, bool forward)
{
    if (channel) {
        if (MultipleInputsChannelElementBase::disconnect(channel, forward)) {
            // channel was found in the inputs list
            return true;
        }

        if (MultipleOutputsChannelElementBase::disconnect(channel, forward)) {
            // channel was found in the outputs list
            return true;
        }

        // channel was neither an input nor an output
        return false;

    } else if (forward) {
        // disconnect all outputs
        return MultipleOutputsChannelElementBase::disconnect(0, forward);

    } else {
        // disconnect all inputs
        return MultipleInputsChannelElementBase::disconnect(0, forward);
    }
}

void ChannelElementBase::ref()
{
    oro_atomic_inc(&refcount);
}

void ChannelElementBase::deref()
{
    if ( oro_atomic_dec_and_test(&refcount) ) delete this;
}

void RTT::base::intrusive_ptr_add_ref( ChannelElementBase* p )
{ oro_atomic_inc(&p->refcount); }

void RTT::base::intrusive_ptr_release( ChannelElementBase* p )
{ if ( oro_atomic_dec_and_test(&p->refcount) ) delete p; }

