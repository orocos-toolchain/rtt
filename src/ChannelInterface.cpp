#include "Channels.hpp"
#include <os/Atomic.hpp>

using namespace RTT;

ChannelElementBase::ChannelElementBase()
    : writer(0)

{
    oro_atomic_set(&refcount,0);
}

void ChannelElementBase::setReader(shared_ptr new_reader)
{
    reader = new_reader;
    reader->writer = this;
}

void ChannelElementBase::removeWriter()
{ writer = 0; }
void ChannelElementBase::removeReader()
{ reader = 0; }

void ChannelElementBase::disconnect(bool writer_to_reader)
{
    if (writer_to_reader)
    {
        shared_ptr reader = this->reader;
        if (reader)
            reader->disconnect(true);
    }
    else
    {
        shared_ptr writer = this->writer;
        if (writer)
            writer->disconnect(false);
    }
}

ChannelElementBase::shared_ptr ChannelElementBase::getWriter()
{ return writer; }
ChannelElementBase::shared_ptr ChannelElementBase::getReader()
{ return reader; }

void ChannelElementBase::clear()
{
    shared_ptr writer_ = writer;
    if (writer_) writer_->clear();
}

void ChannelElementBase::signal() const
{
    shared_ptr reader = this->reader;
    if (reader) reader->signal();
}

void ChannelElementBase::ref()
{
    oro_atomic_inc(&refcount);
}

void ChannelElementBase::deref()
{
    if ( oro_atomic_dec_and_test(&refcount) ) delete this;
}

void RTT::intrusive_ptr_add_ref( ChannelElementBase* p )
{ p->ref(); }

void RTT::intrusive_ptr_release( ChannelElementBase* p )
{ p->deref(); }

