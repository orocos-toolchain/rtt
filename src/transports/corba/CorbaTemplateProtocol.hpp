/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:24 CET 2008  CorbaTemplateProtocol.hpp

                        CorbaTemplateProtocol.hpp -  description
                           -------------------
    begin                : Tue March 11 2008
    copyright            : (C) 2008 FMTC
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


#ifndef ORO_CORBA_TEMPATE_PROTOCOL_HPP
#define ORO_CORBA_TEMPATE_PROTOCOL_HPP

#include "CorbaTypeTransporter.hpp"
#include "RemoteChannelElement.hpp"
#include "../../types/Types.hpp"
#include "../../InputPort.hpp"
#include "../../OutputPort.hpp"
#include "ExpressionProxy.hpp"
#include "ExpressionServer.hpp"
#include "DataFlowI.h"
#include "../../internal/ConnID.hpp"

namespace RTT
{ namespace corba
  {
      /**
       * For each transportable type T, specify the conversion functions.
       *
       * @warning
       * * * * B I G  N O T E * * * *
       * This class uses the TypeTransport of Orocos which uses (void*) for
       * passing on data. TAO's CORBA implementation uses virtual inheritance,
       * which does not work well together with (void*). That is, you must cast
       * back to the exect same type the (void*) originated from and NOT to a
       * sub- or super-class. That would have been allowed without virtual inheritance.
       * @warning
       * Hence, this class uses always the same base class (CExpression_ptr) to
       * communicate with the TypeTransport interface. Such that we know that when
       * we receive a (void*) it came from an (CExpression_ptr) and vice versa.
       * @warning
       * Don't obey this and you'll get immediate hard to dissect crashes !
       * * * * B I G  N O T E * * * *
       */
      template<class T>
      class CorbaTemplateProtocol
          : public CorbaTypeTransporter
      {
      public:
          /**
           * The given \a T parameter is the type for reading DataSources.
           */
          typedef T UserType;
          /**
           * When Properties of \a T are constructed, they are non-const, non-reference.
           */
          typedef typename Property<T>::DataSourceType PropertyType;

          CRemoteChannelElement_i* createChannelElement_i(PortableServer::POA_ptr poa, bool is_pull) const
          { return new RemoteChannelElement<T>(*this, poa, is_pull); }

          /**
           * Create an transportable object for a \a protocol which contains the value of \a source.
           */
          virtual CORBA::Any* createAny( base::DataSourceBase::shared_ptr source) const
          {
              internal::DataSource<T>* d = internal::AdaptDataSource<T>()( source );
              if ( d )
                  return AnyConversion<PropertyType>::createAny( d->value());
              return 0;
          }

          /**
           * Update \a target with the contents of \a any which is an object of a \a protocol.
           */
          virtual bool updateFromAny(const CORBA::Any* any, base::DataSourceBase::shared_ptr target) const
          {
            //This line causes a compile error in DataSourceAdaptor.hpp (where the bug is)
            //Only narrow.
//             internal::AssignableDataSource<T>* ad = internal::AdaptAssignableDataSource<T>()( target );
            typename internal::AssignableDataSource<T>::shared_ptr ad = internal::AssignableDataSource<T>::narrow( target.get() );
            if ( ad ) {
                PropertyType value;
                if (AnyConversion<PropertyType>::update(*any, value ) ) {
                    ad->set( value );
                    return true;
                }
            }
            return false;
          }

          /**
           * Create a internal::DataSource which is a proxy for a remote object.
           */
          virtual base::DataSourceBase* proxy( void* data ) const
          {
            base::DataSourceBase* result = 0;
            corba::CExpression_ptr e = static_cast<corba::CExpression_ptr>(data);

            // first try as assignable DS, if not possible, try as normal DS.
            result = corba::ExpressionProxy::NarrowAssignableDataSource<PropertyType>( e );
            if (!result )
                result = corba::ExpressionProxy::NarrowDataSource<PropertyType>( e );

            return result;
          }

          virtual void* server(base::DataSourceBase::shared_ptr source, bool assignable, void* arg ) const
          {
              PortableServer::POA_ptr p = static_cast<PortableServer::POA_ptr>(arg);
              if (assignable){
                  return static_cast<CExpression_ptr>(corba::ExpressionServer::CreateAssignableExpression( source, p ));

              } else {
                  return corba::ExpressionServer::CreateExpression( source, p );
              }
          }

          virtual void* method(base::DataSourceBase::shared_ptr source, internal::MethodC* orig, void* arg) const
          {
              PortableServer::POA_ptr p = static_cast<PortableServer::POA_ptr>(arg);
              return corba::ExpressionServer::CreateMethod( source, orig, p );
          }

          virtual base::DataSourceBase* narrowDataSource(base::DataSourceBase* dsb)
          {
              // then try to see if it is a CORBA object.
              //corba::ExpressionProxyInterface* prox = dynamic_cast< corba::ExpressionProxyInterface* >(dsb);
              // Only try if the names match in the first place.
              if ( dsb->serverProtocol() == ORO_CORBA_PROTOCOL_ID && dsb->getTypeName() == internal::DataSource<T>::GetTypeName() ) {
                  Logger::log() << Logger::Debug << "Trying to narrow server "<<dsb->getType()<<" to local "<<internal::DataSource<T>::GetType() <<Logger::endl;
                  corba::CExpression_var expr = (corba::CExpression_ptr)dsb->server(ORO_CORBA_PROTOCOL_ID, 0) ;
                  assert( !CORBA::is_nil(expr) );
                  return corba::ExpressionProxy::NarrowDataSource<T>( expr.in() );
              }
              Logger::log() << Logger::Debug << "Failed to narrow server "<<dsb->getType()<<" to local "<<internal::DataSource<T>::GetType() <<Logger::endl;

              // See if the DS contains an Any. This is required for the createMethodAny variants:
              internal::DataSource<CORBA::Any_var>* aret = dynamic_cast< internal::DataSource<CORBA::Any_var>* >( dsb );
              if (aret){
                  return corba::ExpressionProxy::NarrowConstant<T>( aret->get().in() );
              }
              return 0;
          }

          virtual base::DataSourceBase* narrowAssignableDataSource(base::DataSourceBase* dsb)
          {
              // then try to see if it is a CORBA object.
              //corba::ExpressionProxyInterface* prox = dynamic_cast< corba::ExpressionProxyInterface* >(dsb);
              if ( dsb->serverProtocol() == ( ORO_CORBA_PROTOCOL_ID ) && dsb->getTypeName() == internal::DataSource<T>::GetTypeName() ) {
                  corba::CExpression_var expr = (corba::CExpression_ptr)dsb->server(ORO_CORBA_PROTOCOL_ID,0) ;
                  return corba::ExpressionProxy::NarrowAssignableDataSource<T>( expr.in() );
              }
              return 0;
          }

      };
}
}

#endif
