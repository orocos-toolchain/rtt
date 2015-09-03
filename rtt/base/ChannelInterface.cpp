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

void ChannelElementBase::disconnect(bool forward, ChannelElementBase *)
{
    if (forward)
    {
        shared_ptr output = getOutput();
        if (output)
            output->disconnect(true, this);
    }
    else
    {
        shared_ptr input = getInput();
        if (input)
            input->disconnect(false, this);
    }

    { RTT::os::MutexLock lock(inout_lock);
        this->input = 0;
        this->output = 0;
    }
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

internal::ConnID* ChannelElementBase::getConnID() const {
    return 0;
}

void MultipleInputsChannelElementBase::addInput(shared_ptr input)
{
    RTT::os::MutexLock lock(inputs_lock);
    assert(std::find(inputs.begin(), inputs.end(), input) == inputs.end());
    inputs.push_back(input);
}

void MultipleInputsChannelElementBase::setInput(shared_ptr input)
{
    if (!input) return;
    addInput(input);
}

bool MultipleInputsChannelElementBase::inputReady()
{
    RTT::os::SharedMutexLock lock(inputs_lock);
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

void MultipleInputsChannelElementBase::disconnect(bool forward, ChannelElementBase *caller)
{
    RTT::os::MutexLock lock(inputs_lock);

    if (forward) {
        // Simply remove the caller from the inputs list
        Inputs::iterator found = std::find(inputs.begin(), inputs.end(), caller);
        if (found != inputs.end()) {
            removeInput(found->get());
        }

        // If this has been the last input, disconnect the output, too.
        if (inputs.empty()) {
            ChannelElementBase::disconnect(true, caller);
        }

    } else {
        // Disconnect and remove all inputs
        for (Inputs::iterator it = inputs.begin(); it != inputs.end(); ) {
            const shared_ptr &input = *it++;
            input->disconnect(false, this);
            removeInput(input.get());
        }
        assert(inputs.empty());
        ChannelElementBase::disconnect(false, caller);
    }
}

void MultipleOutputsChannelElementBase::addOutput(shared_ptr output)
{
    RTT::os::MutexLock lock(outputs_lock);
    assert(std::find(outputs.begin(), outputs.end(), output) == outputs.end());
    outputs.push_back(output);
}


void MultipleOutputsChannelElementBase::setOutput(shared_ptr output)
{
    if (!output) return;
    addOutput(output);
    output->setInput(this);
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

void MultipleOutputsChannelElementBase::disconnect(bool forward, ChannelElementBase *caller)
{
    RTT::os::MutexLock lock(outputs_lock);

    if (!forward) {
        // Simply remove the caller from the outputs list
        Outputs::iterator found = std::find(outputs.begin(), outputs.end(), caller);
        if (found != outputs.end()) {
            removeOutput(found->get());
        }

        // If this has been the last output, disconnect the input, too.
        if (outputs.empty()) {
            ChannelElementBase::disconnect(false, caller);
        }

    } else {
        // Disconnect and remove all outputs
        for (Outputs::iterator it = outputs.begin(); it != outputs.end(); ) {
            const shared_ptr &output = *it++;
            output->disconnect(true, this);
            removeOutput(output.get());
        }
        assert(outputs.empty());
        ChannelElementBase::disconnect(true, caller);
    }
}

void MultipleInputsMultipleOutputsChannelElementBase::disconnect(bool forward, ChannelElementBase *caller)
{
   if (forward) {
        MultipleInputsChannelElementBase::disconnect(forward, caller);

        // Also disconnect all outputs if no inputs are left:
        RTT::os::SharedMutexLock lock(inputs_lock);
        if (inputs.empty())
        {
            MultipleOutputsChannelElementBase::disconnect(forward, caller);
        }

    } else {
        MultipleOutputsChannelElementBase::disconnect(forward, caller);

        // Also disconnect all inputs if no outputs are left:
        RTT::os::SharedMutexLock lock(outputs_lock);
        if (outputs.empty())
        {
            MultipleInputsChannelElementBase::disconnect(forward, caller);
        }
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

