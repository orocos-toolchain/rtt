#include "../internal/Channels.hpp"
#include "../os/Atomic.hpp"

using namespace RTT;
using namespace RTT::detail;

ChannelElementBase::ChannelElementBase()
    : input(0)

{
    oro_atomic_set(&refcount,0);
}

void ChannelElementBase::setOutput(shared_ptr output)
{
    if (this->output) {
        this->output->input = 0;
    }
    this->output = output;
    if (output)
        output->input = this;
}

void ChannelElementBase::removeInput()
{ input = 0; }
void ChannelElementBase::removeOutput()
{ output = 0; }

void ChannelElementBase::disconnect(bool forward)
{
    if (forward)
    {
        shared_ptr output = this->output;
        this->output = 0;
        if (output)
            output->disconnect(true);
    }
    else
    {
        // this is a trick: we raise & lower refcount on input (a pure pointer) here
        // so if input was refcounted by no other before, we will delete it. Smart !
        shared_ptr input = this->input;
        this->input = 0;
        if (input)
            input->disconnect(false);
    }
}

ChannelElementBase* ChannelElementBase::getInput()
{ return input; }
ChannelElementBase::shared_ptr ChannelElementBase::getOutput()
{ return output; }

ChannelElementBase* ChannelElementBase::getInputEndPoint()
{ return input ? input->getInputEndPoint() : this; }

ChannelElementBase::shared_ptr ChannelElementBase::getOutputEndPoint()
{ return output ? output->getOutputEndPoint() : this; }

bool ChannelElementBase::inputReady()
{
    // we go against the data stream
    if (input)
        return input->inputReady();
    return false;
}

void ChannelElementBase::clear()
{
    if (input)
        input->clear();
}

bool ChannelElementBase::signal()
{
    shared_ptr output = this->output;
    if (output) return output->signal();
    return true;
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

