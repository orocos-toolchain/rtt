/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:09:48 CET 2006  TaskContextFactory.cpp

                        TaskContextFactory.cpp -  description
                           -------------------
    begin                : Wed January 18 2006
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


#include "rtt/transports/corba/TaskContextFactory.hpp"
#include "rtt/transports/corba/TaskContextProxy.hpp"

bool RTT::corba::TaskContextFactory::InitOrb(int argc, char* argv[], Seconds orb_timeout)
{
  return RTT::corba::TaskContextProxy::InitOrb(argc, argv, orb_timeout);
}

void RTT::corba::TaskContextFactory::DestroyOrb()
{
  RTT::corba::TaskContextProxy::DestroyOrb();
}

RTT::TaskContext* RTT::corba::TaskContextFactory::Create(std::string name, bool is_ior)
{
  return RTT::corba::TaskContextProxy::Create(name, is_ior);
}

RTT::TaskContext* RTT::corba::TaskContextFactory::CreateFromFile(std::string filename)
{
  return RTT::corba::TaskContextProxy::CreateFromFile(filename);
}
