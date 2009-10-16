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
	     * @param sender The data flow interface which will be sending or receiving this channel.
	     * @param poa The POA to manage the server code.
	     * @return the created CChannelElement_i.
	     */
	    virtual CRemoteChannelElement_i* createChannelElement_i(interface::DataFlowInterface* sender, ::PortableServer::POA* poa, bool is_pull) const = 0;

	    /**
	     * The CORBA transport does not support creating 'CORBA' streams.
	     * @return null
	     */
        virtual base::ChannelElementBase* createStream( base::PortInterface* /*port*/, const ConnPolicy& /*policy*/, bool /*is_sender*/) const;
	};
    }
}

#endif

