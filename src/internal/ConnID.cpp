/*
 * ConnID.cpp
 *
 *  Created on: Oct 14, 2009
 *      Author: kaltan
 */

#include "ConnID.hpp"

using namespace RTT::internal;

SimpleConnID::SimpleConnID(const ConnID* orig ) : cid( orig == 0 ? this : orig) {}
bool SimpleConnID::isSameID(ConnID const& id) const {
    SimpleConnID const* real_id = dynamic_cast<SimpleConnID const*>(&id);
    if (!real_id) return false;
    return real_id->cid == this->cid;

}
ConnID* SimpleConnID::clone() const {
    return new SimpleConnID( this->cid );
}
