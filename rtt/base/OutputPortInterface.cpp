/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  OutputPortInterface.cpp

                        OutputPortInterface.cpp -  description
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


#include "PortInterface.hpp"
#include "OutputPortInterface.hpp"
#include "InputPortInterface.hpp"
#include <exception>
#include <stdexcept>
#include "../os/traces.h"

using namespace RTT;
using namespace RTT::detail;
using namespace std;


OutputPortInterface::OutputPortInterface(std::string const& name)
    : PortInterface(name), cmanager(this) { }

OutputPortInterface::~OutputPortInterface()
{
    cmanager.disconnect();
}

/** Returns true if this port is connected */
bool OutputPortInterface::connected() const
{ return cmanager.connected(); }

bool OutputPortInterface::disconnect(PortInterface* port)
{
    return cmanager.disconnect(port);
}

void OutputPortInterface::disconnect()
{
    cmanager.disconnect();
}

bool OutputPortInterface::addConnection(ConnID* port_id, ChannelElementBase::shared_ptr channel_input, ConnPolicy const& policy)
{
    if ( this->connectionAdded(channel_input, policy) ) {
        cmanager.addConnection(port_id, channel_input, policy);
        return true;
    }
    return false;
}

// This is called by our input endpoint.
bool OutputPortInterface::removeConnection(ConnID* conn)
{
    return cmanager.removeConnection(conn);
}


void OutputPortInterface::write(DataSourceBase::shared_ptr source)
{ throw std::runtime_error("calling default OutputPortInterface::write(datasource) implementation"); }

bool OutputPortInterface::createDataConnection( InputPortInterface& input, int lock_policy )
{ return createConnection( input, ConnPolicy::data(lock_policy) ); }

bool OutputPortInterface::createBufferConnection( InputPortInterface& input, int size, int lock_policy )
{ return createConnection( input, ConnPolicy::buffer(size, lock_policy) ); }

bool OutputPortInterface::createConnection( InputPortInterface& input )
{ return createConnection(input, input.getDefaultPolicy()); }

bool OutputPortInterface::connectTo(PortInterface* other, ConnPolicy const& policy)
{
    InputPortInterface* input  = dynamic_cast<InputPortInterface*>(other);
    if (! input)
        return false;
    return createConnection(*input, policy);
}

bool OutputPortInterface::connectTo(PortInterface* other)
{
    InputPortInterface* input  = dynamic_cast<InputPortInterface*>(other);
    if (! input)
        return false;
    return createConnection(*input);
}

void OutputPortInterface::traceWrite()
{
    tracepoint(orocos_rtt, OutputPort_write, cFullName);
}

