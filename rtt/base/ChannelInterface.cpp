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
#include <boost/lexical_cast.hpp>

using namespace RTT;
using namespace RTT::detail;

ChannelElementBase::ChannelElementBase()
{
    ORO_ATOMIC_SETUP(&refcount,0);
}

ChannelElementBase::~ChannelElementBase()
{
    ORO_ATOMIC_CLEANUP(&refcount);
}

ChannelElementBase::shared_ptr ChannelElementBase::getInput()
{
    RTT::os::SharedMutexLock lock(input_lock);
    return input;
}

ChannelElementBase::shared_ptr ChannelElementBase::getOutput()
{
    RTT::os::SharedMutexLock lock(output_lock);
    return output;
}

bool ChannelElementBase::connectTo(ChannelElementBase::shared_ptr const& output, bool mandatory)
{
    if (!addOutput(output, mandatory)) return false;
    if (!output->addInput(this)) {
        (void) removeOutput(output);
        return false;
    }
    return true;
}

bool ChannelElementBase::addOutput(shared_ptr const& output, bool)
{
    RTT::os::MutexLock lock(output_lock);
    if (this->output == output) return true;
    if (output && this->output) return false;
    this->output = output;
    return true;
}

ChannelElementBase::shared_ptr ChannelElementBase::removeOutput(shared_ptr const& output)
{
    RTT::os::MutexLock lock(output_lock);
    ChannelElementBase::shared_ptr removed;
    if (!output || this->output == output) {
        this->output.swap(removed);
    }
    return removed;
}

bool ChannelElementBase::connectFrom(ChannelElementBase::shared_ptr const& input)
{
    if (!addInput(input)) return false;
    if (!input->addOutput(this)) {
        (void) removeInput(input);
        return false;
    }
    return true;
}

bool ChannelElementBase::addInput(shared_ptr const& input)
{
    RTT::os::MutexLock lock(input_lock);
    if (this->input == input) return true;
    if (input && this->input) return false;
    this->input = input;
    return true;
}

ChannelElementBase::shared_ptr ChannelElementBase::removeInput(shared_ptr const& input)
{
    RTT::os::MutexLock lock(input_lock);
    ChannelElementBase::shared_ptr removed;
    if (!input || this->input == input) {
        this->input.swap(removed);
    }
    return removed;
}

bool ChannelElementBase::connected()
{
    RTT::os::SharedMutexLock lock1(input_lock);
    RTT::os::SharedMutexLock lock2(output_lock);
    return (this->input || this->output);
}

void ChannelElementBase::disconnect(bool forward)
{
    disconnect(0, forward);
}

