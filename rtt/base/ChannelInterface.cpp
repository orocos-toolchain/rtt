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
{ RTT::os::MutexLock lock(inout_lock);
    return ChannelElementBase::shared_ptr(input);
}

ChannelElementBase::shared_ptr ChannelElementBase::getOutput()
{ RTT::os::MutexLock lock(inout_lock);
    return ChannelElementBase::shared_ptr(output);
}

void ChannelElementBase::setOutput(shared_ptr output)
{ RTT::os::MutexLock lock(inout_lock);
    this->output = output;
    if (output)
        output->setInput(this);
}

void ChannelElementBase::setInput(shared_ptr input)
{ RTT::os::MutexLock lock(inout_lock);
    this->input = input;
}

void ChannelElementBase::disconnect(bool forward)
{
    disconnect(forward, 0);
}

bool ChannelElementBase::disconnect(bool forward, const ChannelElementBase::shared_ptr& )
{
    if (forward)
    {
        shared_ptr output = getOutput();
        if (output)
            if (!output->disconnect(true, this))
                return false;
    }
    else
    {
        shared_ptr input = getInput();
        if (input)
            if (!input->disconnect(false, this))
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

bool ChannelElementBase::inputReady()
{
    // we go against the data stream
    shared_ptr input = getInput();
    if (input)
        return input->inputReady();
    return false;
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

void MultipleInputsChannelElementBase::addInput(ChannelElementBase::shared_ptr input)
{
    RTT::os::MutexLock lock(inputs_lock);
    assert(std::find(inputs.begin(), inputs.end(), input) == inputs.end());
    inputs.push_back(input);
}

void MultipleInputsChannelElementBase::setInput(ChannelElementBase::shared_ptr input)
{
    if (!input) return;
    addInput(input);
}

bool MultipleInputsChannelElementBase::inputReady()
{
    RTT::os::SharedMutexLock lock(inputs_lock);
    for (Inputs::const_iterator it = inputs.begin(); it != inputs.end(); ++it) {
        if (!(*it)->inputReady()) return false;
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

bool MultipleInputsChannelElementBase::disconnect(bool forward, const ChannelElementBase::shared_ptr& channel)
{
    if (channel) {
        // Remove the channel from the inputs list
        RTT::os::MutexLock lock(inputs_lock);
        Inputs::iterator found = std::find(inputs.begin(), inputs.end(), channel);
        if (found == inputs.end()) {
            return false;
        }
        ChannelElementBase::shared_ptr input = *found;

        if (!forward) {
            if (!input->disconnect(forward, this)) {
                return false;
            }
        }

        removeInput(input.get()); // invalidates input

        // If the removed input was the last channel, disconnect output side, too.
        // Warning: This may invert the propagation direction!
        if (inputs.empty()) disconnect(true);

        return true;

    } else if (!forward) {
        // Disconnect and remove all inputs
        RTT::os::MutexLock lock(inputs_lock);
        for (Inputs::iterator it = inputs.begin(); it != inputs.end(); ) {
            const ChannelElementBase::shared_ptr &input = *it++;
            input->disconnect(false, this);
            removeInput(input.get()); // invalidates input
        }
        assert(inputs.empty());
    }

    return ChannelElementBase::disconnect(forward, channel);
}

void MultipleOutputsChannelElementBase::addOutput(ChannelElementBase::shared_ptr output)
{
    if (!output) return;
    RTT::os::MutexLock lock(outputs_lock);
    assert(std::find(outputs.begin(), outputs.end(), output) == outputs.end());
    outputs.push_back(output);
    output->setInput(this);
}


void MultipleOutputsChannelElementBase::setOutput(ChannelElementBase::shared_ptr output)
{
    addOutput(output);
}

bool MultipleOutputsChannelElementBase::signal()
{
    RTT::os::SharedMutexLock lock(outputs_lock);
    for (Outputs::const_iterator output = outputs.begin(); output != outputs.end(); ++output) {
        (*output)->signal();
    }
    return ChannelElementBase::signal();
}

void MultipleOutputsChannelElementBase::removeOutput(ChannelElementBase *output)
{
    outputs.remove(output);
}

bool MultipleOutputsChannelElementBase::disconnect(bool forward, const ChannelElementBase::shared_ptr& channel)
{
    if (channel) {
        // Remove the channel from the outputs list
        RTT::os::MutexLock lock(outputs_lock);
        Outputs::iterator found = std::find(outputs.begin(), outputs.end(), channel);
        if (found == outputs.end()) {
            return false;
        }
        ChannelElementBase::shared_ptr output = *found;

        if (forward) {
            if (!output->disconnect(forward, this)) {
                return false;
            }
        }

        removeOutput(output.get()); // invalidates output

        // If the removed output was the last channel, disconnect input side, too.
        // Warning: This may invert the propagation direction!
        if (outputs.empty()) disconnect(false);

        return true;
    }

    if (forward) {
        // Disconnect and remove all outputs
        RTT::os::MutexLock lock(outputs_lock);
        for (Outputs::iterator it = outputs.begin(); it != outputs.end(); ) {
            const ChannelElementBase::shared_ptr &output = *it++;
            output->disconnect(true, this);
            removeOutput(output.get()); // invalidates output
        }
        assert(outputs.empty());
    }

    return ChannelElementBase::disconnect(forward, channel);
}

bool MultipleInputsMultipleOutputsChannelElementBase::disconnect(bool forward, const ChannelElementBase::shared_ptr& channel)
{
    if (channel) {
        if (MultipleInputsChannelElementBase::disconnect(forward, channel)) {
            // channel was found in the inputs list
            return true;
        }

        if (MultipleOutputsChannelElementBase::disconnect(forward, channel)) {
            // channel was found in the outputs list
            return true;
        }

        // channel was neither an input nor an output
        return false;

    } else if (forward) {
        // disconnect all outputs
        return MultipleOutputsChannelElementBase::disconnect(forward, 0);

    } else {
        // disconnect all inputs
        return MultipleInputsChannelElementBase::disconnect(forward, 0);
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
{ p->ref(); }

void RTT::base::intrusive_ptr_release( ChannelElementBase* p )
{ p->deref(); }

