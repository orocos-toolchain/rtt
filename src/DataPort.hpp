/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:18 CET 2006  DataPort.hpp 

                        DataPort.hpp -  description
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
 
 
#ifndef ORO_EXECUTION_DATA_PORT_INTERFACE_HPP
#define ORO_EXECUTION_DATA_PORT_INTERFACE_HPP

#include <string>
#include "PortInterface.hpp"
#include "DataConnectionInterface.hpp"
#include "OperationInterface.hpp"
#include "Method.hpp"
#include "ConnectionTypes.hpp"

namespace RTT
{
    /**
     * A Port to a readable Data Connection.
     * Use connection() to access the data object. If the port is not
     * connected, connection() returns null.
     * @param T The type of the data of the data object.
     * @ingroup Ports
     */
    template<class T>
    class ReadDataPort
        : public virtual PortInterface
    {
        typedef T DataType;
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
         * Provide a new implementation for the connection of this port.
         * If this port is not connected, a new connection is created.
         */
        ReadDataPort& operator=(DataObjectInterface<T>* impl);

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
         * Get the current value of this Port.
         * @param result the variable in which the port value will be stored.
         * @post result == this->data()->Get(result) if this->connected()
         * @post result is unmodified if !this->connected()
         */
        void Get(T& result) const
        {
            if ( mconn )
                return mconn->data()->Get(result);
        }

        virtual PortType getPortType() const { return ReadPort; }

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

        using PortInterface::connectTo;
        bool connectTo( ConnectionInterface::shared_ptr other) {
            return other && !mconn && other->addReader( this );
        }

        void disconnect() {
            mconn = 0;
        }

        /**
         * Get the data object to read from. The Task may use this to read from a
         * Data object connection connected to this port.
         * @return 0 if !connected(), the Data Object otherwise.
         */
        const DataObjectInterface<T>* data() const { return mconn ? mconn->data() : 0; }

        virtual PortInterface* clone() const {
            return new ReadDataPort<T>( this->getName() );
        }

        virtual PortInterface* antiClone() const;

        virtual TaskObject* createPortObject() {
#ifndef ORO_EMBEDDED
            typedef T (ReadDataPort<T>::*GetType)(void) const;
            GetType get_type = &ReadDataPort<T>::Get;
            TaskObject* to = new TaskObject( this->getName() );
            to->methods()->addMethod( method("ready",&PortInterface::ready, this),
                                      "Check if this port is connected and ready for use.");
            to->methods()->addMethod( method("Get",get_type, this),
                                      "Get the current value of this Read Data Port");
            return to;
#else
            return 0;
#endif
        }

    protected:
        /**
         * The connection to read from.
         */
        typename DataConnectionInterface<T>::shared_ptr mconn;

    };

    /**
     * A Port to a writable Data Connection.
     * Use connection() to access the data object. If the port is not
     * connected, connection() returns null.
     * @param T The type of the data of the Data Object.
     * @ingroup Ports
     */
    template<class T>
    class WriteDataPort
        : public virtual PortInterface
    {
    public:
        typedef T DataType;

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
         * Provide a new implementation for the connection of this port.
         * If this port is not connected, a new connection is created.
         */
        WriteDataPort& operator=(DataObjectInterface<T>* impl);

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

        virtual PortType getPortType() const { return WritePort; }

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

        using PortInterface::connectTo;
        bool connectTo( ConnectionInterface::shared_ptr other) {
            return other && !mconn && other->addWriter( this );
        }

        void disconnect() {
            mconn = 0;
        }

        /**
         * Get the data object to write to. The Task may use this to write to a
         * Data Object connected to this port.
         * @return 0 if !connected(), the data object otherwise.
         */
        DataObjectInterface<T>* data() { return mconn ? mconn->data() : 0; }

        /**
         * Get the data object to read from. The Task may use this to read from a
         * Data Object connected to this port.
         * @return 0 if !connected(), the data object otherwise.
         */
        const DataObjectInterface<T>* data() const { return mconn ? mconn->data() : 0; }

        virtual PortInterface* clone() const {
            return new WriteDataPort<T>( this->getName(), minitial_value );
        }

        virtual PortInterface* antiClone() const {
            return new ReadDataPort<T>( this->getName() );
        }

        ConnectionInterface::shared_ptr createConnection(PortInterface* other, ConnectionTypes::ConnectionType con_type = ConnectionTypes::lockfree);

        ConnectionInterface::shared_ptr createConnection(ConnectionTypes::ConnectionType con_type);

        virtual TaskObject* createPortObject() {
#ifndef ORO_EMBEDDED
            TaskObject* to = new TaskObject( this->getName() );
            to->methods()->addMethod( method("ready",&PortInterface::ready, this),
                                      "Check if this port is connected and ready for use.");
            to->methods()->addMethod( method("Set",&WriteDataPort<T>::Set, this),
                                      "Set the current value of this Write Data Port",
                                      "Value", "The new value.");
            return to;
#else
            return 0;
#endif
        }

    protected:
        /**
         * The connection to write to
         */
        typename DataConnectionInterface<T>::shared_ptr mconn;

        DataType minitial_value;
    };


