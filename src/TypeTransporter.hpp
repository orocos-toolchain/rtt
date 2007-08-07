#ifndef ORO_TYPE_TRANSPORTER_HPP
#define ORO_TYPE_TRANSPORTER_HPP

#include "DataSourceBase.hpp"
#include "BufferBase.hpp"

namespace RTT
{
    class PortInterface;

    namespace detail {

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
        struct RTT_API TypeTransporter
        {
            virtual ~TypeTransporter() {}

            /**
             * Create an transportable object for a \a protocol which contains the value of \a source.
             */
            virtual void* createBlob(DataSourceBase::shared_ptr source) const = 0;

            /**
             * Update \a target with the contents of \a blob which is an object of a \a protocol.
             */
            virtual bool updateBlob(const void* blob, DataSourceBase::shared_ptr target) const = 0;

            /**
             * Create a DataSource which is a proxy for a remote server object.
             * Used to read/write remote attributes, properties and general data over a network.
             */
            virtual DataSourceBase* proxy(void* data ) const = 0;

            /**
             * Create a server for a DataSource, which can be picked up by a proxy.
             * Used to export local data to a network.
             */
            virtual void* server(DataSourceBase::shared_ptr source, bool assignable, void* arg) const = 0;

            /**
             * Create a server for a local method.
             * Used to export local methods to a network.
             */
            virtual void* method(DataSourceBase::shared_ptr source, MethodC* orig, void* arg) const = 0;

            /**
             * Returns a new DataObject<T> object mirroring a remote data object server.
             * Used to setup a Corba Data Flow.
             */
            virtual DataSourceBase* dataProxy( PortInterface* data ) const = 0;

            virtual void* dataServer( DataSourceBase::shared_ptr source, void* arg) const = 0;

            /**
             * Returns a new BufferInterface<T> object mirroring a remote buffer object server.
             * Used to setup a Corba Data Flow.
             */
            virtual BufferBase* bufferProxy( PortInterface* data ) const = 0;

            virtual void* bufferServer( BufferBase::shared_ptr source, void* arg) const = 0;

            /**
             * Narrows a remote data source object or proxy to this type.
             * Used internally to determine the type of a remote object.
             * @return 0 if \a dsb is not of this type.
             */
            virtual DataSourceBase* narrowDataSource(DataSourceBase* dsb) = 0;

            /**
             * Narrows a remote assignable data source object or proxy to this type.
             * Used internally to determine the type of a remote object.
             * @return 0 if \a dsb is not of this type.
             */
            virtual DataSourceBase* narrowAssignableDataSource(DataSourceBase* dsb) = 0;
        };

        /**
         * A class which registers TransportProtocol instances to types.
         * In order to register a transport for some types,
         * inherit from this class and add an instance to 
         * the TypeInfoRepository.
         * The TypeInfoRepository will call the registerType callback
         * for each type present or added.
         */
        struct RTT_API TransportRegistrator
        {
            virtual ~TransportRegistrator() {}

            /**
             * Add a transport for the given type to the TypeInfo
             * instance.
             * @param type_name The name of the type to transport
             * @param ti The typ to which transports may be added.
             * @return false if no transport was added, true otherwise.
             */
            virtual bool registerTransport(std::string type_name, TypeInfo* ti) = 0;

            /**
             * Returns the (protocol) name of this transport.
             */
            virtual std::string getTransportName() const = 0;
        };
    }
}

#endif
