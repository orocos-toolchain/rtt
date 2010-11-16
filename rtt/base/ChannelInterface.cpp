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
    oro_atomic_set(&refcount,0);
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
{
    this->output = output;
    if (output)
        output->input = this;
}

void ChannelElementBase::disconnect(bool forward)
{
    if (forward)
    {
        shared_ptr output = getOutput();
        if (output)
            output->disconnect(true);
    }
    else
    {
        shared_ptr input = getInput();
        if (input)
            input->disconnect(false);
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

