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
#include "TemplateMethodFactory.hpp"
#include "MemberFactoryComposite.hpp"

namespace ORO_Execution
{
    /**
     * A Port to a readable Buffer.
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
        virtual ReadInterface<T>* read() const { return mconn ? mconn->read() : 0; }

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
            TemplateDataSourceFactory<ReadInterface<T> >* datas = newDataSourceFactory( mconn->read() );
            bool (ReadInterface<T>::*PopPtr)(typename ReadInterface<T>::reference_t) = &ReadInterface<T>::Pop;
            datas->add( "Pop", data( PopPtr, "Pop a single value from the Buffer. Returns false if empty.",
                                     "Val", "The value returned by argument.") );
            datas->add( "front", data( &ReadInterface<T>::front, "Get the next to be popped value from the buffer. Returns default value if buffer is empty."));

            TemplateDataSourceFactory<BufferBase>* datab = newDataSourceFactory<BufferBase>( mconn->read() );
            datab->add("size", data( &BufferBase::size, "Get the used size of the buffer."));
            datab->add("capacity", data( &BufferBase::capacity, "Get the capacity of the buffer."));
            datab->add("empty", data( &BufferBase::empty, "Inspect if the buffer is empty."));
            return new MemberFactoryComposite( datas, datab );

        }
    };

    /**
     * A Port to a writable Buffer.
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
    public:
        /**
         * Construct an unconnected Port to a writeable buffer.
         * @param name The name of this port.
         */
        WriteBufferPort(const std::string& name, size_t preferred_size )
            : PortInterface(name), mconn(), buf_size(preferred_size) {}

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
        virtual WriteInterface<T>* write() const { return mconn ? mconn->write() : 0; }

        virtual ConnectionInterface::shared_ptr connection() const { return mconn; }

        bool connected() const { return mconn; };

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
            TemplateDataSourceFactory<WriteInterface<T> >* datas = newDataSourceFactory( mconn->write() );
            bool (WriteInterface<T>::*PushPtr)(typename WriteInterface<T>::param_t) = &WriteInterface<T>::Push;
            datas->add("Push", data( PushPtr, "Push a single value in the Buffer. Returns false if full.",
                                        "Val", "The value.") );

            TemplateDataSourceFactory<BufferBase>* datab = newDataSourceFactory<BufferBase>( mconn->write() );
            datab->add("size", data( &BufferBase::size, "Get the used size of the buffer."));
            datab->add("capacity", data( &BufferBase::capacity, "Get the capacity of the buffer."));
            datab->add("empty", data( &BufferBase::empty, "Inspect if the buffer is empty."));
            return new MemberFactoryComposite( datas, datab );
        }
    };


    /**
     * A Port to a read-write Buffer.
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
        BufferPort(const std::string& name, size_t prefered_size)
            : PortInterface(name), WriteBufferPort<T>(name,prefered_size), ReadBufferPort<T>(name), mconn() {}

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
        virtual BufferInterface<T>* buffer() const { return mconn ? mconn->buffer() : 0; }

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
            TemplateDataSourceFactory<BufferInterface<T> >* datas = newDataSourceFactory( mconn->buffer() );
            bool (BufferInterface<T>::*PushPtr)(typename BufferInterface<T>::param_t) = &BufferInterface<T>::Push;
            bool (BufferInterface<T>::*PopPtr)(typename BufferInterface<T>::reference_t) = &BufferInterface<T>::Pop;
            typename BufferInterface<T>::value_t (BufferInterface<T>::*FrontPtr)() const = &BufferInterface<T>::front;
            datas->add("Push", data( PushPtr, "Push a single value in the Buffer. Returns false if full.",
                                        "Val", "The value.") );
            datas->add("Pop", data( PopPtr, "Pop a single value from the Buffer. Returns false if empty.",
                                       "Val", "The value returned by argument.") );
            datas->add("front", data( FrontPtr, "Get the next to be popped value from the buffer. Returns default value if buffer is empty."));

            TemplateDataSourceFactory<BufferBase>* datab = newDataSourceFactory<BufferBase>( mconn->buffer() );
            datab->add("size", data( &BufferBase::size, "Get the used size of the buffer."));
            datab->add("capacity", data( &BufferBase::capacity, "Get the capacity of the buffer."));
            datab->add("empty", data( &BufferBase::empty, "Inspect if the buffer is empty."));
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
            ConnectionFactory cf;
            return ConnectionInterface::shared_ptr (cf.createBuffer<T>(this, other, buf_size, con_type));
        }
}
#endif
