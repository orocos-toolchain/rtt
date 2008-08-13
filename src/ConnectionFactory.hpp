/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:17 CET 2006  ConnectionFactory.hpp

                        ConnectionFactory.hpp -  description
                           -------------------
    begin                : Thu March 02 2006
    copyright            : (C) 2006 Peter Soetens
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


#ifndef ORO_EXECUTION_CONNECTION_FACTORY_HPP
#define ORO_EXECUTION_CONNECTION_FACTORY_HPP

#include "ConnectionTypes.hpp"
#include "rtt-config.h"

namespace RTT
{
    class PortInterface;
    template<class T>
    class BufferConnection;
    template<class T>
    class DataConnection;
    /**
     * Builds Connections (buffers or data objects) between
     * the ports of tasks.
     * @param T The contents type of the buffers or data objects.
     */
    template<class T>
    class ConnectionFactory
    {
    public:
        /**
         * Create an empty Buffered data connection.
         *
         * @param size The maximum capacity of the connnection's buffer
         * @param initial_value The initial value of all elements within
         * the buffer. Use this parameter if the data uses dynamic memory
         * or requires an initial state.
         * @param type The connection type: lock based or lock-free. The
         * latter is highly advised.
         *
         * @return a new connection object
         * @see BufferInterface for a look at the internal buffer structure
         * @see ReadBufferPort, WriteBufferPort, BufferPort for ports to which this
         * connection can be attached.
         */
        BufferConnection<T>* createBuffer(int size, const T& initial_value = T(), ConnectionTypes::ConnectionType type = ConnectionTypes::lockfree);

        /**
         * Create an empty unbuffered data connection.
         *
         * @param initial_value The initial value of all elements within
         * the buffer. Use this parameter if the data uses dynamic memory
         * or requires an initial state.
         * @param type The connection type: lock based or lock-free. The
         * latter is highly advised.
         *
         * @return a new connection object
         * @see DataObjectInterface for a look at the internal data sharing structure
         * @see ReadDataPort, WriteDataPort for ports to which this
         * connection can be attached.
         */
        DataConnection<T>* createDataObject(const T& initial_value = T(), ConnectionTypes::ConnectionType type = ConnectionTypes::lockfree);

    };
}
#endif

#ifndef ORO_CONNECTIONFACTORY_INLINE
#define ORO_CONNECTIONFACTORY_INLINE
#include "BufferConnection.hpp"
#include "DataConnection.hpp"
#include "BufferLocked.hpp"
#include "BufferLockFree.hpp"
#include "DataObjectInterfaces.hpp"

namespace RTT
{

        template<class T>
        BufferConnection<T>* ConnectionFactory<T>::createBuffer(int size, const T& initial_value, ConnectionTypes::ConnectionType type )
        {
            if (type == ConnectionTypes::lockfree)
                return new BufferConnection<T>( typename BufferInterface<T>::shared_ptr(new BufferLockFree<T>(size, initial_value)) );
            if (type == ConnectionTypes::locked)
                return new BufferConnection<T>( typename BufferInterface<T>::shared_ptr(new BufferLocked<T>(size, initial_value)) );
            return 0;
        }

        template<class T>
        DataConnection<T>* ConnectionFactory<T>::createDataObject(const T& initial_value, ConnectionTypes::ConnectionType type)
        {
            if (type == ConnectionTypes::lockfree)
                return new DataConnection<T>( new DataObjectLockFree<T>("DataObject", initial_value) );
            if (type == ConnectionTypes::locked)
                return new DataConnection<T>( new DataObjectLocked<T>("DataObject", initial_value) );
            return 0;
        }
}
#endif
