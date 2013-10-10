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


#ifndef ORO_MQ_TEMPATE_PROTOCOL_BASE_HPP
#define ORO_MQ_TEMPATE_PROTOCOL_BASE_HPP

#include "MQLib.hpp"
#include "../../types/TypeMarshaller.hpp"
#include "MQChannelElement.hpp"

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
      class MQTemplateProtocolBase
          : public RTT::types::TypeMarshaller
      {
      public:
          /**
           * The given \a T parameter is the type for reading DataSources.
           */
          typedef T UserType;

          virtual base::ChannelElementBase::shared_ptr createStream(base::PortInterface* port, const ConnPolicy& policy, bool is_sender) const {
              try {
                  base::ChannelElementBase::shared_ptr mq = new MQChannelElement<T>(port, *this, policy, is_sender);
                  if ( !is_sender ) {
                      // the receiver needs a buffer to store his messages in.
                      base::ChannelElementBase::shared_ptr buf = detail::DataSourceTypeInfo<T>::getTypeInfo()->buildDataStorage(policy);
                      mq->setOutput(buf);
                  }
                  return mq;
              } catch(std::exception& e) {
                  log(Error) << "Failed to create MQueue Channel element: " << e.what() << endlog();
              }
              return base::ChannelElementBase::shared_ptr();
          }

      };
}
}

#endif
