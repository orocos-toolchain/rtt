/***************************************************************************
  tag: FMTC  do nov 2 13:06:18 CET 2006  CorbaBuffer.hpp 

                        CorbaBuffer.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
    email                : peter.soetens@fmtc.be
 
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
 
 

#ifndef ORO_CORBA_BUFFER_HPP
#define ORO_CORBA_BUFFER_HPP

#include "../BufferInterface.hpp"
#include "DataFlowI.h"
#include "DataFlowC.h"
#include "DataFlowS.h"
#include "orbsvcs/CosEventChannelAdminC.h"
#include "orbsvcs/CosEventCommC.h"


namespace RTT
{ namespace Corba {

    /**
     * A Buffer servant using a Corba Event Channel.
     * @ingroup CoreLibBuffers
     */
    template<class T>
    class CorbaBuffer
        : public virtual POA_RTT::Corba::BufferChannel,
          public virtual PortableServer::RefCountServantBase
    {

        class CorbaBufferPushI
            : public virtual POA_CosEventComm::PushConsumer,
              public virtual PortableServer::RefCountServantBase
        {
            typename BufferInterface<T>::shared_ptr mimpl;
            //! the Channel
            CosEventChannelAdmin::EventChannel_var mec;
            //! We accept pushed data from a proxy supplier
            CosEventChannelAdmin::ProxyPushSupplier_var cproxy;
        public:

            typedef typename ReadInterface<T>::reference_t reference_t;
            typedef typename WriteInterface<T>::param_t param_t;
            typedef typename BufferInterface<T>::size_type size_type;
            typedef T value_t;

            /**
             * Create a buffer which is accessible through the Corba Event Service
             * as transport medium.
             * @param ec The event service which transfers data of type \a T
             */
            CorbaBufferPushI(typename BufferInterface<T>::shared_ptr buf, CosEventChannelAdmin::EventChannel_ptr ec )
                : mimpl(buf), mec(CosEventChannelAdmin::EventChannel::_duplicate(ec) )
            {
                // This part lets suppliers write our data by calling push()
                CosEventChannelAdmin::ConsumerAdmin_var cadm = mec->for_consumers();
                cproxy = cadm->obtain_push_supplier();
                CosEventComm::PushConsumer_var puscon = POA_CosEventComm::PushConsumer::_this();
                cproxy->connect_push_consumer( puscon.in() );
            }

            /**
             * Destructor
             */
            ~CorbaBufferPushI() {
                try {
                    cproxy->disconnect_push_supplier();
                } catch(...) {}
            }

            virtual CosEventChannelAdmin::EventChannel_ptr getChannel()
                ACE_THROW_SPEC ((
                                 CORBA::SystemException
                                 )) 
            {
                return CosEventChannelAdmin::EventChannel::_duplicate(mec);
            }


            virtual void push (
                               const ::CORBA::Any & data
                               )
                ACE_THROW_SPEC ((
                                 CORBA::SystemException,
                                 ::CosEventComm::Disconnected
                                 ))
            {
                value_t new_value = value_t();
                ReferenceDataSource<T> rds( new_value );
                rds.ref();
                if ( rds.update( data ) == false) {
                    Logger::log() <<Logger::Error << "Could not accept remote value: wrong data type."<<Logger::endl;
                    return;
                }
                // if full, drop it.
                mimpl->Push( new_value );
            }

            virtual void disconnect_push_consumer ()
                ACE_THROW_SPEC ((
                                 CORBA::SystemException
                                 ))
            {
                // TODO: do some refcounting magic to destroy this
            }

        };

        class CorbaBufferPullI
            : public virtual POA_CosEventComm::PullSupplier,
              public virtual PortableServer::RefCountServantBase
        {
            typename BufferInterface<T>::shared_ptr mimpl;
            //! the Channel
            CosEventChannelAdmin::EventChannel_var mec;
            //! We allow to pull our data by a proxy consumer
            CosEventChannelAdmin::ProxyPullConsumer_var sproxy;
        public:

            typedef typename ReadInterface<T>::reference_t reference_t;
            typedef typename WriteInterface<T>::param_t param_t;
            typedef typename BufferInterface<T>::size_type size_type;
            typedef T value_t;

            /**
             * Create a buffer which is accessible through the Corba Event Service
             * as transport medium.
             * @param ec The event service which transfers data of type \a T
             */
            CorbaBufferPullI(typename BufferInterface<T>::shared_ptr buf, CosEventChannelAdmin::EventChannel_ptr ec )
                : mimpl(buf), mec(CosEventChannelAdmin::EventChannel::_duplicate(ec) )
            {
                // This part lets consumers read our data by calling pull()
                CosEventChannelAdmin::SupplierAdmin_var sadm = mec->for_suppliers();
                sproxy = sadm->obtain_pull_consumer();
                CosEventComm::PullSupplier_var pulsup = POA_CosEventComm::PullSupplier::_this();
                sproxy->connect_pull_supplier( pulsup.in() ); // implicitly activate object.
            }

            /**
             * Destructor
             */
            ~CorbaBufferPullI() {
                try {
                    sproxy->disconnect_pull_consumer();
                } catch(...) {}
            }

            /**
             * This method returns the value of front() as a CORBA::Any.
             */
            virtual CORBA::Any * pull ()
                ACE_THROW_SPEC ((
                                 CORBA::SystemException,
                                 ::CosEventComm::Disconnected
                                 )) 
            {
                ReferenceDataSource<T> rds(  mimpl->front()  );
                rds.ref();
                CORBA::Any_var toset = (CORBA::Any_ptr)rds.createBlob(ORO_CORBA_PROTOCOL_ID);
                return toset._retn();
            }

            /**
             * This method returns the value of this->Pop() as a CORBA::Any.
             */
            virtual CORBA::Any * try_pull (
                                           ::CORBA::Boolean_out has_event
                                           )
                ACE_THROW_SPEC ((
                                 CORBA::SystemException,
                                 ::CosEventComm::Disconnected
                                 ))
            {
                value_t data = value_t();
                has_event = mimpl->Pop( data );
                ReferenceDataSource<T> rds( data );
                rds.ref();
                CORBA::Any_var toset = (CORBA::Any_ptr)rds.createBlob(ORO_CORBA_PROTOCOL_ID);
                return toset._retn();
            }

            virtual void disconnect_pull_supplier ()
                ACE_THROW_SPEC ((
                                 CORBA::SystemException
                                 ))
            {
                // TODO: do some refcounting magic to destroy this
            }

        };

        typename BufferInterface<T>::shared_ptr mimpl;

        //! the Channel
        CosEventChannelAdmin::EventChannel_var mec;

        CosEventComm::PullSupplier_var pulsup;
        CosEventComm::PushConsumer_var puscon;
    public:

        typedef typename ReadInterface<T>::reference_t reference_t;
        typedef typename WriteInterface<T>::param_t param_t;
        typedef typename BufferInterface<T>::size_type size_type;
        typedef T value_t;

        /**
         * Create a buffer which is accessible through the Corba Event Service
         * as transport medium.
         * @param ec The event service which transfers data of type \a T
         */
        CorbaBuffer(typename BufferInterface<T>::shared_ptr buf, CosEventChannelAdmin::EventChannel_ptr ec )
            : mimpl(buf), mec(CosEventChannelAdmin::EventChannel::_duplicate(ec) )
        {
            pulsup = (new CorbaBufferPullI(buf, CosEventChannelAdmin::EventChannel::_duplicate(ec) ))->_this();
            puscon = (new CorbaBufferPushI(buf, CosEventChannelAdmin::EventChannel::_duplicate(ec) ))->_this();
        }

        /**
         * Destructor
         */
        ~CorbaBuffer() {
        }

        virtual CosEventChannelAdmin::EventChannel_ptr getChannel()
            ACE_THROW_SPEC ((
                             CORBA::SystemException
                             )) 
        {
            return CosEventChannelAdmin::EventChannel::_duplicate(mec);
        }

        virtual CORBA::Long capacity() 
            ACE_THROW_SPEC ((
                             CORBA::SystemException
                             ))
        {
            return mimpl->capacity();
        }

        virtual CORBA::Long size() 
            ACE_THROW_SPEC ((
                             CORBA::SystemException
                             ))
        {
            return mimpl->size();
        }

        virtual void clear() 
            ACE_THROW_SPEC ((
                             CORBA::SystemException
                             ))
        {
            mimpl->clear();
        }

        virtual CORBA::Boolean empty() 
            ACE_THROW_SPEC ((
                             CORBA::SystemException
                             ))
        {
            return mimpl->empty();
        }

        virtual CORBA::Boolean full() 
            ACE_THROW_SPEC ((
                             CORBA::SystemException
                             ))
        {
            return mimpl->full();
        }
    };
}}
#endif
