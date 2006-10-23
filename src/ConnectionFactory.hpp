/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:17 CET 2006  ConnectionFactory.hpp 

                        ConnectionFactory.hpp -  description
                           -------------------
    begin                : Thu March 02 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef ORO_EXECUTION_CONNECTION_FACTORY_HPP
#define ORO_EXECUTION_CONNECTION_FACTORY_HPP

#include "ConnectionTypes.hpp"
#include <pkgconf/rtt-config.h>

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
#ifdef OROPKG_CORBA
#include "corba/CorbaPort.hpp"
#include "corba/CorbaDataObjectProxy.hpp"
#include "corba/CorbaBufferProxy.hpp"
#include "corba/DataFlowI.h"
#endif

namespace RTT
{

        template<class T>
        BufferConnectionInterface<T>* ConnectionFactory<T>::createBuffer(PortInterface* writer, PortInterface* reader, int size, const T& initial_value, ConnectionTypes::ConnectionType type )
        {
            WriteBufferPort<T>* wt = dynamic_cast<WriteBufferPort<T>*>( writer );
            ReadBufferPort<T>* rt  = dynamic_cast<ReadBufferPort<T>*>( reader );
            
#ifdef OROPKG_CORBA
            using namespace Corba;
            // Detect corba connection
            CorbaPort* wcp = dynamic_cast<CorbaPort*>( writer );
            CorbaPort* rcp = dynamic_cast<CorbaPort*>( reader );

            if ( wcp || rcp ) {
                log(Info) << "Connecting ports using 'corba' connection type." <<endlog();
                if ( (wcp && rcp) ) {
                    log() << "Can not connect two remote ports." <<endlog(Error);
                    log() << "One must be local and one must be remote." <<endlog(Error);
                    return 0;
                }
                if (wcp && rt) {
                    // remote writer, local reader connection
                    // The writer is requested to open an event channel (getEventChannel), our
                    // local connection object reads from that channel through the CorbaDataObjectProxy.
                    // Only the local reader connected. If a writer is connected lateron, it will write
                    // into the Event Channel as well.
                    BufferConnection<T>* dc = new BufferConnection<T>(new CorbaBufferProxy<T>( wcp->getBufferChannel() ));
                    dc->addReader(rt);
                    return dc;
                }

                if ( rcp && wt ) {
                    // remote reader, local writer connection
                    // quite the same as above.
                    BufferConnection<T>* dc = new BufferConnection<T>(new CorbaBufferProxy<T>( rcp->getBufferChannel() ));
                    dc->addWriter(wt);
                    return dc;
                }
                log(Error) << "Failed to setup Corba connection: local reader or writer was null." <<endlog();
                return 0;
            }
#endif

            if ( wt == 0 || rt == 0 || wt->connection() || rt->connection() ){
                Logger::log() <<Logger::Warning<< "ConnectionFactory could not create a BufferConnection between "<<writer->getName() <<" and "
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

#ifdef OROPKG_CORBA
            using namespace Corba;
            // Detect corba connection
            CorbaPort* wcp = dynamic_cast<CorbaPort*>( writer );
            CorbaPort* rcp = dynamic_cast<CorbaPort*>( reader );

            if ( wcp || rcp ) {
                log(Info) << "Connecting ports using 'corba' connection type." <<endlog();
                if ( (wcp && rcp) ) {
                    log() << "Can not connect two remote ports." <<endlog(Error);
                    log() << "One must be local and one must be remote." <<endlog(Error);
                    return 0;
                }
                if (wcp && rt) {
                    // remote writer, local reader connection
                    // The writer is requested to open an event channel (getEventChannel), our
                    // local connection object reads from that channel through the CorbaDataObjectProxy.
                    // Only the local reader connected. If a writer is connected lateron, it will write
                    // into the Event Channel as well.
                    DataConnection<T>* dc = new DataConnection<T>(new CorbaDataObjectProxy<T>("CorbaProxy", wcp->getDataChannel() ));
                    dc->addReader(rt);
                    return dc;
                }

                if ( rcp && wt ) {
                    // remote reader, local writer connection
                    // quite the same as above.
                    DataConnection<T>* dc = new DataConnection<T>(new CorbaDataObjectProxy<T>("CorbaProxy", rcp->getDataChannel() ));
                    dc->addWriter(wt);
                    return dc;
                }
                log(Error) << "Failed to setup Corba connection: local reader or writer was null." <<endlog();
                return 0;
            }
#endif
            
            if ( wt == 0 || rt == 0 || wt->connection() || rt->connection() ) {
                Logger::log() <<Logger::Warning<< "ConnectionFactory could not create a DataConnection between "<<writer->getName() <<" and "
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