    /**
     * A data port which can be used as a reader and as a writer.
     * @ingroup Ports
     * @ingroup RTTComponentInterface
     */
    template<class T>
    class DataPort
        : public ReadDataPort<T>,
          public WriteDataPort<T>
    {
    public:
        typedef T DataType;

        /**
         * Construct an unconnected Port to a writable DataObject.
         * @param name The name of this port.
         * @param initial_value The initial value of this port's connection
         * when the connection is created. If this port is connected to an
         * existing connection, this value is ignored.
         */
        DataPort(const std::string& name, const DataType& initial_value = DataType() )
            : PortInterface(name),
              ReadDataPort<T>(name),
              WriteDataPort<T>(name, initial_value)
        {}

        ~DataPort() {
        }

        /**
         * Provide a new implementation for the connection of this port.
         * If this port is not connected, a new connection is created.
         */
        DataPort& operator=(DataObjectInterface<T>* impl);

        /**
         * Write data to the connection of this port.
         * If the port is not connected, this methods sets the
         * initial value of the connection to \a data.
         * @param data The data to write to this port.
         */
        void Set(const DataType& data )
        {
            WriteDataPort<T>::Set(data);
        }

        /**
         * Get the current value of this Port.
         * If the port is not connected, a default value is returned.
         * @retval this->data()->Get() if this->connected()
         * @retval initial_value if !this->connected()
         */
        DataType Get() const
        {
            if ( WriteDataPort<T>::mconn )
                return WriteDataPort<T>::mconn->data()->Get();
            else
                return WriteDataPort<T>::minitial_value;
        }

        /**
         * Get the current value of this Port.
         * If the port is not connected, a default value is returned.
         * @param result The variable to store the result in.
         * @post this->data()->Get(result) if this->connected()
         * @post result == initial_value if !this->connected()
         */
        void Get(DataType& result) const
        {
            if ( WriteDataPort<T>::mconn )
                WriteDataPort<T>::mconn->data()->Get(result);
            else
                result = WriteDataPort<T>::minitial_value;
        }

        virtual PortInterface::PortType getPortType() const { return PortInterface::ReadWritePort; }

        bool connect(typename DataConnectionInterface<T>::shared_ptr conn) { 
            return WriteDataPort<T>::connect(conn) && ReadDataPort<T>::connect(conn);
        }

        virtual ConnectionInterface::shared_ptr connection() const { return WriteDataPort<T>::connection(); }

        bool connected() const { return WriteDataPort<T>::connected() && ReadDataPort<T>::connected(); }

        using PortInterface::connectTo;
        bool connectTo( ConnectionInterface::shared_ptr other) {
            return WriteDataPort<T>::connectTo(other) && ReadDataPort<T>::connectTo(other);
        }

        void disconnect() {
            WriteDataPort<T>::disconnect();
            ReadDataPort<T>::disconnect();
        }

        DataObjectInterface<T>* data() { return WriteDataPort<T>::data(); }

        const DataObjectInterface<T>* data() const { return WriteDataPort<T>::data(); }

        virtual PortInterface* clone() const {
            return new DataPort<T>( this->getName(), this->minitial_value );
        }

        virtual PortInterface* antiClone() const {
            return this->clone();
        }

        virtual TaskObject* createPortObject() {
#ifndef ORO_EMBEDDED
            typedef T (DataPort<T>::*GetType)(void) const;
            GetType get_type = &DataPort<T>::Get;
            TaskObject* to = new TaskObject( this->getName() );
            to->methods()->addMethod( method("ready",&PortInterface::ready, this),
                                      "Check if this port is connected and ready for use.");
            to->methods()->addMethod( method("Get", get_type, this),
                                      "Get the current value of this Data Port");
            to->methods()->addMethod( method("Set",&DataPort<T>::Set, this),
                                      "Set the current value of this Data Port",
                                      "Value", "The new value.");
            return to;
#else
            return 0;
#endif
        }

        ConnectionInterface::shared_ptr createConnection(PortInterface* other, ConnectionTypes::ConnectionType con_type = ConnectionTypes::lockfree) 
        {
            ConnectionInterface::shared_ptr ret = WriteDataPort<T>::createConnection(other, con_type);
            ret->addReader( this );
            ret->addWriter( other );
            return ret;
        }

        ConnectionInterface::shared_ptr createConnection(ConnectionTypes::ConnectionType con_type)
        {
            ConnectionInterface::shared_ptr ret = WriteDataPort<T>::createConnection(con_type);
            ret->addReader( this );
            return ret;
        }

    };
}
#endif

