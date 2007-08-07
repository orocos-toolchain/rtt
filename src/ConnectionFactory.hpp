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
    class BufferConnectionInterface;
    template<class T>
    class DataConnectionInterface;
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
         * Create a new Buffered data connection.
         * 
         * @param writer A port which will write to the connection
         * @param reader A port which will read from the connection
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
        BufferConnectionInterface<T>* createBuffer(PortInterface* writer, PortInterface* reader, int size, const T& initial_value = T(), ConnectionTypes::ConnectionType type = ConnectionTypes::lockfree);

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
        BufferConnectionInterface<T>* createBuffer(int size, const T& initial_value = T(), ConnectionTypes::ConnectionType type = ConnectionTypes::lockfree);

        /** 
         * Create a new unbuffered data connection.
         * 
         * @param writer A port which will write to the connection
         * @param reader A port which will read from the connection
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
        DataConnectionInterface<T>* createDataObject(PortInterface* writer, PortInterface* reader, const T& initial_value = T(), ConnectionTypes::ConnectionType type = ConnectionTypes::lockfree);

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
        DataConnectionInterface<T>* createDataObject(const T& initial_value = T(), ConnectionTypes::ConnectionType type = ConnectionTypes::lockfree);

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
        BufferConnectionInterface<T>* ConnectionFactory<T>::createBuffer(PortInterface* writer, PortInterface* reader, int size, const T& initial_value, ConnectionTypes::ConnectionType type )
        {
            WriteBufferPort<T>* wt = dynamic_cast<WriteBufferPort<T>*>( writer );
            ReadBufferPort<T>* rt  = dynamic_cast<ReadBufferPort<T>*>( reader );

            BufferBase* conn_buffer = 0;
            int protocol = 0;
            if ( (protocol = writer->serverProtocol()) ) {
                if ( reader->serverProtocol() ) {
                    log() << "Can not connect two remote ports." <<endlog(Error);
                    log() << "One must be local and one must be remote." <<endlog(Error);
                    return 0;
                }
                conn_buffer = writer->getTypeInfo()->getProtocol( writer->serverProtocol() )->bufferProxy(writer);
            }
            else {
                if ( (protocol=reader->serverProtocol()) ) {
                    conn_buffer = reader->getTypeInfo()->getProtocol( reader->serverProtocol() )->bufferProxy(writer);
                }
            }
            // if BufferBase, we already got a remote connection.
            if (conn_buffer) {
                BufferInterface<T>* bi = dynamic_cast<BufferInterface<T>*>(conn_buffer);
                if (!bi) {
                    log(Error) << "Misconfigured protocol "<<protocol<<": could not dynamic_cast to data type."<<endlog();
                    delete conn_buffer;
                    return 0;
                }
                BufferConnectionInterface<T>* bci = new BufferConnection<T>( bi );
                bci->addReader(reader);
                bci->addWriter(writer);
                return bci;
            }

            if ( wt == 0 || rt == 0 || wt->connection() || rt->connection() ){
                Logger::log() <<Logger::Warning<< "ConnectionFactory could not create a BufferConnection between Writer:"<<writer->getName() <<" and Reader:"
                              << reader->getName() <<Logger::endl;
                std::string msg = (wt == 0 ? "Writer is not a WriteBufferPort or has wrong data type."
                                   : (rt == 0 ? "Reader is not a ReadBufferPort or has wrong data type."
                                      : ( wt->connection() ? "Writer already connected. Use connectTo()."
                                          : "Reader already connected. Use connectTo()." )));
                Logger::log() << msg  << Logger::endl;
                return 0;
            }
            
            Logger::log() << Logger::Debug<< "Creating a BufferConnection from "<<writer->getName() <<" to "
                          << reader->getName() << " with size "<<size<<Logger::endl;
            BufferConnectionInterface<T>* bci = this->createBuffer(size, initial_value, type);
            bci->addReader(reader);
            bci->addWriter(writer);
            return bci;
        }

        template<class T>
        BufferConnectionInterface<T>* ConnectionFactory<T>::createBuffer(int size, const T& initial_value, ConnectionTypes::ConnectionType type )
        {
            if (type == ConnectionTypes::lockfree)
                return new BufferConnection<T>(new BufferLockFree<T>(size, initial_value) );
            if (type == ConnectionTypes::locked)
                return new BufferConnection<T>(new BufferLocked<T>(size, initial_value) );
            return 0;
        }
            

        template<class T>
        DataConnectionInterface<T>* ConnectionFactory<T>::createDataObject(PortInterface* writer, PortInterface* reader, const T& initial_value, ConnectionTypes::ConnectionType type)
        {
            
            WriteDataPort<T>* wt = dynamic_cast<WriteDataPort<T>*>( writer );
            ReadDataPort<T>* rt  = dynamic_cast<ReadDataPort<T>*>( reader );

            DataSourceBase::shared_ptr conn_data;
            int protocol = 0;
            if ( (protocol = writer->serverProtocol()) ) {
                if ( reader->serverProtocol() ) {
                    log() << "Can not connect two remote ports." <<endlog(Error);
                    log() << "One must be local and one must be remote." <<endlog(Error);
                    return 0;
                }
                conn_data = writer->getTypeInfo()->getProtocol( writer->serverProtocol() )->dataProxy(writer);
            }
            else {
                if ( (protocol=reader->serverProtocol()) ) {
                    conn_data = reader->getTypeInfo()->getProtocol( reader->serverProtocol() )->dataProxy(writer);
                }
            }
            // if DataBase, we already got a remote connection.
            if (conn_data) {
                DataObjectInterface<T>* bi = dynamic_cast<DataObjectInterface<T>*>( conn_data.get() );
                if (!bi) {
                    log(Error) << "Misconfigured protocol "<<protocol<<": could not dynamic_cast to data type."<<endlog();
                    delete bi;
                    return 0;
                }
                DataConnectionInterface<T>* bci = new DataConnection<T>( bi );
                bci->addReader(reader);
                bci->addWriter(writer);
                return bci;
            }

            if ( wt == 0 || rt == 0 || wt->connection() || rt->connection() ) {
                Logger::log() <<Logger::Warning<< "ConnectionFactory could not create a DataConnection between Writer:"<<writer->getName() <<" and Reader:"
                              << reader->getName() << Logger::nl;
                std::string msg = (wt == 0 ? "Writer is not a WriteDataPort or has wrong data type."
                                   : (rt == 0 ? "Reader is not a ReadDataPort or has wrong data type."
                                      : ( wt->connection() ? "Writer already connected. Use connectTo()."
                                          : "Reader already connected. Use connectTo()" )));
                Logger::log() << msg  << Logger::endl;
                return 0;
            }

            Logger::log() << Logger::Debug<< "Creating a DataConnection from "<<writer->getName() <<" to "
                          << reader->getName() <<Logger::endl;
            DataConnectionInterface<T>* dci = this->createDataObject(initial_value, type);
            dci->addReader( reader );
            dci->addWriter( writer );
            return dci;
        }

        template<class T>
        DataConnectionInterface<T>* ConnectionFactory<T>::createDataObject(const T& initial_value, ConnectionTypes::ConnectionType type)
        {
            if (type == ConnectionTypes::lockfree)
                return new DataConnection<T>( new DataObjectLockFree<T>("DataObject", initial_value) );
            if (type == ConnectionTypes::locked)
                return new DataConnection<T>( new DataObjectLocked<T>("DataObject", initial_value) );
            return 0;
        }
}
#endif
