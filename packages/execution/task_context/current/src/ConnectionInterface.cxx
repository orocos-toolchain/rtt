/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:18 CET 2006  ConnectionInterface.cxx 

                        ConnectionInterface.cxx -  description
                           -------------------
    begin                : Thu March 02 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 

#include "rtt/ConnectionInterface.hpp"

namespace RTT
{

    ConnectionInterface::ConnectionInterface() { atomic_set(&refcount,0); }

    ConnectionInterface::~ConnectionInterface()
    {
    }

}
void intrusive_ptr_add_ref( RTT::ConnectionInterface* p )
{ 
    atomic_inc(&(p->refcount) ); 
}

void intrusive_ptr_release( RTT::ConnectionInterface* p )
{
    if ( atomic_dec_and_test(&(p->refcount) ) ) delete p;
}
