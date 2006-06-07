/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:17 CET 2006  BufferPort.hpp 

                        BufferPort.hpp -  description
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
 
 
#ifndef ORO_EXECUTION_BUFFER_PORT_INTERFACE_HPP
#define ORO_EXECUTION_BUFFER_PORT_INTERFACE_HPP

#include <string>
#include "PortInterface.hpp"
#include "BufferConnectionInterface.hpp"
#include "TemplateDataSourceFactory.hpp"
#include "MemberFactoryComposite.hpp"

namespace ORO_Execution
{
    /**
     * A Port to a readable Buffer.
     * Use connection() to access the buffer. If the port is not
     * connected, connection() returns null.
     * @param T The type of the data of the buffer.
     */
    template<class T>
    class ReadBufferPort
        : public virtual PortInterface
    {
    protected:
        typename ReadConnectionInterface<T>::shared_ptr mconn;
    public:
        /**
         * Get the buffer to read from. The Task may use this to read from a
         * Buffer connected to this port.
         * @return 0 if !connected(), the buffer otherwise.
         */
        virtual ORO_CoreLib::ReadInterface<T>* read() const { return mconn ? mconn->read() : 0; }

        /**
         * Construct an unconnected Port to a readable buffer.
         * @param name The name of this port.
         */
        ReadBufferPort(const std::string& name) : PortInterface(name), mconn() {}

        ~ReadBufferPort() {
            if (mconn) 
                mconn->removeReader(this);
        }

        virtual ConnectionInterface::shared_ptr connection() const { return mconn; }

        /**
         * Pop a value from the buffer of this Port's connection.
         * @param data The location where to store the popped value.
         * @retval this->read()->Pop(data) if this->connected()
         * @retval false if !this->connected() 
         */
        bool Pop(T& data)
        {
            if (mconn)
                return mconn->read()->Pop(data);
            return false;
        }

        bool connected() const { return mconn; };

        /**
         * Connect a readable buffer to this Port.
         */
        virtual bool connect( typename ReadConnectionInterface<T>::shared_ptr conn) {
            if ( mconn || !conn )
                return false;
            mconn = conn;
            return true;
        }

        void disconnect() {
            mconn = 0;
        }

        bool connectTo( ConnectionInterface::shared_ptr other) {
            return other->addReader( this );
        }

        virtual DataSourceFactoryInterface* createDataSources() {
            if ( !mconn )
                return 0;
            TemplateDataSourceFactory<ORO_CoreLib::ReadInterface<T> >* datas = newDataSourceFactory( mconn->read() );
            bool (ORO_CoreLib::ReadInterface<T>::*PopPtr)(typename ORO_CoreLib::ReadInterface<T>::reference_t) = &ORO_CoreLib::ReadInterface<T>::Pop;
            datas->add( "Pop", data( PopPtr, "Pop a single value from the Buffer. Returns false if empty.",
                                     "Val", "The value returned by argument.") );
            datas->add( "front", data( &ORO_CoreLib::ReadInterface<T>::front, "Get the next to be popped value from the buffer. Returns default value if buffer is empty."));

            TemplateDataSourceFactory<ORO_CoreLib::BufferBase>* datab = newDataSourceFactory<ORO_CoreLib::BufferBase>( mconn->read() );
            datab->add("size", data( &ORO_CoreLib::BufferBase::size, "Get the used size of the buffer."));
            datab->add("capacity", data( &ORO_CoreLib::BufferBase::capacity, "Get the capacity of the buffer."));
            datab->add("empty", data( &ORO_CoreLib::BufferBase::empty, "Inspect if the buffer is empty."));
            datab->add("full", data( &ORO_CoreLib::BufferBase::full, "Inspect if the buffer is full."));

            return new MemberFactoryComposite( datas, datab );

        }
    };

