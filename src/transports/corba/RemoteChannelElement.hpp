#ifndef CORBA_REMOTE_CHANNEL_ELEMENT_H
#define CORBA_REMOTE_CHANNEL_ELEMENT_H

#include "DataFlowI.h"
#include "CorbaTypeTransporter.hpp"

namespace RTT {

    namespace corba {

	/**
	 * Implements the CChannelElement of the CORBA IDL interface.
	 * It converts the C++ calls into CORBA calls and vice versa.
	 * A read will cause a call to the remote channel (which is of the
	 * same type of this RemoteChannelElement) which returns an Any
	 * with the data. A similar mechanism is in place for a write.
	 */
	template<typename T>
	class RemoteChannelElement 
	    : public CChannelElement_i
	    , public base::ChannelElement<T>
	{
	    typename internal::ValueDataSource<T>::shared_ptr data_source;

	public:
	    /**
	     * Create a channel element for remote data exchange.
	     * @param transport The type specific object that will be used to marshal the data.
	     * @param poa The POA that manages the underlying CChannelElement_i.
	     */
	    RemoteChannelElement(CorbaTypeTransporter const& transport, PortableServer::POA_ptr poa)
		: CChannelElement_i(transport, poa)
		, data_source(new internal::ValueDataSource<T>)
            {
                // CORBA refcount-managed servants must start with a refcount of
                // 1
                this->ref();
            }

            /** Increase the reference count, called from the CORBA side */
            void _add_ref()
            { this->ref(); }
            /** Decrease the reference count, called from the CORBA side */
            void _remove_ref()
            { this->deref(); }


            CORBA::Boolean remoteSignal()
            { return base::ChannelElement<T>::signal(); }
            bool signal() const
            {
                try
                { return remote_side->remoteSignal(); }
                catch(CORBA::Exception&)
                { return false; }
            }

            void remoteDisconnect(bool writer_to_reader)
            {
                base::ChannelElement<T>::disconnect(writer_to_reader);
                remote_side = 0;
                PortableServer::ObjectId_var oid=mpoa->servant_to_id(this);
                mpoa->deactivate_object(oid.in());
            }

            void disconnect(bool writer_to_reader)
            {
                try { remote_side->remoteDisconnect(writer_to_reader); }
                catch(CORBA::Exception&) {}

                remote_side = 0;
                PortableServer::ObjectId_var oid=mpoa->servant_to_id(this);
                mpoa->deactivate_object(oid.in());
            }

            bool read(typename base::ChannelElement<T>::reference_t sample)
            {
                CORBA::Any_var remote_value;
                try
                {
                    if (remote_side->read(remote_value))
                    {
                        transport.updateBlob(&remote_value.in(), data_source);
                        sample = data_source->rvalue();
                        return true;
                    }
                    else return false;
                }
                catch(CORBA::Exception&) { return false; }
            }

            CORBA::Boolean read(::CORBA::Any_out sample)
            {
                if (base::ChannelElement<T>::read(data_source->set()))
                {
                    sample = static_cast<CORBA::Any*>(transport.createBlob(data_source));
                    return true;
                }
                else return false;
            }

            bool write(typename base::ChannelElement<T>::param_t sample)
            {
                data_source->set(sample);
                CORBA::Any_var ret = static_cast<CORBA::Any*>(transport.createBlob(data_source));
                try
                {
                    remote_side->write(ret.in()); 
                    return true;
                }
                catch(CORBA::Exception& e)
                {
                    return false;
                }
            }

            bool write(const ::CORBA::Any& sample)
            {
                transport.updateBlob(&sample, data_source);
                return base::ChannelElement<T>::write(data_source->rvalue());
            }

        };
    }
}

#endif

