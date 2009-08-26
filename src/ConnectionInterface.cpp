/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:18 CET 2006  ConnectionInterface.cxx

                        ConnectionInterface.cxx -  description
                           -------------------
    begin                : Thu March 02 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

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



#include "ConnectionInterface.hpp"
#include "PortInterface.hpp"

namespace RTT
{

    ConnectionInterface::ConnectionInterface()
        : refcount(0), mconnected(false)
    {}

    ConnectionInterface::~ConnectionInterface()
    {
    }

    bool ConnectionInterface::connected() const
    {
        return mconnected;
    }

    bool ConnectionInterface::connect()
    {
        // note, this method is not thread-safe.
        if (!mconnected) {

            // first check existing connections:
            for ( PList::iterator it( ports.begin() ); it != ports.end(); ++it)
                if ((*it)->connected())
                    return false;

            // now do connect:
            for ( PList::iterator it( ports.begin() ); it != ports.end(); ++it)
                (*it)->connect(this);
            mconnected = true;
            return true;
        }
        return false;
    }

    bool ConnectionInterface::disconnect()
    {
        if (mconnected) {
            for ( PList::iterator it( ports.begin() ); it != ports.end(); ++it)
                (*it)->disconnect();
            mconnected = false;
            return true;
        }
        return false;
    }

    bool ConnectionInterface::addPort(PortInterface* r)
    {
        ports.push_back( r );
        if ( this->connected() )
            r->connect(this);
        return true;
    }

    bool ConnectionInterface::removePort(PortInterface* r) {
        PList::iterator it( find(ports.begin(), ports.end(), r ) );
        if ( it != ports.end() ) {
            ports.erase(it);
            return true;
        }
        return false;
    }

    void ConnectionInterface::signal()
    {
        for ( PList::iterator it( ports.begin() ); it != ports.end(); ++it)
        {
            int porttype = (*it)->getPortType();
            if (porttype == PortInterface::ReadPort || porttype == PortInterface::ReadWritePort)
                (*it)->signal();
        }
    }
}
void intrusive_ptr_add_ref( RTT::ConnectionInterface* p )
{
    p->refcount.inc();
}

void intrusive_ptr_release( RTT::ConnectionInterface* p )
{
    if ( p->refcount.dec_and_test() ) delete p;
}
