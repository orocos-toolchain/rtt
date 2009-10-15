#ifndef CORBA_REMOTE_CHANNEL_ELEMENT_H
#define CORBA_REMOTE_CHANNEL_ELEMENT_H

#include "DataFlowI.h"
#include "CorbaTypeTransporter.hpp"
#include "CorbaDispatcher.hpp"

namespace RTT {

    namespace corba {

	/**
	 * Implements the CRemoteChannelElement of the CORBA IDL interface.
	 * It converts the C++ calls into CORBA calls and vice versa.
	 * A read will cause a call to the remote channel (which is of the
	 * same type of this RemoteChannelElement) which returns an Any
	 * with the data. A similar mechanism is in place for a write.
	 */
	template<typename T>
	class RemoteChannelElement 
	    : public CRemoteChannelElement_i
	    , public base::ChannelElement<T>
	{
	    typename internal::ValueDataSource<T>::shared_ptr data_source;

	    /**
	     * Becomes false if we couldn't transfer data to remote
	     */
	    bool valid;
	    /**
	     * In pull mode, we don't send data, just signal it and remote must read it back.
	     */
	    bool pull;
	public:
	    /**
	     * Create a channel element for remote data exchange.
	     * @param transport The type specific object that will be used to marshal the data.
	     * @param poa The POA that manages the underlying CRemoteChannelElement_i.
	     */
	    RemoteChannelElement(CorbaTypeTransporter const& transport, PortableServer::POA_ptr poa, bool is_pull)
		: CRemoteChannelElement_i(transport, poa)
		, data_source(new internal::ValueDataSource<T>), valid(true), pull(is_pull)
            {
                // Big note about cleanup: The RTT will dispose this object through
	            // the ChannelElement<T> refcounting. So we only need to inform the
                // POA that our object is dead in disconnect().
                // CORBA refcount-managed servants must start with a refcount of
                // 1
                this->ref();
                // Force creation of dispatcher.
                CorbaDispatcher::Instance();
            }

            /** Increase the reference count, called from the CORBA side */
            void _add_ref()
            { this->ref(); }
            /** Decrease the reference count, called from the CORBA side */
            void _remove_ref()
            { this->deref(); }


            /**
             * CORBA IDL function.
             */
            CORBA::Boolean remoteSignal()
            { return base::ChannelElement<T>::signal(); }

            bool signal()
            {
                // forward too.
                base::ChannelElementBase::signal();
                // intercept signal if no remote side set.
                if ( CORBA::is_nil(remote_side.in()) )
                    return true;
                // Remember that signal() is called in the context of the one
                // that wrote the data, so we must decouple here to keep hard-RT happy.
                // the dispatch thread must read the data and send it over by calling transferSample().
                CorbaDispatcher::Instance()->dispatchChannel( this );

                return valid;
            }

            virtual void transferSamples() {
                if (!valid)
                    return;
                //log(Debug) <<"transfering..." <<endlog();
                // in push mode, transfer all data, in pull mode, only signal once for each sample.
                if ( pull ) {
                    try
                    { valid = remote_side->remoteSignal(); }
                    catch(CORBA::Exception&)
                    { valid = false; }
                } else {
                    typename base::ChannelElement<T>::value_t sample; // Not RT.
                    //log(Debug) <<"...read..."<<endlog();
                    while ( this->read(sample) == NewData && valid) {
                        //log(Debug) <<"...write..."<<endlog();
                        if ( this->write(sample) == false )
                            valid = false;
                        //log(Debug) <<"...next read?..."<<endlog();
                    }
                }
                //log(Debug) <<"... done." <<endlog();

            }

            /**
             * CORBA IDL function.
             */
            void disconnect() {
                // disconnect both local and remote side.
                // !!!THIS RELIES ON BEHAVIOR OF REMOTEDISCONNECT BELOW doing both writer_to_reader and !writer_to_reader !!!
                try {
                    if ( ! CORBA::is_nil(remote_side.in()) )
                        remote_side->remoteDisconnect(true);
                }
                catch(CORBA::Exception&) {}

                try { this->remoteDisconnect(true); }
                catch(CORBA::Exception&) {}
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

            /**
             * CORBA IDL function.
             */
            void disconnect(bool writer_to_reader)
            {
                try {
                    if ( ! CORBA::is_nil(remote_side.in()) )
                        remote_side->remoteDisconnect(writer_to_reader);
                }
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
                    if ( remote_side && (cfs = remote_side->read(remote_value) ) )
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

            /**
             * CORBA IDL function.
             */
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
                assert( remote_side.in() != 0 && "Got write() without remote side. Need buffer OR remote side but neither was present.");
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

            /**
             * CORBA IDL function.
             */
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

            /**
             * CORBA IDL function.
             */
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

