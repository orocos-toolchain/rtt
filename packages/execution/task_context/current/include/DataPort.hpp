#ifndef ORO_EXECUTION_DATA_PORT_INTERFACE_HPP
#define ORO_EXECUTION_DATA_PORT_INTERFACE_HPP

#include <string>
#include "PortInterface.hpp"
#include "DataConnectionInterface.hpp"
#include "ConnectionTypes.hpp"

namespace ORO_Execution
{
    /**
     * A Port to a readable DataObject.
     * @param T The type of the data of the buffer.
     */
    template<class T>
    class ReadDataPort
        : public PortInterface
    {
        typedef T DataType;
    protected:
        /**
         * The connection to read from.
         */
        typename DataConnectionInterface<T>::shared_ptr mconn;

    public:
        /**
         * Construct an unconnected Port to a readable buffer.
         * @param name The name of this port.
         */
        ReadDataPort(const std::string& name) : PortInterface(name), mconn() {}

        /**
         * Get the buffer to read from. The Task may use this to read from a
         * Buffer connected to this port.
         * @return 0 if !connected(), the buffer otherwise.
         */
        const DataObjectInterface<T>* data() const { return mconn ? mconn->data() : 0; }

        /**
         * Connect a readable buffer to this Port.
         */
        bool connect(typename DataConnectionInterface<T>::shared_ptr conn) { 
            if ( mconn || !conn )
                return false;
            mconn = conn;
            return true;
        }

        virtual ConnectionInterface::shared_ptr connection() const { return mconn; }

        bool connected() const { return mconn; };

        bool connectTo( ConnectionInterface::shared_ptr other) {
            return other->addReader( this );
        }

        void disconnect() {
            mconn = 0;
        }

        virtual DataSourceFactoryInterface* createDataSources() {
            if ( !mconn )
                return 0;
            TemplateDataSourceFactory<DataObjectInterface<T> >* datas = newDataSourceFactory( mconn->data() );
            datas->add("Get", ORO_Execution::data( &DataObjectInterface<T>::Get, "Get the current value of this Data Object"));
            return datas;
        }

    };

    /**
     * A Port to a readable Buffer.
     * @param T The type of the data of the buffer.
     */
    template<class T>
    class WriteDataPort
        : public PortInterface
    {
        typedef T DataType;
    protected:
        /**
         * The connection to read from.
         */
        typename DataConnectionInterface<T>::shared_ptr mconn;

    public:
        ConnectionInterface::shared_ptr createConnection(PortInterface* other, ConnectionTypes::ConnectionType con_type = ConnectionTypes::lockfree);

        /**
         * Get the buffer to read from. The Task may use this to read from a
         * Buffer connected to this port.
         * @return 0 if !connected(), the buffer otherwise.
         */
        DataObjectInterface<T>* data() { return mconn ? mconn->data() : 0; }

        /**
         * Construct an unconnected Port to a readable buffer.
         * @param name The name of this port.
         */
        WriteDataPort(const std::string& name) : PortInterface(name), mconn() {}

        /**
         * Connect a readable buffer to this Port.
         */
        bool connect(typename DataConnectionInterface<T>::shared_ptr conn) { 
            if ( mconn || !conn )
                return false;
            mconn = conn;
            return true;
        }

        virtual ConnectionInterface::shared_ptr connection() const { return mconn; }

        bool connected() const { return mconn; };

        bool connectTo( ConnectionInterface::shared_ptr other) {
            return other->addWriter( this );
        }

        void disconnect() {
            mconn = 0;
        }

        virtual DataSourceFactoryInterface* createDataSources() {
            if ( !mconn )
                return 0;
            TemplateDataSourceFactory<DataObjectInterface<T> >* datas = newDataSourceFactory( mconn->data() );
            datas->add("Get", ORO_Execution::data( &DataObjectInterface<T>::Get, "Get the current value of this Data Object"));
            datas->add("Set", ORO_Execution::data( &DataObjectInterface<T>::Set, "Set the current value of this Data Object",
                                    "Value", "The new value.") );
            return datas;
        }
    };


}

#include "ConnectionFactory.hpp"

namespace ORO_Execution
{
    template<class T>
    ConnectionInterface::shared_ptr WriteDataPort<T>::createConnection(PortInterface* other, ConnectionTypes::ConnectionType con_type)
    {
        ConnectionFactory cf;
        return ConnectionInterface::shared_ptr ( cf.createDataObject<T>(this, other, con_type) );
    }
}

#endif
