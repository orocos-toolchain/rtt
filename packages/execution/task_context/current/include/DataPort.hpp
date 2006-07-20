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
#include "OperationInterface.hpp"
#include "Method.hpp"
#include "ConnectionTypes.hpp"

namespace ORO_Execution
{
    /**
     * A Port to a readable Data Connection.
     * Use connection() to access the data object. If the port is not
     * connected, connection() returns null.
     * @param T The type of the data of the data object.
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
         * Construct an unconnected Port to a readable data object.
         * @param name The name of this port.
         */
        ReadDataPort(const std::string& name) : PortInterface(name), mconn() {}

        ~ReadDataPort() {
            if (mconn)
                mconn->removeReader(this);
        }

        /**
         * Get the data object to read from. The Task may use this to read from a
         * Data object connection connected to this port.
         * @return 0 if !connected(), the Data Object otherwise.
         */
        const ORO_CoreLib::DataObjectInterface<T>* data() const { return mconn ? mconn->data() : 0; }

        /**
         * Get the current value of this Port.
         * @retval this->data()->Get() if this->connected()
         * @retval T() if !this->connected()
         */
        T Get() const
        {
            if ( mconn )
                return mconn->data()->Get();
            return T();
        }

        /**
         * Connect a readable data object connection to this Port.
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

        virtual PortInterface* clone() const {
            return new ReadDataPort<T>( this->getName() );
        }

        virtual PortInterface* antiClone() const;

        virtual OperationInterface* createPortObject() {
#ifndef ORO_EMBEDDED
            TaskObject* to = new TaskObject( this->getName() );
            to->methods()->addMethod( method("Get",&ReadDataPort<T>::Get, this),
                                      "Get the current value of this Data Port");
            return to;
#else
            return 0;
#endif
        }

    };

    /**
     * A Port to a readable and writable Data Connection.
     * Use connection() to access the data object. If the port is not
     * connected, connection() returns null.
     * @param T The type of the data of the Data Object.
     */
    template<class T>
    class WriteDataPort
        : public PortInterface
    {
    public:
        typedef T DataType;
    protected:
        /**
         * The connection to write to
         */
        typename DataConnectionInterface<T>::shared_ptr mconn;

        DataType minitial_value;
    public:
        ConnectionInterface::shared_ptr createConnection(PortInterface* other, ConnectionTypes::ConnectionType con_type = ConnectionTypes::lockfree);

        /**
         * Get the data object to write to. The Task may use this to write to a
         * Data Object connected to this port.
         * @return 0 if !connected(), the data object otherwise.
         */
        ORO_CoreLib::DataObjectInterface<T>* data() { return mconn ? mconn->data() : 0; }

        /**
         * Get the data object to read from. The Task may use this to read from a
         * Data Object connected to this port.
         * @return 0 if !connected(), the data object otherwise.
         */
        const ORO_CoreLib::DataObjectInterface<T>* data() const { return mconn ? mconn->data() : 0; }

        /**
         * Write data to the connection of this port.
         * If the port is not connected, this methods sets the
         * initial value of the connection to \a data.
         * @param data The data to write to this port.
         */
        void Set(const T& data )
        {
            if ( mconn )
                mconn->data()->Set(data);
            else
                minitial_value = data;
        }

        /**
         * Get the current value of this Port.
         * If the port is not connected, a default value is returned.
         * @retval this->data()->Get() if this->connected()
         * @retval initial_value if !this->connected()
         */
        T Get() const
        {
            if ( mconn )
                return mconn->data()->Get();
            else
                return minitial_value;
        }

        /**
         * Construct an unconnected Port to a writable DataObject.
         * @param name The name of this port.
         * @param initial_value The initial value of this port's connection
         * when the connection is created. If this port is connected to an
         * existing connection, this value is ignored.
         */
        WriteDataPort(const std::string& name, const DataType& initial_value = DataType() )
            : PortInterface(name), mconn(), minitial_value(initial_value) {}

        ~WriteDataPort() {
            if (mconn)
                mconn->removeWriter(this);
        }

        /**
         * Connect an existing data object connection to this Port.
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

        virtual PortInterface* clone() const {
            return new WriteDataPort<T>( this->getName(), minitial_value );
        }

        virtual PortInterface* antiClone() const {
            return new ReadDataPort<T>( this->getName() );
        }

        virtual OperationInterface* createPortObject() {
#ifndef ORO_EMBEDDED
            TaskObject* to = new TaskObject( this->getName() );
            to->methods()->addMethod( method("Get",&WriteDataPort<T>::Get, this),
                                      "Get the current value of this Data Port");
            to->methods()->addMethod( method("Set",&WriteDataPort<T>::Set, this),
                                      "Set the current value of this Data Port",
                                      "Value", "The new value.");
            return to;
#else
            return 0;
#endif
        }
    };


}

#include "ConnectionFactory.hpp"

namespace ORO_Execution
{

    template<class T>
    PortInterface* ReadDataPort<T>::antiClone() const {
        return new WriteDataPort<T>( this->getName() );
    }

    template<class T>
    ConnectionInterface::shared_ptr WriteDataPort<T>::createConnection(PortInterface* other, ConnectionTypes::ConnectionType con_type)
    {
        ConnectionFactory<T> cf;
        return ConnectionInterface::shared_ptr ( cf.createDataObject(this, other, minitial_value, con_type) );
    }
}

#endif
