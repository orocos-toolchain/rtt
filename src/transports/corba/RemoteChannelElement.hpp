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
                // Big note about cleanup: The RTT will dispose this object through
	            // the ChannelElement<T> refcounting. So we only need to inform the
                // POA that our object is dead in disconnect().
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
            bool signal()
            {
                try
                { return remote_side->remoteSignal(); }
                catch(CORBA::Exception&)
                { return false; }
            }

            void remoteDisconnect(bool writer_to_reader)
            {
                base::ChannelElement<T>::disconnect(writer_to_reader);

                // Because we support out-of-band transports, we must cleanup more thoroughly.
                // an oob channel may be sitting at our other end. If not, this is a nop.
                base::ChannelElement<T>::disconnect(!writer_to_reader);

                remote_side = 0;
                PortableServer::ObjectId_var oid=mpoa->servant_to_id(this);
                mpoa->deactivate_object(oid.in());
            }

            void disconnect(bool writer_to_reader)
            {
                try { remote_side->remoteDisconnect(writer_to_reader); }
                catch(CORBA::Exception&) {}
                base::ChannelElement<T>::disconnect(writer_to_reader);
                remote_side = 0;
                PortableServer::ObjectId_var oid=mpoa->servant_to_id(this);
                mpoa->deactivate_object(oid.in());
            }

            FlowStatus read(typename base::ChannelElement<T>::reference_t sample)
            {
                // try to read locally first
                FlowStatus fs;
                CFlowStatus cfs;
                if ( (fs = base::ChannelElement<T>::read(sample)) )
                    return fs;
                // go through corba
                CORBA::Any_var remote_value;
                try
                {
                    if ( (cfs = remote_side->read(remote_value) ) )
                    {
                        transport.updateFromAny(&remote_value.in(), data_source);
                        sample = data_source->rvalue();
                        return (FlowStatus)cfs;
                    }
                    else
                        return NoData;
                }
                catch(CORBA::Exception&) { return NoData; }
            }

            CFlowStatus read(::CORBA::Any_out sample)
            {
                FlowStatus fs;
                if ( (fs = base::ChannelElement<T>::read(data_source->set())) )
                {
                    sample = transport.createAny(data_source);
                    return (CFlowStatus)fs;
                }
                else
                    return CNoData;
            }

            bool write(typename base::ChannelElement<T>::param_t sample)
            {
                data_source->set(sample);
                // try to write locally first
                if (base::ChannelElement<T>::write(sample))
                    return true;
                // go through corba
                CORBA::Any_var ret = transport.createAny(data_source);
                assert( remote_side.in() != 0 );
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
                transport.updateFromAny(&sample, data_source);
                return base::ChannelElement<T>::write(data_source->rvalue());
            }

            virtual bool data_sample(typename base::ChannelElement<T>::param_t sample)
            {
                // we don't pass it on through CORBA (yet).
                // If an oob transport is used, that one will send it through.
                if (this->output)
                    return base::ChannelElement<T>::data_sample(sample);
                return true;
            }

            virtual bool inputReady() {
                // signal to oob transport if any.
                if (this->input)
                    return base::ChannelElement<T>::inputReady();
                return true;
            }

        };
    }
}

#endif

