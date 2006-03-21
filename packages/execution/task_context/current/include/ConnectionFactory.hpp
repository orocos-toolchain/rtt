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

namespace ORO_Execution
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
        BufferConnectionInterface<T>* createBuffer(PortInterface* writer, PortInterface* reader, int size, ConnectionTypes::ConnectionType type = ConnectionTypes::lockfree);

        DataConnectionInterface<T>* createDataObject(PortInterface* writer, PortInterface* reader, ConnectionTypes::ConnectionType type = ConnectionTypes::lockfree);

    };
}

#include "BufferConnection.hpp"
#include "DataConnection.hpp"
#include <corelib/BufferLocked.hpp>
#include <corelib/BufferLockFree.hpp>
#include <corelib/DataObjectInterfaces.hpp>

namespace ORO_Execution
{

        template<class T>
        BufferConnectionInterface<T>* ConnectionFactory<T>::createBuffer(PortInterface* writer, PortInterface* reader, int size, ConnectionTypes::ConnectionType type )
        {
            using ORO_CoreLib::Logger;
            WriteBufferPort<T>* wt = dynamic_cast<WriteBufferPort<T>*>( writer );
            ReadBufferPort<T>* rt  = dynamic_cast<ReadBufferPort<T>*>( reader );
            
            if ( wt == 0 || rt == 0 || wt->connection() || rt->connection() ){
                Logger::log() <<Logger::Warning<< "ConnectionFactory could not create a BufferConnection between "<<writer->getName() <<" and "
                              << reader->getName() <<Logger::endl;
                std::string msg = (wt == 0 ? "Writer has wrong type." : (rt == 0 ? "Reader has wrong type." : ( wt->connection() ? "Writer already connected." : "Reader already connected." )));
                Logger::log() << msg  << Logger::endl;
                return 0;
            }
            
            if (type == ConnectionTypes::lockfree)
                return new BufferConnection<T,ORO_CoreLib::BufferLockFree<T> >(wt, rt, size);
            if (type == ConnectionTypes::locked)
                return new BufferConnection<T,ORO_CoreLib::BufferLocked<T> >(wt, rt, size);
                
            return 0;
        }

        template<class T>
        DataConnectionInterface<T>* ConnectionFactory<T>::createDataObject(PortInterface* writer, PortInterface* reader, ConnectionTypes::ConnectionType type)
        {
            using ORO_CoreLib::Logger;
            WriteDataPort<T>* wt = dynamic_cast<WriteDataPort<T>*>( writer );
            ReadDataPort<T>* rt  = dynamic_cast<ReadDataPort<T>*>( reader );
            
            if ( wt == 0 || rt == 0 || wt->connection() || rt->connection() ) {
                Logger::log() <<Logger::Warning<< "ConnectionFactory could not create a DataConnection between "<<writer->getName() <<" and "
                              << reader->getName() << Logger::nl;
                std::string msg = (wt == 0 ? "Writer has wrong type." : (rt == 0 ? "Reader has wrong type." : ( wt->connection() ? "Writer already connected." : "Reader already connected." )));
                Logger::log() << msg  << Logger::endl;
                return 0;
            }

            if (type == ConnectionTypes::lockfree)
                return new DataConnection<T,ORO_CoreLib::DataObjectLockFree<T> >(wt, rt);
            if (type == ConnectionTypes::locked)
                return new DataConnection<T,ORO_CoreLib::DataObjectLocked<T> >(wt, rt);
            return 0;
        }
}
#endif