    /**
     * A Port to a writable Buffer.
     * Use connection() to access the buffer. If the port is not
     * connected, connection() returns null.
     * @param T The type of the data of the buffer.
     */
    template<class T>
    class WriteBufferPort
        : public virtual PortInterface
    {
    protected:
        /**
         * The buffer to write from.
         */
        typename WriteConnectionInterface<T>::shared_ptr mconn;

        size_t buf_size;

        T minitial_value;
    public:
        /**
         * Construct an unconnected Port to a writeable buffer.
         * @param name The name of this port.
         * @param initial_value The initial value of this port's connection
         * when the connection is created. If this port is connected to an
         * existing connection, this value is ignored.
         */
        WriteBufferPort(const std::string& name, size_t preferred_size, const T& initial_value = T() )
            : PortInterface(name), mconn(), buf_size(preferred_size), minitial_value(initial_value) {}

        ~WriteBufferPort() {
            if (mconn) 
                mconn->removeWriter(this);
        }

        ConnectionInterface::shared_ptr createConnection(PortInterface* other, ConnectionTypes::ConnectionType con_type = ConnectionTypes::lockfree);

        /**
         * Set the prefered buffersize before this port is
         * connected. If this->connected(), this value has no
         * effect.
         */
        void setBufferSize(size_t b_size) { buf_size = b_size; }

        /**
         * Get the buffer to write from.
         * @return 0 if !connected(), the buffer otherwise.
         */
        virtual ORO_CoreLib::WriteInterface<T>* write() const { return mconn ? mconn->write() : 0; }

        virtual ConnectionInterface::shared_ptr connection() const { return mconn; }

        bool connected() const { return mconn; };

        /**
         * Push a value into the buffer of this Port's connection.
         * @param data The data to push.
         * @retval this->read()->Push(data) if this->connected()
         * @retval false if !this->connected()
         */
        bool Push(const T& data)
        {
            if (mconn)
                return mconn->write()->Push(data);
            return false;
        }

        /**
         * Set the initial value of the port's connection.
         * This value is only used when the connection is created.
         * If this port is connected to an existing connection,
         * the value is ignored.
         */
        void Set(const T& data)
        {
            minitial_value = data;
        }

        /**
         * Connect a writeable buffer to this Port.
         */
        virtual bool connect(typename WriteConnectionInterface<T>::shared_ptr conn) { 
            if ( mconn  || !conn  )
                return false;
            mconn = conn;
            return true;
        }

        bool connectTo( ConnectionInterface::shared_ptr other) {
            return other->addWriter( this );
        }

        void disconnect() {
            mconn = 0;
        }

        virtual DataSourceFactoryInterface* createDataSources() {
            if ( !mconn )
                return 0;
            TemplateDataSourceFactory<ORO_CoreLib::WriteInterface<T> >* datas = newDataSourceFactory( mconn->write() );
            bool (ORO_CoreLib::WriteInterface<T>::*PushPtr)(typename ORO_CoreLib::WriteInterface<T>::param_t) = &ORO_CoreLib::WriteInterface<T>::Push;
            datas->add("Push", data( PushPtr, "Push a single value in the Buffer. Returns false if full.",
                                        "Val", "The value.") );

            TemplateDataSourceFactory<ORO_CoreLib::BufferBase>* datab = newDataSourceFactory<ORO_CoreLib::BufferBase>( mconn->write() );
            datab->add("size", data( &ORO_CoreLib::BufferBase::size, "Get the used size of the buffer."));
            datab->add("capacity", data( &ORO_CoreLib::BufferBase::capacity, "Get the capacity of the buffer."));
            datab->add("empty", data( &ORO_CoreLib::BufferBase::empty, "Inspect if the buffer is empty."));
            datab->add("full", data( &ORO_CoreLib::BufferBase::full, "Inspect if the buffer is full."));
            return new MemberFactoryComposite( datas, datab );
        }
    };


