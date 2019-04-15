/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  RemoteChannelElement.hpp

                        RemoteChannelElement.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef CORBA_REMOTE_CHANNEL_ELEMENT_H
#define CORBA_REMOTE_CHANNEL_ELEMENT_H

#include "DataFlowI.h"
#include "CorbaTypeTransporter.hpp"
#include "CorbaDispatcher.hpp"
#include "ApplicationServer.hpp"

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

	    /**
	     * Becomes false if we couldn't transfer data to remote
	     */
	    bool valid;
	    /**
	     * In pull mode, we don't send data, just signal it and remote must read it back.
	     */
	    bool pull;
            CorbaDispatcher* mdispatcher;
            PortableServer::ObjectId_var oid;
            std::string localUri;

	public:
	    /**
	     * Create a channel element for remote data exchange.
	     * @param transport The type specific object that will be used to marshal the data.
	     * @param poa The POA that manages the underlying CRemoteChannelElement_i.
	     */
	    RemoteChannelElement(std::string const& dispatcherName, CorbaTypeTransporter const& transport, PortableServer::POA_ptr poa, bool is_pull)
                : CRemoteChannelElement_i(transport, poa)
                , valid(true), pull(is_pull)
                , mdispatcher(CorbaDispatcher::Acquire(dispatcherName))
            {
                // Big note about cleanup: The RTT will dispose this object through
	            // the ChannelElement<T> refcounting. So we only need to inform the
                // POA that our object is dead in disconnect().
                // CORBA refcount-managed servants must start with a refcount of
                // 1
                this->ref();
                oid = mpoa->activate_object(this);
                
                localUri = ApplicationServer::orb->object_to_string(_this());
            }
	    /**
	     * Create a channel element for remote data exchange.
	     * @param transport The type specific object that will be used to marshal the data.
	     * @param poa The POA that manages the underlying CRemoteChannelElement_i.
	     */
	    RemoteChannelElement(CorbaTypeTransporter const& transport, PortableServer::POA_ptr poa, bool is_pull)
                : CRemoteChannelElement_i(transport, poa)
                , valid(true), pull(is_pull)
                , mdispatcher(0)
            {
                // Big note about cleanup: The RTT will dispose this object through
	            // the ChannelElement<T> refcounting. So we only need to inform the
                // POA that our object is dead in disconnect().
                // CORBA refcount-managed servants must start with a refcount of
                // 1
                this->ref();
                oid = mpoa->activate_object(this);
                
                localUri = ApplicationServer::orb->object_to_string(_this());
            }

            ~RemoteChannelElement()
            {
                if (mdispatcher)
                    CorbaDispatcher::Release(mdispatcher);
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
            void remoteSignal() ACE_THROW_SPEC ((
          	      CORBA::SystemException
          	    ))
            { base::ChannelElement<T>::signal(); }

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
                mdispatcher->dispatchChannel( this );

                return valid;
            }

            virtual void transferSamples() {
                if (!valid)
                    return;
                //log(Debug) <<"transfering..." <<endlog();
                // in push mode, transfer all data, in pull mode, only signal once for each sample.
                if ( pull ) {
                    try
                    { remote_side->remoteSignal(); }
#ifdef CORBA_IS_OMNIORB
                    catch(CORBA::SystemException& e)
                    {
                        log(Error) << "caught CORBA exception while signalling our remote endpoint: " << e._name() << " " << e.NP_minorString() << endlog();
                        valid = false;
                    }
#endif
                    catch(CORBA::Exception& e)
                    {
                        log(Error) << "caught CORBA exception while signalling our remote endpoint: " << e._name() << endlog();
                        valid = false;
                    }
                } else {
                    /** This is used on to read the channel */
                    typename base::ChannelElement<T>::value_t sample;

                    //log(Debug) <<"...read..."<<endlog();
                    while ( this->read(sample, false) == NewData && valid) {
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
            void disconnect() ACE_THROW_SPEC ((
          	      CORBA::SystemException
          	    )) {
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

            void remoteDisconnect(bool writer_to_reader) ACE_THROW_SPEC ((
          	      CORBA::SystemException
          	    ))
            {
                base::ChannelElement<T>::disconnect(writer_to_reader);

                // Because we support out-of-band transports, we must cleanup more thoroughly.
                // an oob channel may be sitting at our other end. If not, this is a nop.
                base::ChannelElement<T>::disconnect(!writer_to_reader);

                // Will fail at shutdown if all objects are already deactivated
                try {
                    if (mdataflow)
                        mdataflow->deregisterChannel(_this());
                    mpoa->deactivate_object(oid);
                }
                catch(CORBA::Exception&) {}
            }

            /**
             * CORBA IDL function.
             */
            void disconnect(bool writer_to_reader) ACE_THROW_SPEC ((
          	      CORBA::SystemException
          	    ))
            {
                try {
                    if ( ! CORBA::is_nil(remote_side.in()) )
                        remote_side->remoteDisconnect(writer_to_reader);
                }
                catch(CORBA::Exception&) {}

                base::ChannelElement<T>::disconnect(writer_to_reader);

                // Will fail at shutdown if all objects are already deactivated
                try {
                    if (mdataflow)
                        mdataflow->deregisterChannel(_this());
                    mpoa->deactivate_object(oid);
                }
                catch(CORBA::Exception&) {}
            }

            FlowStatus read(typename base::ChannelElement<T>::reference_t sample, bool copy_old_data)
            {
                if (!valid)
                    return NoData;

                // try to read locally first
                FlowStatus fs;
                CFlowStatus cfs;
                if ( (fs = base::ChannelElement<T>::read(sample, copy_old_data)) )
                    return fs;

                // go through corba
                CORBA::Any_var remote_value;
                try
                {
                    if ( remote_side && (cfs = remote_side->read(remote_value, copy_old_data) ) )
                    {
                        if (cfs == CNewData || (cfs == COldData && copy_old_data)) {
                            internal::LateReferenceDataSource<T> ref_data_source(&sample);
                            ref_data_source.ref();
                            transport.updateFromAny(&remote_value.in(), &ref_data_source);
                        }
                        return (FlowStatus)cfs;
                    }
                    else
                        return NoData;
                }
#ifdef CORBA_IS_OMNIORB
                catch(CORBA::SystemException& e)
                {
                    log(Error) << "caught CORBA exception while reading a remote channel: " << e._name() << " " << e.NP_minorString() << endlog();
                    valid = false;
                    return NoData;
                }
#endif
                catch(CORBA::Exception& e)
                {
                    log(Error) << "caught CORBA exception while reading a remote channel: " << e._name() << endlog();
                    valid = false;
                    return NoData;
                }
            }

            /**
             * CORBA IDL function.
             */
            CFlowStatus read(::CORBA::Any_out sample, bool copy_old_data) ACE_THROW_SPEC ((
          	      CORBA::SystemException
          	    ))
            {

                FlowStatus fs;
                typename internal::ValueDataSource<T> value_data_source;
                value_data_source.ref();
                fs = base::ChannelElement<T>::read(value_data_source.set(), copy_old_data);
                if (fs == NewData || (fs == OldData && copy_old_data)) {
                    sample = transport.createAny(&value_data_source);
                    if ( sample != 0) {
                        return (CFlowStatus)fs;
                    }
                    // this is a programmatic error and should never happen during run-time.
                    log(Error) << "CORBA Transport failed to create Any for " << value_data_source.getTypeName() << " while it should have!" <<endlog();
                }
                // we *must* return something in sample.
                sample = new CORBA::Any();
                return (CFlowStatus)fs;
            }

            bool write(typename base::ChannelElement<T>::param_t sample)
            {
                // try to write locally first
                if (base::ChannelElement<T>::write(sample))
                    return true;
                // go through corba
                assert( remote_side.in() != 0 && "Got write() without remote side. Need buffer OR remote side but neither was present.");
                try
                {
                      /** This is used on the writing side, to avoid allocating an Any for
                       * each write
                       */
                    CORBA::Any write_any;
                    internal::LateConstReferenceDataSource<T> const_ref_data_source(&sample);
                    const_ref_data_source.ref();

                    // There is a trick. We allocate on the stack, but need to
                    // provide shared pointers. Manually increment refence count
                    // (the stack "owns" the object)
                    transport.updateAny(&const_ref_data_source, write_any);
                    remote_side->write(write_any); 
                    return true;
                }
#ifdef CORBA_IS_OMNIORB
                catch(CORBA::SystemException& e)
                {
                    log(Error) << "caught CORBA exception while marshalling: " << e._name() << " " << e.NP_minorString() << endlog();
                    return false;
                }
#endif
                catch(CORBA::Exception& e)
                {
                    log(Error) << "caught CORBA exception while marshalling: " << e._name() << endlog();
                    return false;
                }
            }

            /**
             * CORBA IDL function.
             */
            bool write(const ::CORBA::Any& sample) ACE_THROW_SPEC ((
          	      CORBA::SystemException
          	    ))
            {
                typename internal::ValueDataSource<T> value_data_source;
                value_data_source.ref();
                transport.updateFromAny(&sample, &value_data_source);
                return base::ChannelElement<T>::write(value_data_source.rvalue());
            }

            virtual bool data_sample(typename base::ChannelElement<T>::param_t sample)
            {
                // we don't pass it on through CORBA (yet).
                // If an oob transport is used, that one will send it through.
                typename base::ChannelElement<T>::shared_ptr output =
                    this->getOutput();
                if (output)
                    return base::ChannelElement<T>::data_sample(sample);
                return true;
            }

            /**
             * CORBA IDL function.
             */
            virtual bool inputReady() {
                // signal to oob transport if any.
                typename base::ChannelElement<T>::shared_ptr input =
                    this->getInput();
                if (input)
                    return base::ChannelElement<T>::inputReady();
                return true;
            }

            virtual bool isRemoteElement() const
            {
                return true;
            }
            
            virtual std::string getRemoteURI() const
            {
                //check for output element case
                RTT::base::ChannelElementBase *base = const_cast<RemoteChannelElement<T> *>(this);
                if(base->getOutput())
                    return RTT::base::ChannelElementBase::getRemoteURI();
                
                std::string uri = ApplicationServer::orb->object_to_string(remote_side);
                return uri;
            }
            
            virtual std::string getLocalURI() const
            {
                //check for input element case
                RTT::base::ChannelElementBase *base = const_cast<RemoteChannelElement<T> *>(this);
                if(base->getInput())
                    return RTT::base::ChannelElementBase::getLocalURI();
                
                return localUri;
            }
            
            virtual std::string getElementName() const
            {
                return "CorbaRemoteChannelElement";
            }
        };
    }
}

#endif

