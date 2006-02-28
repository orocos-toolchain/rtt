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
     */
    class ConnectionFactory
    {
    public:
        template<class T>
        BufferConnectionInterface<T>* createBuffer(PortInterface* writer, PortInterface* reader, int size, ConnectionTypes::ConnectionType type = ConnectionTypes::lockfree);

        template<class T>
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
        BufferConnectionInterface<T>* ConnectionFactory::createBuffer(PortInterface* writer, PortInterface* reader, int size, ConnectionTypes::ConnectionType type )
        {
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
                return new BufferConnection<T,BufferLockFree<T> >(wt, rt, size);
            if (type == ConnectionTypes::locked)
                return new BufferConnection<T,BufferLocked<T> >(wt, rt, size);
                
            return 0;
        }

        template<class T>
        DataConnectionInterface<T>* ConnectionFactory::createDataObject(PortInterface* writer, PortInterface* reader, ConnectionTypes::ConnectionType type)
        {
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
                return new DataConnection<T,DataObjectLockFree<T> >(wt, rt);
            if (type == ConnectionTypes::locked)
                return new DataConnection<T,DataObjectLocked<T> >(wt, rt);
            return 0;
        }
}
#endif
