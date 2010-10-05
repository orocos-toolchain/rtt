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

#include "../Types.hpp"
#include "ExpressionProxy.hpp"

#include "CorbaPort.hpp"
#include "CorbaDataObjectProxy.hpp"
#include "CorbaBufferProxy.hpp"
#include "ExpressionServer.hpp"
#include "DataFlowI.h"

namespace RTT
{ namespace detail
  {
      using namespace RTT::Corba;

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
       * Hence, this class uses always the same base class (Expression_ptr) to
       * communicate with the TypeTransport interface. Such that we know that when
       * we receive a (void*) it came from an (Expression_ptr) and vice versa.
       * @warning
       * Don't obey this and you'll get immediate hard to dissect crashes !
       * * * * B I G  N O T E * * * *
       */
      template<class T>
      class CorbaTemplateProtocol
          : public TypeTransporter
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

          /**
           * Create an transportable object for a \a protocol which contains the value of \a source.
           */
          virtual void* createBlob( DataSourceBase::shared_ptr source) const
          {
			  typename DataSource<T>::shared_ptr d = AdaptDataSource<T>()( source );
              if ( d )
                  return AnyConversion<PropertyType>::createAny( d->value() );
              return 0;
          }

          /**
           * Update \a target with the contents of \a blob which is an object of a \a protocol.
           */
          virtual bool updateBlob(const void* blob, DataSourceBase::shared_ptr target) const
          {
            //This line causes a compile error in DataSourceAdaptor.hpp (where the bug is)
            //Only narrow.
//             AssignableDataSource<T>* ad = AdaptAssignableDataSource<T>()( target );
            typename AssignableDataSource<T>::shared_ptr ad = AssignableDataSource<T>::narrow( target.get() );
            const CORBA::Any* any = static_cast<const CORBA::Any*>(blob);
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
           * Create a DataSource which is a proxy for a remote object.
           */
          virtual DataSourceBase* proxy( void* data ) const
          {
            DataSourceBase* result = 0;
            Corba::Expression_ptr e = static_cast<Corba::Expression_ptr>(data);

            // first try as assignable DS, if not possible, try as normal DS.
            result = Corba::ExpressionProxy::NarrowAssignableDataSource<PropertyType>( e );
            if (!result )
                result = Corba::ExpressionProxy::NarrowDataSource<PropertyType>( e );

            return result;
          }

          virtual void* server(DataSourceBase::shared_ptr source, bool assignable, void* arg ) const
          {
              PortableServer::POA_ptr p = static_cast<PortableServer::POA_ptr>(arg);
              if (assignable){
                  return static_cast<Expression_ptr>(Corba::ExpressionServer::CreateAssignableExpression( source, p ));

              } else {
                  return Corba::ExpressionServer::CreateExpression( source, p );
              }
          }

          virtual void* method(DataSourceBase::shared_ptr source, MethodC* orig, void* arg) const
          {
              PortableServer::POA_ptr p = static_cast<PortableServer::POA_ptr>(arg);
              return Corba::ExpressionServer::CreateMethod( source, orig, p );
          }

          virtual DataSourceBase* narrowDataSource(DataSourceBase* dsb)
          {
              // then try to see if it is a CORBA object.
              //Corba::ExpressionProxyInterface* prox = dynamic_cast< Corba::ExpressionProxyInterface* >(dsb);
              // Only try if the names match in the first place.
              if ( dsb->serverProtocol() == ORO_CORBA_PROTOCOL_ID && dsb->getTypeName() == DataSource<T>::GetTypeName() ) {
                  Logger::log() << Logger::Debug << "Trying to narrow server "<<dsb->getType()<<" to local "<<DataSource<T>::GetType() <<Logger::endl;
                  Corba::Expression_var expr = (Corba::Expression_ptr)dsb->server(ORO_CORBA_PROTOCOL_ID, 0) ;
                  assert( !CORBA::is_nil(expr) );
                  return Corba::ExpressionProxy::NarrowDataSource<T>( expr.in() );
              }
              Logger::log() << Logger::Debug << "Failed to narrow server "<<dsb->getType()<<" to local "<<DataSource<T>::GetType() <<Logger::endl;

              // See if the DS contains an Any. This is required for the createMethodAny variants:
              DataSource<CORBA::Any_var>* aret = dynamic_cast< DataSource<CORBA::Any_var>* >( dsb );
              if (aret){
                  return Corba::ExpressionProxy::NarrowConstant<T>( aret->get().in() );
              }
              return 0;
          }

          virtual DataSourceBase* narrowAssignableDataSource(DataSourceBase* dsb)
          {
              // then try to see if it is a CORBA object.
              //Corba::ExpressionProxyInterface* prox = dynamic_cast< Corba::ExpressionProxyInterface* >(dsb);
              if ( dsb->serverProtocol() == ( ORO_CORBA_PROTOCOL_ID ) && dsb->getTypeName() == DataSource<T>::GetTypeName() ) {
                  Corba::Expression_var expr = (Corba::Expression_ptr)dsb->server(ORO_CORBA_PROTOCOL_ID,0) ;
                  return Corba::ExpressionProxy::NarrowAssignableDataSource<T>( expr.in() );
              }
              return 0;
          }

          virtual DataSourceBase* dataProxy( PortInterface* data ) const
          {
              // Detect corba connection
              Corba::CorbaPort* cp = dynamic_cast<Corba::CorbaPort*>( data );

              assert(cp);

              return new Corba::CorbaDataObjectProxy<T>("CorbaProxy", cp->getDataChannel());
          }

          virtual DataSourceBase* dataProxy( void* data ) const
          {
              Corba::AssignableExpression_ptr ae = static_cast<Corba::AssignableExpression_ptr>(data);
              log(Debug) << "Creating Corba DataSource proxy." << endlog();
              return new Corba::CorbaDataObjectProxy<T>("CorbaProxy", ae );
          }

          virtual void* dataServer( DataSourceBase::shared_ptr source, void* arg) const
          {
              // create a default channel.
              log(Debug) << "Returning Corba Data Object." << endlog();
              return this->server(source, true, arg );
          }

          /**
           * Returns a new BufferInterface<T> object mirroring a remote buffer object server.
           * Used to setup a Corba Data Flow.
           */
          virtual BufferBase* bufferProxy( PortInterface* data ) const
          {
              // Detect corba connection
              Corba::CorbaPort* cp = dynamic_cast<Corba::CorbaPort*>( data );

              assert( cp );

              return new Corba::CorbaBufferProxy<T>( cp->getBufferChannel() );
          }

          virtual BufferBase* bufferProxy( void* data ) const
          {
              Corba::BufferChannel_ptr buf = static_cast<Corba::BufferChannel_ptr>(data);
              log(Debug) << "Creating Corba BufferChannel proxy." << endlog();
              return new Corba::CorbaBufferProxy<T>( buf );
          }

            virtual void* bufferServer( BufferBase::shared_ptr source, void* arg) const
          {
              // arg is POA !
              typename RTT::BufferInterface<T>::shared_ptr bi = boost::dynamic_pointer_cast< RTT::BufferInterface<T> >( source );
              log(Debug) << "Creating Corba BufferChannel." << endlog();
              RTT_Corba_BufferChannel_i<T>* cbuf = new RTT_Corba_BufferChannel_i<T>( bi );
              return cbuf->_this();
          }
      };
}
}

#endif
