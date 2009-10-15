/*
 * CorbaTypeTransporter.cpp
 *
 *  Created on: Sep 21, 2009
 *      Author: kaltan
 */

#include "CorbaTypeTransporter.hpp"
#include "DataFlowI.h"

using namespace RTT;
using namespace RTT::corba;

base::ChannelElementBase* CorbaTypeTransporter::createStream( base::PortInterface* /*port*/, std::string& /*name_id*/, int /*size_hint*/, bool /*is_sender*/) const {
    return 0;
}
