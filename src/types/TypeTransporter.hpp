/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:25 CET 2008  TypeTransporter.hpp

                        TypeTransporter.hpp -  description
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


#ifndef ORO_TYPE_TRANSPORTER_HPP
#define ORO_TYPE_TRANSPORTER_HPP

#include "../base/DataSourceBase.hpp"
#include "../base/BufferBase.hpp"
#include "../base/ChannelElementBase.hpp"

namespace RTT
{

    namespace types {

        /**
         * This interface defines the function a transport protocol
         * must support in order to allow Orocos components to
         * remotely communicate data.
         *
         * @warning This class requires that you cast objects to (void*).
         * If your objects use virtual inheritance, you \b must cast to and
         * from the same type and not to a sub- or super-class. If you fail
         * to do so, you'll get immediate hard to debug crashes, as objects
         * appear to be corrupted.
         */
        class RTT_API TypeTransporter
        {
		public:
            virtual ~TypeTransporter() {}

            /**
             * Create an transportable object for a \a protocol which contains the value of \a source.
             */
            virtual void* createBlob(base::DataSourceBase::shared_ptr source) const = 0;

            /**
             * Update \a target with the contents of \a blob which is an object of a \a protocol.
             */
            virtual bool updateBlob(const void* blob, base::DataSourceBase::shared_ptr target) const = 0;

            /**
             * Create a internal::DataSource which is a proxy for a remote server object.
             * Used to read/write remote attributes, properties and general data over a network.
             */
            virtual base::DataSourceBase* proxy(void* data ) const = 0;

            /**
             * Create a server for a internal::DataSource, which can be picked up by a proxy.
             * Used to export local data to a network.
             */
            virtual void* server(base::DataSourceBase::shared_ptr source, bool assignable, void* arg) const = 0;

            /**
             * Create a server for a local method.
             * Used to export local methods to a network.
             */
            virtual void* method(base::DataSourceBase::shared_ptr source, internal::MethodC* orig, void* arg) const = 0;

            /**
             * Creates a channel element for reading or writing over this transport.
             * The transport receives a channel id which consists of 'component_name.port_name' (or a user supplied id)
             * and a transport specific argument.
             */
            virtual base::ChannelElementBase* createRemoteChannel(std::string channel_id, void* arg, bool is_sender) const = 0;

            /**
             * Narrows a remote data source object or proxy to this type.
             * Used internally to determine the type of a remote object.
             * @return 0 if \a dsb is not of this type.
             */
            virtual base::DataSourceBase* narrowDataSource(base::DataSourceBase* dsb) = 0;

            /**
             * Narrows a remote assignable data source object or proxy to this type.
             * Used internally to determine the type of a remote object.
             * @return 0 if \a dsb is not of this type.
             */
            virtual base::DataSourceBase* narrowAssignableDataSource(base::DataSourceBase* dsb) = 0;
        };
    }
}

#endif
