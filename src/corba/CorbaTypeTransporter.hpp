#ifndef CORBA_TYPE_TRANSPORTER_H
#define CORBA_TYPE_TRANSPORTER_H

#include "DataFlowI.h"
#include "../TypeTransporter.hpp"

namespace RTT {
    class ConnElementBase;
    class ConnPolicy;
    class ReadPortInterface;

    namespace Corba {
	class CorbaTypeTransporter : public RTT::detail::TypeTransporter
	{
	public:
	    virtual ConnElement_i* createConnElement_i(PortableServer::POA_ptr poa) const = 0;
	    virtual ConnElementBase* buildReaderHalf(RTT::ReadPortInterface& reader,
		    RTT::ConnPolicy const& policy) const = 0;

	};

        template<typename T>
	class RemoteConnElement 
	    : public ConnElement_i
	    , public RTT::ConnElement<T>
	{
	    typename RTT::ValueDataSource<T>::shared_ptr data_source;

	public:
	    RemoteConnElement(CorbaTypeTransporter const& transport, PortableServer::POA_ptr poa)
		: ConnElement_i(transport, poa)
		, data_source(new RTT::ValueDataSource<T>)
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


            void remoteSignal()
            { RTT::ConnElement<T>::signal(); }
            void signal() const
            { remote_side->remoteSignal(); }

            void remoteDisconnect(bool writer_to_reader)
            {
		RTT::ConnElement<T>::disconnect(writer_to_reader);
                remote_side = 0;
		PortableServer::ObjectId_var oid=mpoa->servant_to_id(this);
		mpoa->deactivate_object(oid.in());
	    }

            void disconnect(bool writer_to_reader)
            {
		remote_side->remoteDisconnect(writer_to_reader);
		remote_side = 0;
		PortableServer::ObjectId_var oid=mpoa->servant_to_id(this);
		mpoa->deactivate_object(oid.in());
	    }

            bool read(typename RTT::ConnElement<T>::reference_t sample)
            {
                ::CORBA::Any_var remote_value;
                if (remote_side->read(remote_value))
                {
                    transport.updateBlob(&remote_value.in(), data_source);
                    sample = data_source->rvalue();
                    return true;
                }
                else return false;
            }

            ::CORBA::Boolean read(::CORBA::Any_OUT_arg sample)
            {
                if (RTT::ConnElement<T>::read(data_source->set()))
                {
                    sample = static_cast<CORBA::Any*>(transport.createBlob(data_source));
                    return true;
                }
                else return false;
            }

            bool write(typename RTT::ConnElement<T>::param_t sample)
            {
                data_source->set(sample);
                CORBA::Any_var ret = static_cast<CORBA::Any*>(transport.createBlob(data_source));
                remote_side->write(ret.in());
		return true;
            }

            void write(const ::CORBA::Any& sample)
            {
                transport.updateBlob(&sample, data_source);
                RTT::ConnElement<T>::write(data_source->rvalue());
            }

        };
    }
}

#endif

