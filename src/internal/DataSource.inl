#ifndef CORELIB_DATASOURCE_INL
#define CORELIB_DATASOURCE_INL

#include "AssignCommand.hpp"
#include "DataSourceTypeInfo.hpp"
#include "DataSourceAdaptor.hpp"
#include "../types/TypeTransporter.hpp"

#include "../rtt-config.h"

namespace RTT
{ namespace internal {


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
      return DataSourceTypeInfo< T >::getType() + DataSourceTypeInfo< T >::getQualifier();

  }

  template< typename T>
  std::string DataSource<T>::GetTypeName()
  {
      return DataSourceTypeInfo< T >::getType();

  }

  template< typename T>
  const types::TypeInfo* DataSource<T>::getTypeInfo() const { return GetTypeInfo(); }

  template< typename T>
  const types::TypeInfo* DataSource<T>::GetTypeInfo() { return DataSourceTypeInfo<T>::getTypeInfo(); }

  template< typename T>
  bool DataSource<T>::evaluate() const
  {
      this->get();
      return true;
  }

  template<>
  RTT_API bool DataSource<bool>::evaluate() const;


    template<typename T>
    AssignableDataSource<T>::~AssignableDataSource()
    {}

    template<typename T>
    bool AssignableDataSource<T>::updateBlob(int protocol, const void* data)
    {
#ifndef ORO_EMBEDDED
        types::TypeTransporter* tt = this->getTypeInfo()->getProtocol(protocol);
        if ( tt )
            return tt->updateBlob( data, base::DataSourceBase::shared_ptr(this) );
#endif
        return false;
    }


    template<typename T>
    void* AssignableDataSource<T>::server( int protocol, void* arg )
    {
#ifndef ORO_EMBEDDED
        types::TypeTransporter* tt = this->getTypeInfo()->getProtocol(protocol);
        if ( tt )
            return tt->server( base::DataSourceBase::shared_ptr(this), true, arg );
#endif
        return 0;
    }


    template<class T>
    DataSource<T>* DataSource<T>::narrow(base::DataSourceBase* dsb) {
        // first try conventional C++ style cast.
        DataSource<T>* ret = dynamic_cast< DataSource<T>* >( dsb );
        if (ret) return ret;
#ifndef ORO_EMBEDDED
        // if narrowing failed, maybe this DS is a proxy for a remote object.
        int p_id = dsb->serverProtocol();
        if ( p_id ) {
            types::TypeTransporter* tt = DataSource<T>::GetTypeInfo()->getProtocol( p_id );
            if (tt) {
                base::DataSourceBase* ret2 = tt->narrowDataSource( dsb );
                // it may be that <T> is a 'const &' and the protocol returns a value type.
                return dynamic_cast<DataSource<T>*>(ret2);
            }
        }
#endif
        // all failed:
        return 0;
    }

    template<class T>
    AssignableDataSource<T>* AssignableDataSource<T>::narrow(base::DataSourceBase* dsb) {
        // first try conventional C++ style cast.
        AssignableDataSource<T>* ret = dynamic_cast< AssignableDataSource<T>* >( dsb );
        if (ret) return ret;
#ifndef ORO_EMBEDDED
        int p_id = dsb->serverProtocol();
        if ( p_id ) {
            types::TypeTransporter* tt = DataSource<T>::GetTypeInfo()->getProtocol( p_id );
            if (tt) {
                base::DataSourceBase* ret2 = tt->narrowAssignableDataSource( dsb );
                return dynamic_cast<AssignableDataSource<T>*>(ret2);
            }
        }
#endif
        // all failed:
        return 0;
    }

    template<class T>
    bool AssignableDataSource<T>::update( base::DataSourceBase* other ) {
        base::DataSourceBase::shared_ptr r( other );
        typename DataSource<T>::shared_ptr o = AdaptDataSource<T>()( DataSourceTypeInfo<T>::getTypeInfo()->convert(r) );
        if (o) {
            this->set( o->get() );
            return true;
        }
        return false;
    }

    template<class T>
    base::ActionInterface* AssignableDataSource<T>::updateCommand( base::DataSourceBase* other) {
        // Use the same rules of parameter passing as C++, but no const for 'int',...
        base::DataSourceBase::shared_ptr r( other );
        typename DataSource<copy_t>::shared_ptr t = AdaptDataSource<copy_t>()( DataSourceTypeInfo<T>::getTypeInfo()->convert(r) );
        if ( t )
            return new AssignCommand<T,copy_t>( this, t );

#ifndef ORO_EMBEDDED
        throw bad_assignment();
#endif

        return 0;
    }
}}


#endif