    /**
     * A Port to a read-write Buffer.
     * Use connection() to access the buffer. If the port is not
     * connected, connection() returns null.
     * @param T The type of the data of the buffer.
     */
    template<class T>
    class BufferPort
        : public WriteBufferPort<T>, public ReadBufferPort<T>
    {
    protected:
        /**
         * The buffer to write from.
         */
        typename BufferConnectionInterface<T>::shared_ptr mconn;

    public:
        /**
         * Construct an unconnected Port to a writeable buffer.
         * @param name The name of this port.
         */
        BufferPort(const std::string& name, size_t prefered_size, const T& initial_value = T())
            : PortInterface(name), WriteBufferPort<T>(name,prefered_size, initial_value), ReadBufferPort<T>(name), mconn() {}

        ~BufferPort() {
            if (mconn) {
                mconn->removeReader(this);
                mconn->removeWriter(this);
            }
        }

        /**
         * Get the buffer to write from.
         * @return 0 if !connected(), the buffer otherwise.
         */
        virtual ORO_CoreLib::BufferInterface<T>* buffer() const { return mconn ? mconn->buffer() : 0; }

        virtual ConnectionInterface::shared_ptr connection() const { return mconn; }

        bool connected() const { return mconn; };

        /**
         * Connect a writeable buffer to this Port.
         */
        bool connect(typename BufferConnectionInterface<T>::shared_ptr conn) { 
            if ( (mconn && conn != mconn) || !conn ) // allow to connect twice to same connection.
                return false;
            mconn = conn;
            return true;
        }

        virtual bool connect(typename WriteConnectionInterface<T>::shared_ptr conn) { 
            if ( this->connect( boost::dynamic_pointer_cast<BufferConnectionInterface<T> >(conn) ) ) {
                 WriteBufferPort<T>::connect(conn);
                 return true;
            }
            return false;
        }

        virtual bool connect(typename ReadConnectionInterface<T>::shared_ptr conn) {
            if ( this->connect( boost::dynamic_pointer_cast<BufferConnectionInterface<T> >(conn) ) ) {
                 ReadBufferPort<T>::connect(conn);
                 return true;
            }
            return false;
        }

        bool connectTo( ConnectionInterface::shared_ptr other) {
            if ( !mconn ) {
                return other->addWriter( this ) && other->addReader( this );
            }
            return false;
        }

        void disconnect() {
            mconn = 0;
            WriteBufferPort<T>::disconnect();
            ReadBufferPort<T>::disconnect();
        }

        virtual DataSourceFactoryInterface* createDataSources() {
            if ( !mconn )
                return 0;
            TemplateDataSourceFactory<ORO_CoreLib::BufferInterface<T> >* datas = newDataSourceFactory( mconn->buffer() );
            bool (ORO_CoreLib::BufferInterface<T>::*PushPtr)(typename ORO_CoreLib::BufferInterface<T>::param_t) = &ORO_CoreLib::BufferInterface<T>::Push;
            bool (ORO_CoreLib::BufferInterface<T>::*PopPtr)(typename ORO_CoreLib::BufferInterface<T>::reference_t) = &ORO_CoreLib::BufferInterface<T>::Pop;
            typename ORO_CoreLib::BufferInterface<T>::value_t (ORO_CoreLib::BufferInterface<T>::*FrontPtr)() const = &ORO_CoreLib::BufferInterface<T>::front;
            datas->add("Push", data( PushPtr, "Push a single value in the Buffer. Returns false if full.",
                                        "Val", "The value.") );
            datas->add("Pop", data( PopPtr, "Pop a single value from the Buffer. Returns false if empty.",
                                       "Val", "The value returned by argument.") );
            datas->add("front", data( FrontPtr, "Get the next to be popped value from the buffer. Returns default value if buffer is empty."));

            TemplateDataSourceFactory<ORO_CoreLib::BufferBase>* datab = newDataSourceFactory<ORO_CoreLib::BufferBase>( mconn->buffer() );
            datab->add("size", data( &ORO_CoreLib::BufferBase::size, "Get the used size of the buffer."));
            datab->add("capacity", data( &ORO_CoreLib::BufferBase::capacity, "Get the capacity of the buffer."));
            datab->add("empty", data( &ORO_CoreLib::BufferBase::empty, "Inspect if the buffer is empty."));
            datab->add("full", data( &ORO_CoreLib::BufferBase::full, "Inspect if the buffer is full."));
            return new MemberFactoryComposite( datas, datab );
        }
    };
}

#include "ConnectionFactory.hpp"

namespace ORO_Execution
{
    template<class T>
    ConnectionInterface::shared_ptr WriteBufferPort<T>::createConnection(PortInterface* other, ConnectionTypes::ConnectionType con_type )
        {
            ConnectionFactory<T> cf;
            return ConnectionInterface::shared_ptr (cf.createBuffer(this, other, buf_size, minitial_value, con_type));
        }
}
#endif
