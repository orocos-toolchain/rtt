/*
 * RemoteConnID.hpp
 *
 *  Created on: Oct 14, 2009
 *      Author: kaltan
 */

#include "../../internal/ConnID.hpp"
#include "DataFlowC.h"
#include <string>

#ifndef REMOTECONNID_HPP_
#define REMOTECONNID_HPP_

namespace RTT
{
    namespace corba
    {
        /**
         * Represents a connection to a remote CORBA port.
         */
        struct RemoteConnID : public internal::ConnID
        {
            const CDataFlowInterface_var dataflow;
            const std::string name;

            RemoteConnID(CDataFlowInterface_ptr dataflow, std::string const& name);

            bool isSameID(ConnID const& id) const;
            ConnID* clone() const;
        };

    }

}

#endif /* REMOTECONNID_HPP_ */
