#ifndef ORO_MQ_TEMPATE_PROTOCOL_HPP
#define ORO_MQ_TEMPATE_PROTOCOL_HPP

#include "../../types/TypeMarshaller.hpp"
#include "MQChannelElement.hpp"
#include "../../types/Types.hpp"
#include "../../InputPort.hpp"
#include "../../OutputPort.hpp"

#include <boost/type_traits/has_virtual_destructor.hpp>
#include <boost/static_assert.hpp>

namespace RTT
{ namespace mqueue
  {
      /**
       * For each transportable type T, specify the conversion functions.
       * @warning This can only be used if T is a trivial type without
       * meaningful (copy) constructor. For all other cases, or in doubt,
       * use the MQSerializationProtocol class.
       *
       */
      template<class T>
      class MQTemplateProtocol
          : public RTT::types::TypeMarshaller<T>
      {
      public:
          /**
           * We don't support types with virtual functions !
           * TODO: use this type trait to make the necessary adjustments
           * in the memcopy (adding the vptr table offset).
           */
          BOOST_STATIC_ASSERT( !boost::has_virtual_destructor<T>::value );
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
           * This is a real-time function which does not allocate memory and which requires source
           * to be an AssignableDataSource.
           */
          virtual std::pair<void*,int> createBlob( base::DataSourceBase::shared_ptr source) const
          {
              internal::AssignableDataSource<T>* d = internal::AdaptAssignableDataSource<T>()( source );
              if ( d )
                  return std::make_pair((void*) &(d->set()), int(sizeof(T)));
              return std::make_pair((void*)0,int(0));
          }

          virtual std::pair<void*,int> fillBlob( base::DataSourceBase::shared_ptr source, void* blob, int size) const
          {
              internal::AssignableDataSource<T>* d = internal::AdaptAssignableDataSource<T>()( source );
              if ( d ) {
                  memcpy(blob, (void*) &(d->set()),  int(sizeof(T)) );
                  return std::make_pair( blob, int(sizeof(T)) );
              }
              return std::make_pair((void*)0,int(0));
          }

          /**
           * Update \a target with the contents of \a blob which is an object of a \a protocol.
           */
          virtual bool updateBlob(const void* blob, base::DataSourceBase::shared_ptr target) const
          {
            typename internal::AssignableDataSource<T>::shared_ptr ad = internal::AssignableDataSource<T>::narrow( target.get() );
            if ( ad ) {
                ad->set( *(T*)(blob) );
                return true;
            }
            return false;
          }

          virtual unsigned int getSampleSize(const T& ignored) const
          {
              // re-implement this in case of complex types, like std::vector<T>.
              return sizeof(T);
          }

          virtual base::ChannelElementBase* createChannel(base::PortInterface* port, std::string& name_id, int size_hint, bool is_sender) const {
              try {
                  return new MQChannelElement<T>(port, *this, name_id, size_hint, is_sender);
              } catch(std::exception& e) {
                  log(Error) << "Failed to create MQueue Channel element: " << e.what() << endlog();
              }
              return 0;
          }

          /**
           * Create a internal::DataSource which is a proxy for a remote object.
           */
          virtual base::DataSourceBase* proxy( void* data ) const
          {
            base::DataSourceBase* result = 0;
            return result;
          }

          virtual void* server(base::DataSourceBase::shared_ptr source, bool assignable, void* arg ) const
          {
              return 0;
          }

          virtual void* method(base::DataSourceBase::shared_ptr source, internal::MethodC* orig, void* arg) const
          {
              return 0;
          }

          virtual base::DataSourceBase* narrowDataSource(base::DataSourceBase* dsb)
          {
              return 0;
          }

          virtual base::DataSourceBase* narrowAssignableDataSource(base::DataSourceBase* dsb)
          {
              return 0;
          }

      };
}
}

#endif
