#include "Connections.hpp"
#include <os/Atomic.hpp>

using namespace RTT;

ConnElementBase::ConnElementBase()
    : writer(0)

{
    oro_atomic_set(&refcount,0);
}

void ConnElementBase::setReader(shared_ptr new_reader)
{
    reader = new_reader;
    reader->writer = this;
}

void ConnElementBase::removeWriter()
{ writer = 0; }
void ConnElementBase::removeReader()
{ reader = 0; }

void ConnElementBase::disconnect(bool writer_to_reader)
{
    if (writer_to_reader)
    {
        if (reader)
            reader->disconnect(true);
    }
    else
    {
        if (writer)
            writer->disconnect(false);
    }
}

ConnElementBase::shared_ptr ConnElementBase::getWriter()
{ return writer; }
ConnElementBase::shared_ptr ConnElementBase::getReader()
{ return reader; }

void ConnElementBase::clear()
{
    shared_ptr writer_ = writer;
    if (writer_) writer_->clear();
}

void ConnElementBase::signal() const
{ if (reader) reader->signal(); }

void ConnElementBase::ref()
{
    oro_atomic_inc(&refcount);
}

void ConnElementBase::deref()
{
    if ( oro_atomic_dec_and_test(&refcount) ) delete this;
}

void RTT::intrusive_ptr_add_ref( ConnElementBase* p )
{ p->ref(); }

void RTT::intrusive_ptr_release( ConnElementBase* p )
{ p->deref(); }

