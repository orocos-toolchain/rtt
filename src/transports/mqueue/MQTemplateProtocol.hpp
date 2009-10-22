/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  MQTemplateProtocol.hpp

                        MQTemplateProtocol.hpp -  description
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

          virtual std::pair<void*,int> fillBlob( base::DataSourceBase::shared_ptr source, void* blob, int size) const
          {
              internal::AssignableDataSource<T>* d = internal::AdaptAssignableDataSource<T>()( source );
              if ( d )
                  return std::make_pair((void*) &(d->set()), int(sizeof(T)));
              return std::make_pair((void*)0,int(0));
          }

          virtual bool updateFromBlob(const void* blob, int size, base::DataSourceBase::shared_ptr target) const
          {
            typename internal::AssignableDataSource<T>::shared_ptr ad = internal::AssignableDataSource<T>::narrow( target.get() );
            assert( size == sizeof(T) );
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

          virtual base::ChannelElementBase* createStream(base::PortInterface* port, const ConnPolicy& policy, bool is_sender) const {
              try {
                  base::ChannelElementBase* mq = new MQChannelElement<T>(port, *this, policy, is_sender);
                  if ( !is_sender ) {
                      // the receiver needs a buffer to store his messages in.
                      base::ChannelElementBase* buf = detail::DataSourceTypeInfo<T>::getTypeInfo()->buildDataStorage(policy);
                      mq->setOutput(buf);
                  }
                  return mq;
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
