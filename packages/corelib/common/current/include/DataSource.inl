#ifndef CORELIB_DATASOURCE_INL
#define CORELIB_DATASOURCE_INL

#include "AssignCommand.hpp"
#include "DataSourceTypeInfo.hpp"
#include "DataSourceAdaptor.hpp"

#ifdef OROINT_OS_CORBA
#include "corba/ExpressionProxy.hpp"
#include "corba/ExpressionServer.hpp"
#include "corba/CorbaConversion.hpp"
#endif

namespace ORO_CoreLib
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
    void DataSource<T>::updated()
    {}

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
    Orocos::Expression_ptr DataSource<T>::server()
    {
        return ORO_Corba::ExpressionServer::CreateExpression( const_ptr(this) );
    }

    template< typename T>
    Orocos::Expression_ptr DataSource<T>::server() const
    {
        return ORO_Corba::ExpressionServer::CreateExpression( const_ptr(this) );
    }

    template< typename T>
    Orocos::Method_ptr DataSource<T>::method()
    {
        return ORO_Corba::ExpressionServer::CreateMethod( shared_ptr(this) );
    }

    
    template< typename T>
    Orocos::Expression_ptr AssignableDataSource<T>::server()
    {
        return ORO_Corba::ExpressionServer::CreateAssignableExpression( shared_ptr(this) );
    }
#endif

    template<>
    CORBA::Any* DataSource<void>::createAny() const;

    template<>
    CORBA::Any* DataSource<void>::getAny() const;

    template< typename T>
    CORBA::Any* DataSource<T>::createAny() const {
#ifdef OROINT_OS_CORBA
        return AnyConversion<typename DataSource<T>::value_t>::createAny( this->value() );
#else
        return 0;
#endif
    }

    template< typename T>
    CORBA::Any* DataSource<T>::getAny() const {
#ifdef OROINT_OS_CORBA
        return AnyConversion<typename DataSource<T>::value_t>::createAny( this->get() );
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
        //ORO_Corba::ExpressionProxyInterface* prox = dynamic_cast< ORO_Corba::ExpressionProxyInterface* >(dsb);
        if ( dsb->hasServer() ) {
            Logger::log() << Logger::Debug << "Trying to narrow server "<<dsb->getType()<<" to local "<<DataSource<T>::GetType() <<Logger::endl;
            ::Orocos::Expression_var expr = dsb->server() ;
            return ORO_Corba::ExpressionProxy::NarrowDataSource<T>( expr.in() );
        }
        // See if the DS contains an Any.
        DataSource<CORBA::Any_var>* aret = dynamic_cast< DataSource<CORBA::Any_var>* >( dsb );
        if (aret){
            return ORO_Corba::ExpressionProxy::NarrowConstant<T>( aret->get().in() );
        }
#endif
        // all failed:
        return 0;
    }

    template<class T>
    const DataSource<T>* DataSource<T>::narrow(const DataSourceBase* dsb) {
        // first try conventional C++ style cast.
        const DataSource<T>* ret = dynamic_cast< const DataSource<T>* >( dsb );
        if (ret) return ret;
#ifdef OROINT_OS_CORBA
        // then try to see if it is a CORBA object.
        //ORO_Corba::ExpressionProxyInterface* prox = dynamic_cast< ORO_Corba::ExpressionProxyInterface* >(dsb);
        if ( dsb->hasServer() ) {
            Logger::log() << Logger::Debug << "Trying to narrow server "<<dsb->getType()<<" to local "<<DataSource<T>::GetType() <<Logger::endl;
            ::Orocos::Expression_var expr = dsb->server() ;
            return ORO_Corba::ExpressionProxy::NarrowDataSource<T>( expr.in() );
        }
        const DataSource<CORBA::Any_var>* aret = dynamic_cast< const DataSource<CORBA::Any_var>* >( dsb );
        if (aret){
            typename ValueDataSource<T>::shared_ptr vds = new ValueDataSource<T>();
            if ( AnyConversion<typename DataSource<T>::value_t>::update( aret->get().in(), vds->set() ) )
                return vds.get();
        }
#endif
        // all failed:
        return 0;
    }

    // specialise the void cases...
    template<>
    DataSource<void>* DataSource<void>::narrow(DataSourceBase* dsb);

    // specialise the void cases...
    template<>
    const DataSource<void>* DataSource<void>::narrow(const DataSourceBase* dsb);

#ifdef OROINT_OS_CORBA
    // specialise the Any cases...
    template<>
    DataSource<CORBA::Any_ptr>* DataSource<CORBA::Any_ptr>::narrow(DataSourceBase* dsb);

    // specialise the Any cases...
    template<>
    const DataSource<CORBA::Any_ptr>* DataSource<CORBA::Any_ptr>::narrow(const DataSourceBase* dsb);
#endif

    template<class T>
    AssignableDataSource<T>* AssignableDataSource<T>::narrow(DataSourceBase* dsb) {
        // first try conventional C++ style cast.
        AssignableDataSource<T>* ret = dynamic_cast< AssignableDataSource<T>* >( dsb );
        if (ret) return ret;
#ifdef OROINT_OS_CORBA
        // then try to see if it is a CORBA object.
        //ORO_Corba::ExpressionProxyInterface* prox = dynamic_cast< ORO_Corba::ExpressionProxyInterface* >(dsb);
        if ( dsb->hasServer() ) {
            ::Orocos::Expression_var expr = dsb->server() ;
            return ORO_Corba::ExpressionProxy::NarrowAssignableDataSource<T>( expr.in() );
        }
#endif
        // all failed:
        return 0;
    }

    template<class T>
    const AssignableDataSource<T>* AssignableDataSource<T>::narrow(const DataSourceBase* dsb) {
        // first try conventional C++ style cast.
        const AssignableDataSource<T>* ret = dynamic_cast< const AssignableDataSource<T>* >( dsb );
        if (ret) return ret;
#ifdef OROINT_OS_CORBA
        // then try to see if it is a CORBA object.
        //ORO_Corba::ExpressionProxyInterface* prox = dynamic_cast< ORO_Corba::ExpressionProxyInterface* >(dsb);
        if ( dsb->hasServer() ) {
            ::Orocos::Expression_var expr = dsb->server() ;
            return ORO_Corba::ExpressionProxy::NarrowAssignableDataSource<T>( expr.in() );
        }
#endif
        // all failed:
        return 0;
    }

    template< typename T>
    bool AssignableDataSource<T>::update(const CORBA::Any& any) {
#ifdef OROINT_OS_CORBA
        if ( &(this->set()) == 0 )
            return false;
        return AnyConversion<typename DataSource<T>::value_t>::update( any, this->set() );
#else
        return false;
#endif
    }

    template<class T>
    bool AssignableDataSource<T>::update( DataSourceBase* other ) {
        DataSourceBase::shared_ptr r( other );
        typename DataSource<T>::shared_ptr o( AdaptDataSource<T>()(r) );
        if (o) {
            this->set( o->get() );
            return true;
        }
        return false;
    }

    template<class T>
    CommandInterface* AssignableDataSource<T>::updateCommand( DataSourceBase* other) {
        // WARNING: This does not work when T is a const reference and other is not !
        // use AdaptDataSource<T>()( r )
        DataSourceBase::shared_ptr r( other );
        typename DataSource<T>::shared_ptr t( AdaptDataSource<T>()( r ) );
        if ( t )
            return new detail::AssignCommand<T>( this, t );

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
