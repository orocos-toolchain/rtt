/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  RemoteConnID.cpp

                        RemoteConnID.cpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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


/*
 * RemoteConnID.cpp
 *
 *  Created on: Oct 14, 2009
 *      Author: kaltan
 */

#include "rtt/transports/corba/RemoteConnID.hpp"

using namespace RTT::corba;

RemoteConnID::RemoteConnID(CDataFlowInterface_ptr dataflow, std::string const& name)
: dataflow(CDataFlowInterface::_duplicate(dataflow))
  , name(name) {}

bool RemoteConnID::isSameID(ConnID const& id) const
        {
    RemoteConnID const* real_id = dynamic_cast<RemoteConnID const*>(&id);
    if (!real_id) return false;
    return real_id->dataflow->_is_equivalent(dataflow) && real_id->name == name;
        }

RTT::internal::ConnID* RemoteConnID::clone() const {
    return new RemoteConnID( this->dataflow.in(), this->name);
}
