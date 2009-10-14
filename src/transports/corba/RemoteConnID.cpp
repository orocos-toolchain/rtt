/*
 * RemoteConnID.cpp
 *
 *  Created on: Oct 14, 2009
 *      Author: kaltan
 */

#include "RemoteConnID.hpp"

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
