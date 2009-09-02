#ifndef CORBA_TYPE_TRANSPORTER_H
#define CORBA_TYPE_TRANSPORTER_H

#include "DataFlowI.h"
#include "../../types/TypeTransporter.hpp"

namespace RTT {

    namespace corba {
    /**
     * Extends the TypeTransporter in order to allow the creation of
     * channel elements or output halves for a given type.
     */
	class CorbaTypeTransporter : public RTT::types::TypeTransporter
	{
	public:
	    /**
	     * Builds a channel element for remote transport in both directions.
	     * @param poa The POA to manage the server code.
	     * @return the created CChannelElement_i.
	     */
	    virtual CChannelElement_i* createChannelElement_i(PortableServer::POA_ptr poa) const = 0;

        virtual base::ChannelElementBase* createRemoteChannel( std::string /*name_id*/, void* arg, bool /*is_sender*/) const {
            return dynamic_cast<base::ChannelElementBase*>( this->createChannelElement_i( (PortableServer::POA_ptr)arg ) );
        }

	    /**
	     * Builds an connection output endpoint for the given input port.
	     * This is not CORBA specific !
	     *
	     * The standard ConnectionFactory is used for this.
	     * @param port The port needing an output endpoint
	     * @param policy The policy for the connection factory.
	     * @return a ChannelElement which must be added to the input port.
	     */
	    virtual base::ChannelElementBase* buildOutputHalf(base::InputPortInterface& port,
		    internal::ConnPolicy const& policy) const = 0;

	};
    }
}

#endif

