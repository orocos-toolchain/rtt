/***************************************************************************
  tag: FMTC  do nov 2 13:06:19 CET 2006  CorbaDataObject.hpp 

                        CorbaDataObject.hpp -  description
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
 
 
 
#ifndef ORO_CORBA_DATAOBJECT_HPP
#define ORO_CORBA_DATAOBJECT_HPP


#include "../DataObjectInterfaces.hpp"
#include "orbsvcs/CosEventChannelAdminC.h"
#include "orbsvcs/CosEventChannelAdminS.h"
#include "orbsvcs/CosEventCommC.h"
#include "orbsvcs/CosEventCommS.h"

namespace RTT
{ namespace Corba {


    /**
     * A class which serves local data and provides it to the Corba Event Service as
     * well. 
     *
     * It is a servant in the Corba PullSupplier role: it allows
     * remote consumers (readers) to pull the latest value of this DataObject.
     * pull() and try_pull() are hence equivalent. 
     *
     * It is a servant in the Corba PushConsumer role as well: it allows
     * remote producers (writers) to push a new value to this DataObject.
     *
     * This object does \b not broadcast data. The remote client must initiate
     * reading or writing in order to know or change the value.
     *
     */
    template<class T>
    class CorbaDataObject
        : public DataObjectInterface<T>
    {
        /// Push consumer servant implementation.
        class CorbaDataObjectPushI
            : public virtual POA_CosEventComm::PushConsumer,
              public virtual PortableServer::RefCountServantBase
        {
            /// We are a servant to this implementation
            typename DataObjectInterface<T>::shared_ptr mimpl;

            //! the Channel
            CosEventChannelAdmin::EventChannel_var mec;
            //! We accept pushed data from a proxy supplier
            CosEventChannelAdmin::ProxyPushSupplier_var cproxy;

        public:
            /** 
             * Construct a CorbaDataObject which uses the Corba Event Service
             * as transport medium.
             * 
             * @param _name The name of this CorbaDataObject.
             * @param ec The event service which transfers data of type \a T
             */
            CorbaDataObjectPushI(typename DataObjectInterface<T>::shared_ptr impl, CosEventChannelAdmin::EventChannel_ptr ec )
                : mimpl(impl), mec(CosEventChannelAdmin::EventChannel::_duplicate(ec) ) 
            {
                log(Info) << "Creating Event Channel Push Consumer." << endlog();
                // This part lets suppliers write our data by calling push()
                CosEventChannelAdmin::ConsumerAdmin_var cadm = mec->for_consumers();
                cproxy = cadm->obtain_push_supplier();
                CosEventComm::PushConsumer_var puscon = POA_CosEventComm::PushConsumer::_this();
                cproxy->connect_push_consumer( puscon.in() );
            }

            /**
             * Destructor
             */
            ~CorbaDataObjectPushI() {
                try {
                    cproxy->disconnect_push_supplier();
                } catch(...) {}
            }

            virtual void push (
                               const ::CORBA::Any & data
                               )
                ACE_THROW_SPEC ((
                                 CORBA::SystemException,
                                 ::CosEventComm::Disconnected
                                 ))
            {
                Logger::In in("CorbaDataObjectPushI::push");
                log(Debug) << "Accepting DataObject value."<<endlog();
                if ( mimpl->update( data ) == false) {
                    Logger::log() <<Logger::Error << "Could not accept remote value: wrong data type."<<Logger::endl;
                    return;
                }
            }

            virtual void disconnect_push_consumer ()
                ACE_THROW_SPEC ((
                                 CORBA::SystemException
                                 ))
            {
                // TODO: do some refcounting magic to destroy this
            }

        };

        /// Pull supplier servant implementation.
        class CorbaDataObjectPullI
            : public virtual POA_CosEventComm::PullSupplier,
              public virtual PortableServer::RefCountServantBase
        {
            /// We are a servant to this implementation
            typename DataObjectInterface<T>::shared_ptr mimpl;

            //! the Channel
            CosEventChannelAdmin::EventChannel_var mec;
            //! We allow to pull our data by a proxy consumer
            CosEventChannelAdmin::ProxyPullConsumer_var sproxy;

        public:
            /** 
             * Construct a CorbaDataObject which uses the Corba Event Service
             * as transport medium.
             * 
             * @param _name The name of this CorbaDataObject.
             * @param ec The event service which transfers data of type \a T
             */
            CorbaDataObjectPullI(typename DataObjectInterface<T>::shared_ptr impl, CosEventChannelAdmin::EventChannel_ptr ec )
                : mimpl(impl), mec(CosEventChannelAdmin::EventChannel::_duplicate(ec) ) 
            {
                log(Info) << "Creating Event Channel Pull Supplier." << endlog();
                // This part lets consumers read our data by calling pull()
                CosEventChannelAdmin::SupplierAdmin_var sadm = mec->for_suppliers();
                sproxy = sadm->obtain_pull_consumer();
                // implicitly activate object.
                CosEventComm::PullSupplier_var pulsup = POA_CosEventComm::PullSupplier::_this();
                sproxy->connect_pull_supplier( pulsup.in() ); 
            }

            /**
             * Destructor
             */
            ~CorbaDataObjectPullI() {
                log(Info) << "Destroying Event Channel Pull Supplier." << endlog();
                try {
                    sproxy->disconnect_pull_consumer();
                } catch(...) {}
            }

            /**
             * This method returns the value of this->Get() as a CORBA::Any.
             */
            virtual CORBA::Any * pull ()
                ACE_THROW_SPEC ((
                                 CORBA::SystemException,
                                 ::CosEventComm::Disconnected
                                 )) 
            {
                Logger::In in("CorbaDataObjectPullI::pull");
                log(Debug) << "Returning DataObject value."<<endlog();
                ReferenceDataSource<T> rds( mimpl->Get() );
                rds.ref();
                CORBA::Any_var toset = rds.createAny( );
                return toset._retn();
            }

            /**
             * This method returns the value of this->Get() as a CORBA::Any.
             */
            virtual CORBA::Any * try_pull (
                                           ::CORBA::Boolean_out has_event
                                           )
                ACE_THROW_SPEC ((
                                 CORBA::SystemException,
                                 ::CosEventComm::Disconnected
                                 ))
            {
                Logger::In in("CorbaDataObjectPullI::try_pull");
                log(Debug) << "Returning DataObject value."<<endlog();
                has_event = 1;
                return this->pull();
            }

            virtual void disconnect_pull_supplier ()
                ACE_THROW_SPEC ((
                                 CORBA::SystemException
                                 ))
            {
                // TODO: do some refcounting magic to destroy this
            }
        };

        CosEventComm::PullSupplier_var pull;
        CosEventComm::PushConsumer_var push;

        /// We are a servant to this implementation
        typename DataObjectInterface<T>::shared_ptr mimpl;

        //! the Channel
        CosEventChannelAdmin::EventChannel_var mec;
    public:
        /** 
         * Construct a CorbaDataObject which uses the Corba Event Service
         * as transport medium.
         * 
         * @param _name The name of this CorbaDataObject.
         * @param ec The event service which transfers data of type \a T
         */
        CorbaDataObject(typename DataObjectInterface<T>::shared_ptr impl, CosEventChannelAdmin::EventChannel_ptr ec )
            : mimpl(impl), mec( CosEventChannelAdmin::EventChannel::_duplicate(ec) )
        {
            pull = (new CorbaDataObjectPullI(impl, ec ))->_this();
            push = (new CorbaDataObjectPushI(impl, ec ))->_this();
        }

        /**
         * Destructor
         */
        ~CorbaDataObject() {
        }

        CosEventComm::PullSupplier getPullSupplier() const {
            return CosEventComm::PullSupplier::_duplicate( pull );
        }

        CosEventComm::PushConsumer getPushConsumer() const {
            return CosEventComm::PushConsumer::_duplicate( push );
        }

        /**
         * The type of the data.
         */
        typedef T DataType;

        virtual DataType Get() const { return mimpl->Get(); }
        virtual void Get(DataType& d) const { return mimpl->Get(d); }
        virtual void Set(const DataType& s) { return mimpl->Set(s); }
        virtual const std::string& getName() const {
            return mimpl->getName();
        }
            
        CorbaDataObject<DataType>* clone() const {
            return new CorbaDataObject<DataType>( mimpl, mec.in() );
        }

        CorbaDataObject<DataType>* copy( std::map<const DataSourceBase*, DataSourceBase*>&  ) const {
            return const_cast<CorbaDataObject<DataType>*>(this);
        }

    };

}}

#endif
    