#ifndef ORO_DATA_PORT_INLINE
#define ORO_DATA_PORT_INLINE

#include "ConnectionFactory.hpp"
#include "DataConnection.hpp"

namespace RTT
{

    template<class T>
    PortInterface* ReadDataPort<T>::antiClone() const {
        return new WriteDataPort<T>( this->getName() );
    }

    template<class T>
    ReadDataPort<T>& ReadDataPort<T>::operator=(DataObjectInterface<T>* impl)
    {
        if ( !mconn ) {
            ConnectionInterface::shared_ptr con = new DataConnection<T>(impl);
            this->connectTo(con);
            con->connect();
            } else
                mconn->setImplementation(impl);
        return *this;
    }

    template<class T>
    WriteDataPort<T>& WriteDataPort<T>::operator=(DataObjectInterface<T>* impl)
    {
        if ( !mconn ) {
            ConnectionInterface::shared_ptr con = new DataConnection<T>(impl);
            this->connectTo(con);
            con->connect();
            } else
                mconn->setImplementation(impl);
        return *this;
    }

    template<class T>
    DataPort<T>& DataPort<T>::operator=(DataObjectInterface<T>* impl)
    {
        if ( !connected() ) {
            ConnectionInterface::shared_ptr con = new DataConnection<T>(impl);
            this->connectTo(con);
            con->connect();
        } else
            WriteDataPort<T>::mconn->setImplementation(impl);
        return *this;
    }

    template<class T>
    ConnectionInterface::shared_ptr WriteDataPort<T>::createConnection(PortInterface* other, ConnectionTypes::ConnectionType con_type)
    {
        ConnectionFactory<T> cf;
        return ConnectionInterface::shared_ptr ( cf.createDataObject(this, other, minitial_value, con_type) );
    }

    template<class T>
    ConnectionInterface::shared_ptr WriteDataPort<T>::createConnection(ConnectionTypes::ConnectionType con_type)
    {
        ConnectionFactory<T> cf;
        ConnectionInterface::shared_ptr res = cf.createDataObject(minitial_value, con_type);
        res->addWriter(this);
        return res;
    }

}

#endif
