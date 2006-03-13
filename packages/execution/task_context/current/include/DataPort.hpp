/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:18 CET 2006  DataPort.hpp 

                        DataPort.hpp -  description
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
 
 
#ifndef ORO_EXECUTION_DATA_PORT_INTERFACE_HPP
#define ORO_EXECUTION_DATA_PORT_INTERFACE_HPP

#include <string>
#include "PortInterface.hpp"
#include "DataConnectionInterface.hpp"
#include "TemplateDataSourceFactory.hpp"
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

        ~ReadDataPort() {
            if (mconn)
                mconn->removeReader(this);
        }
        /**
         * Get the buffer to read from. The Task may use this to read from a
         * Buffer connected to this port.
         * @return 0 if !connected(), the buffer otherwise.
         */
        const ORO_CoreLib::DataObjectInterface<T>* data() const { return mconn ? mconn->data() : 0; }

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
            TemplateDataSourceFactory<ORO_CoreLib::DataObjectInterface<T> >* datas = newDataSourceFactory( mconn->data() );
            datas->add("Get", ORO_Execution::data( &ORO_CoreLib::DataObjectInterface<T>::Get, "Get the current value of this Data Object"));
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
        ORO_CoreLib::DataObjectInterface<T>* data() { return mconn ? mconn->data() : 0; }

        /**
         * Construct an unconnected Port to a readable buffer.
         * @param name The name of this port.
         */
        WriteDataPort(const std::string& name) : PortInterface(name), mconn() {}

        ~WriteDataPort() {
            if (mconn)
                mconn->removeWriter(this);
        }

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
            TemplateDataSourceFactory<ORO_CoreLib::DataObjectInterface<T> >* datas = newDataSourceFactory( mconn->data() );
            datas->add("Get", ORO_Execution::data( &ORO_CoreLib::DataObjectInterface<T>::Get, "Get the current value of this Data Object"));
            datas->add("Set", ORO_Execution::data( &ORO_CoreLib::DataObjectInterface<T>::Set, "Set the current value of this Data Object",
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
