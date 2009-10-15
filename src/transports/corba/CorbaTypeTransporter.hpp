#ifndef CORBA_TYPE_TRANSPORTER_H
#define CORBA_TYPE_TRANSPORTER_H

#include "corba.h"
#include "../../types/TypeTransporter.hpp"

namespace RTT {

    namespace corba {

        class CRemoteChannelElement_i;

    /**
     * Extends the TypeTransporter in order to allow the creation of
     * channel elements or output halves for a given type.
     */
	class CorbaTypeTransporter : public RTT::types::TypeTransporter
	{
	public:
        /**
         * Create an transportable object for a \a protocol which contains the value of \a source.
         */
        virtual CORBA::Any_ptr createAny(base::DataSourceBase::shared_ptr source) const = 0;

        /**
         * Update \a target with the contents of \a blob which is an object of a \a protocol.
         */
        virtual bool updateFromAny(const CORBA::Any* blob, base::DataSourceBase::shared_ptr target) const = 0;


	    /**
	     * Builds a channel element for remote transport in both directions.
	     * @param poa The POA to manage the server code.
	     * @return the created CChannelElement_i.
	     */
	    virtual CRemoteChannelElement_i* createChannelElement_i(::PortableServer::POA* poa) const = 0;

        virtual base::ChannelElementBase* createChannel( base::PortInterface* /*port*/, std::string& /*name_id*/, int /*size_hint*/, bool /*is_sender*/) const;

	    /**
	     * Builds an connection output endpoint for the given input port.
	     * This is not CORBA specific !
	     *
	     * The standard ConnectionFactory is used for this.
	     * @param port The port needing an output endpoint
	     * @param policy The policy for the connection factory.
	     * @return a ChannelElement which must be added to the input port.
	     */
	    virtual base::ChannelElementBase* buildChannelOutput(base::InputPortInterface& port,
		    internal::ConnPolicy const& policy) const = 0;

        virtual base::ChannelElementBase* buildChannelInput(base::OutputPortInterface& port,
            internal::ConnPolicy const& policy) const = 0;
	};
    }
}

#endif

