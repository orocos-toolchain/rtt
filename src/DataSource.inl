#ifndef CORELIB_DATASOURCE_INL
#define CORELIB_DATASOURCE_INL

#include "AssignCommand.hpp"
#include "DataSourceTypeInfo.hpp"
#include "DataSourceAdaptor.hpp"

#include <pkgconf/os.h>
#ifdef OROINT_OS_CORBA
#include "corba/ExpressionProxy.hpp"
#include "corba/ExpressionServer.hpp"
#include "corba/CorbaConversion.hpp"
#endif

namespace RTT
{

  template<typename T>
  DataSource<T>::~DataSource()
  {
  }

  template< typename T>
  std::string DataSource<T>::getType() const
  {
      return DataSource<T>::GetType();
  }

  template< typename T>
  std::string DataSource<T>::getTypeName() const
  {
      return DataSource<T>::GetTypeName();
  }

  template< typename T>
  std::string DataSource<T>::GetType()
  {
      return detail::DataSourceTypeInfo< T >::getType() + detail::DataSourceTypeInfo< T >::getQualifier();
          
  }

  template< typename T>
  std::string DataSource<T>::GetTypeName()
  {
      return detail::DataSourceTypeInfo< T >::getType();
          
  }

  template< typename T>
  const TypeInfo* DataSource<T>::getTypeInfo() const { return GetTypeInfo(); }

  template< typename T>
  const TypeInfo* DataSource<T>::GetTypeInfo() { return detail::DataSourceTypeInfo<T>::getTypeInfo(); }

  template< typename T>
  bool DataSource<T>::evaluate() const
  {
      this->get();
      return true;
  }

  template<>
  bool DataSource<bool>::evaluate() const;


  template<typename T>
  AssignableDataSource<T>::~AssignableDataSource()
  {}

#ifdef OROINT_OS_CORBA

    template< typename T>
    Corba::Expression_ptr DataSource<T>::server( PortableServer::POA_ptr p)
    {
        return Corba::ExpressionServer::CreateExpression( shared_ptr(this), p );
    }

    template< typename T>
    Corba::Method_ptr DataSource<T>::method( MethodC* orig, PortableServer::POA_ptr p )
    {
        return Corba::ExpressionServer::CreateMethod( shared_ptr(this), orig, p );
    }

    
    template< typename T>
    Corba::Expression_ptr AssignableDataSource<T>::server( PortableServer::POA_ptr p)
    {
        return Corba::ExpressionServer::CreateAssignableExpression( shared_ptr(this), p );
    }
#endif

    template<>
    CORBA::Any* DataSource<void>::createAny();

    template<>
    CORBA::Any* DataSource<void>::getAny();

    template< typename T>
    CORBA::Any* DataSource<T>::createAny() {
#ifdef OROINT_OS_CORBA
        return DataSource<T>::GetTypeInfo()->createAny( this );
#else
        return 0;
#endif
    }

    template< typename T>
    CORBA::Any* DataSource<T>::getAny() {
#ifdef OROINT_OS_CORBA
	this->get();
        return DataSource<T>::GetTypeInfo()->createAny( this );
#else
        return 0;
#endif
    }

    template<class T>
    DataSource<T>* DataSource<T>::narrow(DataSourceBase* dsb) {
        // first try conventional C++ style cast.
        DataSource<T>* ret = dynamic_cast< DataSource<T>* >( dsb );
        if (ret) return ret;
#ifdef OROINT_OS_CORBA
        // then try to see if it is a CORBA object.
        //Corba::ExpressionProxyInterface* prox = dynamic_cast< Corba::ExpressionProxyInterface* >(dsb);
        if ( dsb->hasServer() ) {
            Logger::log() << Logger::Debug << "Trying to narrow server "<<dsb->getType()<<" to local "<<DataSource<T>::GetType() <<Logger::endl;
            Corba::Expression_var expr = dsb->server(0) ;
            return Corba::ExpressionProxy::NarrowDataSource<T>( expr.in() );
        }
        // See if the DS contains an Any.
        DataSource<CORBA::Any_var>* aret = dynamic_cast< DataSource<CORBA::Any_var>* >( dsb );
        if (aret){
            return Corba::ExpressionProxy::NarrowConstant<T>( aret->get().in() );
        }
#endif
        // all failed:
        return 0;
    }

    // specialise the void cases...
    template<>
    DataSource<void>* DataSource<void>::narrow(DataSourceBase* dsb);

#ifdef OROINT_OS_CORBA
    // specialise the Any cases...
    template<>
    DataSource<CORBA::Any_ptr>* DataSource<CORBA::Any_ptr>::narrow(DataSourceBase* dsb);

#endif

    template<class T>
    AssignableDataSource<T>* AssignableDataSource<T>::narrow(DataSourceBase* dsb) {
        // first try conventional C++ style cast.
        AssignableDataSource<T>* ret = dynamic_cast< AssignableDataSource<T>* >( dsb );
        if (ret) return ret;
#ifdef OROINT_OS_CORBA
        // then try to see if it is a CORBA object.
        //Corba::ExpressionProxyInterface* prox = dynamic_cast< Corba::ExpressionProxyInterface* >(dsb);
        if ( dsb->hasServer() ) {
            Corba::Expression_var expr = dsb->server(0) ;
            return Corba::ExpressionProxy::NarrowAssignableDataSource<T>( expr.in() );
        }
#endif
        // all failed:
        return 0;
    }

    template< typename T>
    bool AssignableDataSource<T>::update(const CORBA::Any& any) {
#ifdef OROINT_OS_CORBA
        return DataSource<T>::GetTypeInfo()->update( any, this );
#else
        return false;
#endif
    }

    template<class T>
    bool AssignableDataSource<T>::update( DataSourceBase* other ) {
        DataSourceBase::shared_ptr r( other );
        typename DataSource<T>::shared_ptr o = AdaptDataSource<T>()(r);
        if (o) {
            this->set( o->get() );
            return true;
        }
        return false;
    }

    template<class T>
    CommandInterface* AssignableDataSource<T>::updateCommand( DataSourceBase* other) {
        // Use the same rules of parameter passing as C++, but no const for 'int',...
        DataSourceBase::shared_ptr r( other );
        typename DataSource<copy_t>::shared_ptr t = AdaptDataSource<copy_t>()( r );
        if ( t )
            return new detail::AssignCommand<T,copy_t>( this, t );

#ifndef ORO_EMBEDDED
        throw bad_assignment();
#endif

        return 0;
    }

#ifdef OROINT_OS_CORBA
    /**
     * Specialisation in case of Any.
     */
    template<>
    CommandInterface* AssignableDataSource<CORBA::Any_ptr>::updateCommand( DataSourceBase* other);
#endif
}


#endif