bool ChannelElementBase::disconnect(ChannelElementBase::shared_ptr const&, bool forward)
{
    if (forward)
    {
        shared_ptr output = removeOutput(shared_ptr());
        if (output) {
            return output->disconnect(this, true);
        }
    }
    else
    {
        shared_ptr input = removeInput(shared_ptr());
        if (input) {
            return input->disconnect(this, false);
        }
    }
    return false;
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

bool ChannelElementBase::inputReady()
{
    // we go against the data stream
    shared_ptr input = getInput();
    return input ? input->inputReady(this) : false;
}

bool ChannelElementBase::inputReady(ChannelElementBase::shared_ptr const&)
{
    return inputReady();
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
        return output->signalFrom(this);
    return true;
}

PortInterface* ChannelElementBase::getPort() const {
    return 0;
}

const ConnPolicy* ChannelElementBase::getConnPolicy() const {
    return 0;
}

bool ChannelElementBase::isRemoteElement() const {
    return false;
}

std::string ChannelElementBase::getRemoteURI() const {
    if(!output)
    {
        return std::string();
    }
    return output->getLocalURI();
}

std::string ChannelElementBase::getLocalURI() const {
    return std::string(boost::lexical_cast<std::string>(this));
}

std::string ChannelElementBase::getElementName() const {
    return std::string("ChannelElementBase");
}

MultipleInputsChannelElementBase::MultipleInputsChannelElementBase()
{}

bool MultipleInputsChannelElementBase::addInput(ChannelElementBase::shared_ptr const& input)
{
    if (!input) return false;
    RTT::os::MutexLock lock(inputs_lock);
    assert(std::find(inputs.begin(), inputs.end(), input) == inputs.end());
    if (std::find(inputs.begin(), inputs.end(), input) != inputs.end()) return false;
    inputs.push_back(input);
    return true;
}

ChannelElementBase::shared_ptr MultipleInputsChannelElementBase::removeInput(ChannelElementBase::shared_ptr const& input)
{
    RTT::os::MutexLock lock(inputs_lock);
    bool was_last = false;
    return removeInput(lock, input, was_last);
}

ChannelElementBase::shared_ptr MultipleInputsChannelElementBase::removeInput(
    RTT::os::MutexLock& lock, ChannelElementBase::shared_ptr const& input, bool& was_last)
{
    ChannelElementBase::shared_ptr removed;
    for(Inputs::iterator it = inputs.begin(); it != inputs.end(); ++it) {
        if (*it == input) {
            removed.swap(*it);
            inputs.erase(it);
            was_last = inputs.empty();
            break;
        }
    }
    return removed;
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

bool MultipleInputsChannelElementBase::disconnect(ChannelElementBase::shared_ptr const& channel, bool forward)
{
    if (channel) {
        if (disconnectSingleInputChannel(channel, forward)) {
            return true;
        }
    }

    if (!channel && !forward) {
        Inputs removedInputs;
        {
            RTT::os::MutexLock lock(inputs_lock);
            bool was_last = false;
            for (Inputs::iterator it = inputs.begin(); it != inputs.end();
                                  it = inputs.begin()) {
                removedInputs.push_back(removeInput(lock, *it, was_last));
            }
            assert(was_last);
        }

        for (Inputs::iterator it = removedInputs.begin(); it != removedInputs.end(); ++it) {
            (*it)->disconnect(this, false);
        }
    }

    return ChannelElementBase::disconnect(channel, forward);
}

bool MultipleInputsChannelElementBase::disconnectSingleInputChannel(ChannelElementBase::shared_ptr const& channel, bool forward)
{
    // Must remove first the item from the input list before we attempt the
    // disconnection, or another thread might try to remove it as well
    bool was_last = false;
    {
        RTT::os::MutexLock lock(inputs_lock);
        if (!removeInput(lock, channel, was_last)) {
            return false;
        }
    }

    if (!forward) {
        if (!channel->disconnect(this, forward /*= false*/)) {
            addInput(channel);
            return false;
        }
    }

    // If the removed input was the last channel, disconnect output side, too.
    if (was_last && forward) {
        return disconnect(0, forward /*= true*/);
    }

    return true;
}

bool MultipleInputsChannelElementBase::signalFrom(ChannelElementBase *)
{
    return signal();
}

MultipleOutputsChannelElementBase::MultipleOutputsChannelElementBase()
{}

MultipleOutputsChannelElementBase::Output::Output(ChannelElementBase::shared_ptr const &channel, bool mandatory)
    : channel(channel)
    , mandatory(mandatory)
    , disconnected(false)
{}

bool MultipleOutputsChannelElementBase::Output::operator==(ChannelElementBase::shared_ptr const& channel) const
{
    return (this->channel == channel);
}

bool MultipleOutputsChannelElementBase::addOutput(ChannelElementBase::shared_ptr const& output, bool mandatory)
{
    if (!output) return false;
    RTT::os::MutexLock lock(outputs_lock);
    // assert(std::find(outputs.begin(), outputs.end(), output) == outputs.end());
    if (std::find(outputs.begin(), outputs.end(), output) != outputs.end()) return false;
    outputs.push_back(Output(output, mandatory));
    return true;
}

ChannelElementBase::shared_ptr MultipleOutputsChannelElementBase::removeOutput(ChannelElementBase::shared_ptr const& output)
{
    RTT::os::MutexLock lock(outputs_lock);
    bool mandatory = false;
    bool was_last = false;
    return removeOutput(lock, output, was_last, mandatory);
}

ChannelElementBase::shared_ptr MultipleOutputsChannelElementBase::removeOutput(
    RTT::os::MutexLock& outputs_lock, ChannelElementBase::shared_ptr const& output, bool& was_last, bool& was_mandatory)
{
    ChannelElementBase::shared_ptr removed;
    for(Outputs::iterator it = outputs.begin(); it != outputs.end(); ++it) {
        if (it->channel == output) {
            was_mandatory = it->mandatory;
            removed.swap(it->channel);
            outputs.erase(it);
            was_last = outputs.empty();
            break;
        }
    }
    return removed;
}

bool MultipleOutputsChannelElementBase::connected()
{
    RTT::os::SharedMutexLock lock(outputs_lock);
    return outputs.size() > 0;
}

bool MultipleOutputsChannelElementBase::signal()
{
    RTT::os::SharedMutexLock lock(outputs_lock);
    for (Outputs::const_iterator output = outputs.begin(); output != outputs.end(); ++output) {
        output->channel->signalFrom(this);
    }
    return ChannelElementBase::signal();
}

bool MultipleOutputsChannelElementBase::channelReady(ChannelElementBase::shared_ptr const&, ConnPolicy const& policy, internal::ConnID *conn_id)
{
    RTT::os::SharedMutexLock lock(outputs_lock);
    for (Outputs::const_iterator it = outputs.begin(); it != outputs.end(); ++it) {
        if (!it->channel->channelReady(this, policy, conn_id)) return false;
    }
    return !outputs.empty();
}

bool MultipleOutputsChannelElementBase::disconnect(ChannelElementBase::shared_ptr const& channel, bool forward)
{
    if (channel) {
        if (disconnectSingleOutputChannel(channel, forward)) {
            return true;
        }
    }

    if (!channel && forward) {
        // Disconnect and remove all outputs
        Outputs outputs;
        {
            RTT::os::MutexLock lock(outputs_lock);
            outputs.splice(outputs.end(), this->outputs);
        }
        for (Outputs::iterator it = outputs.begin(); it != outputs.end(); ++it) {
            it->channel->disconnect(this, forward /*= true*/);
        }
    }

    return ChannelElementBase::disconnect(channel, forward);
}

bool MultipleOutputsChannelElementBase::disconnectSingleOutputChannel(ChannelElementBase::shared_ptr const& channel, bool forward)
{
    // Must remove first the item from the output list before we attempt the
    // disconnection, or another thread might try to remove it as well
    bool was_last = false, was_mandatory = false;
    {
        RTT::os::MutexLock lock(outputs_lock);
        if (!removeOutput(lock, channel, was_last, was_mandatory)) {
            return false;
        }
    }

    if (forward) {
        if (!channel->disconnect(this, forward /*= true*/)) {
            addOutput(channel, was_mandatory);
            return false;
        }
    }

    // If the removed output was the last channel, disconnect input side, too.
    if (was_last && !forward) {
        return disconnect(0, forward /*= false*/);
    }

    return true;
}

void MultipleOutputsChannelElementBase::removeDisconnectedOutputs()
{
    Outputs disconnectedOutputs;
    {
        RTT::os::MutexLock lock(outputs_lock);
        for (Outputs::iterator it = outputs.begin(); it != outputs.end(); ) {
            if (it->disconnected) {
                Outputs::iterator copy = it++;
                disconnectedOutputs.splice(disconnectedOutputs.end(), this->outputs, copy);
            } else {
                ++it;
            }
        }
    }

    for (Outputs::iterator it = disconnectedOutputs.begin(); it != disconnectedOutputs.end(); ++it) {
        it->channel->disconnect(this, true);
    }
}

bool MultipleInputsMultipleOutputsChannelElementBase::connected()
{
    return (MultipleInputsChannelElementBase::connected() && MultipleOutputsChannelElementBase::connected());
}

bool MultipleInputsMultipleOutputsChannelElementBase::disconnect(ChannelElementBase::shared_ptr const& channel, bool forward)
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

