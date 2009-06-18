/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:17 CET 2006  BufferPort.hpp

                        BufferPort.hpp -  description
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


#ifndef ORO_EXECUTION_BUFFER_PORT_HPP
#define ORO_EXECUTION_BUFFER_PORT_HPP

#include <string>
#include "PortInterface.hpp"
#include "BufferConnection.hpp"
#include "OperationInterface.hpp"
#include "Method.hpp"
#include "TaskObject.hpp"

namespace RTT
{
    template<class T>
    class BufferPortBase
        : public PortInterface
    {
    protected:
        mutable typename BufferConnection<T>::shared_ptr mconn;

        /**
         * Connect a readable buffer to this Port.
         */
        virtual bool connect( typename ConnectionInterface::shared_ptr conn) {
            if ( mconn || !conn )
                return false;
            mconn = boost::dynamic_pointer_cast< BufferConnection<T> >(conn);
            return (mconn);
        }

    public:
        /**
         * Construct an unconnected Port to a readable buffer.
         * @param name The name of this port.
         */
        BufferPortBase(const std::string& name) : PortInterface(name), mconn() {}

        ~BufferPortBase() {
            if (mconn)
                mconn->removePort(this);
        }

        /**
         * Clears all contents of this buffer.
         */
        void clear() {
#ifndef ORO_EMBEDDED
            try {
#endif
                if ( mconn )
                    return mconn->buffer()->clear();
#ifndef ORO_EMBEDDED
            } catch (...) {
                mconn = 0;
            }
#endif
        }

        /**
         * Returns the actual number of items that are stored in the
         * buffer.
         * @return number of items.
         */
        BufferBase::size_type size() const {
#ifndef ORO_EMBEDDED
            try {
#endif
                if ( mconn )
                    return mconn->buffer()->size();
#ifndef ORO_EMBEDDED
            } catch (...) {
                mconn = 0;
            }
#endif
            return 0;
        }

        /**
         * Returns the maximum number of items that can be stored in the
         * buffer.
         * @return maximum number of items.
         */
        BufferBase::size_type capacity() const {
#ifndef ORO_EMBEDDED
            try {
#endif
                if ( mconn )
                    return mconn->buffer()->capacity();
#ifndef ORO_EMBEDDED
            } catch (...) {
                mconn = 0;
            }
#endif
            return 0;
        }

        /**
         * Check if this buffer is empty.
         * @return true if size() == 0
         */
        bool empty() const {
#ifndef ORO_EMBEDDED
            try {
#endif
                if ( mconn )
                    return mconn->buffer()->empty();
#ifndef ORO_EMBEDDED
            } catch (...) {
                mconn = 0;
            }
#endif
            return true;
        }

        /**
         * Check if this buffer is full.
         * @return true if size() == capacity()
         */
        bool full() const {
#ifndef ORO_EMBEDDED
            try {
#endif
                if ( mconn )
                    return mconn->buffer()->full();
#ifndef ORO_EMBEDDED
            } catch (...) {
                mconn = 0;
            }
#endif
            return false;
        }

        virtual ConnectionModel getConnectionModel() const { return Buffered; }

        virtual const TypeInfo* getTypeInfo() const { return detail::DataSourceTypeInfo<T>::getTypeInfo(); }

        /**
         * Provide a new implementation for the connection of this port.
         * If this port is not connected, a new connection is created.
         */
        BufferPortBase<T>& operator=(BufferInterface<T>* impl);

        bool connected() const { return mconn; };

        void disconnect() {
            mconn = 0;
        }

        using PortInterface::connectTo;
        bool connectTo( ConnectionInterface::shared_ptr other) {
            return other && !mconn && other->addPort( this );
        }

        /**
         * Get the buffer to read from or write to.
         * @return 0 if !connected(), the buffer otherwise.
         */
        virtual BufferInterface<T>* buffer() { return mconn ? mconn->buffer() : 0; }

        virtual const BufferInterface<T>* buffer() const { return mconn ? mconn->buffer() : 0; }

        virtual ConnectionInterface::shared_ptr connection() const { return mconn; }

        ConnectionInterface::shared_ptr createConnection(BufferBase::shared_ptr impl) {
            typename BufferInterface<T>::shared_ptr buf = boost::dynamic_pointer_cast< BufferInterface<T> >( impl );
            ConnectionInterface::shared_ptr ci( new BufferConnection<T>( buf ) );
            ci->addPort(this);
            return ci;
        }

        virtual TaskObject* createPortObject() {
#ifndef ORO_EMBEDDED
            TaskObject* to = new TaskObject( this->getName() );
            to->methods()->addMethod( method("ready",&PortInterface::ready, this),
                                      "Check if this port is connected and ready for use.");
            to->methods()->addMethod(method("size", &BufferPortBase<T>::size, this),
                                     "Get the used size of the buffer.");
            to->methods()->addMethod(method("capacity", &BufferPortBase<T>::capacity, this),
                                     "Get the capacity of the buffer.");
            to->methods()->addMethod(method("empty", &BufferPortBase<T>::empty, this),
                                     "Inspect if the buffer is empty.");
            to->methods()->addMethod(method("full", &BufferPortBase<T>::full, this),
                                     "Inspect if the buffer is full.");
            to->methods()->addMethod(method("clear", &BufferPortBase<T>::clear, this),
                                     "Clear the contents of the buffer.");
            return to;
#else
            return 0;
#endif
        }
    };

    /**
     * A Port to a readable Buffer.
     * Use connection() to access the buffer. If the port is not
     * connected, connection() returns null.
     * @param T The type of the data of the buffer.
     * @ingroup Ports
     */
    template<class T>
    class ReadBufferPort
        : public BufferPortBase<T>
    {
        using BufferPortBase<T>::mconn;
    public:
        using BufferPortBase<T>::operator=;

        /**
         * Construct an unconnected Port to a readable buffer.
         * @param name The name of this port.
         */
        ReadBufferPort(const std::string& name) : BufferPortBase<T>(name){}

        /**
         * Pop a value from the buffer of this Port's connection.
         * @param data The location where to store the popped value.
         * @retval this->buffer()->Pop(data) if this->connected()
         * @retval false if !this->connected()
         */
        bool Pop(T& data)
        {
#ifndef ORO_EMBEDDED
            try {
#endif
                if ( mconn )
                    return mconn->buffer()->Pop(data);
#ifndef ORO_EMBEDDED
            } catch (...) {
                mconn = 0;
            }
#endif
            return false;
        }

        /**
         * Get the next value to be Pop()'ed, or
         * the default value if empty.
         */
        T front() const {
#ifndef ORO_EMBEDDED
            try {
#endif
                if ( mconn )
                    return mconn->buffer()->front();
#ifndef ORO_EMBEDDED
            } catch (...) {
                mconn = 0;
            }
#endif
            return T();
        }

        virtual PortInterface::PortType getPortType() const { return PortInterface::ReadPort; }

        virtual PortInterface* clone() const {
            return new ReadBufferPort<T>( this->getName() );
        }

        /**
         * The anti-Clone of a ReadBufferPort is a WriteBufferPort with
         * prefered buffer size '1'.
         */
        virtual PortInterface* antiClone() const;

        virtual TaskObject* createPortObject() {
#ifndef ORO_EMBEDDED
            TaskObject* to = BufferPortBase<T>::createPortObject();
            to->methods()->addMethod(method("Pop", &ReadBufferPort<T>::Pop, this),
                                     "Pop a single value from the Buffer. Returns false if empty.",
                                     "Val", "The value returned by argument.");
            to->methods()->addMethod(method("front", &ReadBufferPort<T>::front, this),
                                     "Get the next to be popped value from the buffer. Returns default value if buffer is empty.");
            return to;
#else
            return 0;
#endif
        }
    };

    /**
     * A Port to a writable Buffer.
     * Use connection() to access the buffer. If the port is not
     * connected, connection() returns null.
     * @param T The type of the data of the buffer.
     * @ingroup Ports
     */
    template<class T>
    class WriteBufferPort
        : public BufferPortBase<T>
    {
    protected:
        size_t buf_size;

        T minitial_value;
        using BufferPortBase<T>::mconn;
    public:
        using BufferPortBase<T>::operator=;
        /**
         * Construct an unconnected Port to a writeable buffer.
         * @param name The name of this port.
         * @param initial_value The initial value of this port's connection
         * when the connection is created. If this port is connected to an
         * existing connection, this value is ignored.
         */
        WriteBufferPort(const std::string& name, size_t preferred_size, const T& initial_value = T() )
            : BufferPortBase<T>(name), buf_size(preferred_size), minitial_value(initial_value) {}

        /**
         * Set the prefered buffersize before this port is
         * connected. If this->connected(), this value has no
         * effect.
         */
        void setBufferSize(size_t b_size) { buf_size = b_size; }

        /**
         * Push a value into the buffer of this Port's connection.
         * @param data The data to push.
         * @retval this->buffer()->Push(data) if this->connected()
         * @retval false if !this->connected()
         */
        bool Push(const T& data)
        {
#ifndef ORO_EMBEDDED
            try {
#endif
                if ( mconn && mconn->buffer()->Push(data) ) {
                    mconn->signal();
                    return true;
                }

#ifndef ORO_EMBEDDED
            } catch (...) {
                mconn = 0;
            }
#endif
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

        virtual PortInterface::PortType getPortType() const { return PortInterface::WritePort; }

        ConnectionInterface::shared_ptr createConnection(ConnectionTypes::ConnectionType con_type = ConnectionTypes::lockfree);

        virtual PortInterface* clone() const {
            return new WriteBufferPort<T>( this->getName(), buf_size, minitial_value );
        }

        virtual PortInterface* antiClone() const {
            return new ReadBufferPort<T>( this->getName() );
        }

        virtual TaskObject* createPortObject() {
#ifndef ORO_EMBEDDED
            TaskObject* to = BufferPortBase<T>::createPortObject();
            to->methods()->addMethod(method("Push", &WriteBufferPort<T>::Push, this),
                                     "Push a single value in the Buffer. Returns false if full().",
                                     "Val", "The value.");
            return to;
#else
            return 0;
#endif
        }
    };


    /**
     * A Port to a read-write Buffer.
     * Use connection() to access the buffer. If the port is not
     * connected, connection() returns null.
     * @param T The type of the data of the buffer.
     * @ingroup Ports
     * @ingroup RTTComponentInterface
     */
    template<class T>
    class BufferPort
        : public BufferPortBase<T>
    {
    protected:
        size_t buf_size;

        T minitial_value;
        using BufferPortBase<T>::mconn;
    public:
        using BufferPortBase<T>::operator=;

        typedef PortInterface::PortType PortType;
        typedef PortInterface::ConnectionModel ConnectionModel;

        /**
         * Construct an unconnected Port to a writeable buffer.
         * @param name The name of this port.
         */
        BufferPort(const std::string& name, size_t prefered_size, const T& initial_value = T())
            : BufferPortBase<T>(name), buf_size(prefered_size), minitial_value(initial_value)
            {}

        /**
         * Set the prefered buffersize before this port is
         * connected. If this->connected(), this value has no
         * effect.
         */
        void setBufferSize(size_t b_size) { buf_size = b_size; }

        /**
         * Pop a value from the buffer of this Port's connection.
         * @param data The location where to store the popped value.
         * @retval this->buffer()->Pop(data) if this->connected()
         * @retval false if !this->connected()
         */
        bool Pop(T& data)
        {
#ifndef ORO_EMBEDDED
            try {
#endif
                if ( mconn )
                    return mconn->buffer()->Pop(data);
#ifndef ORO_EMBEDDED
            } catch (...) {
                mconn = 0;
            }
#endif
            return false;
        }

        /**
         * Get the next value to be Pop()'ed, or
         * the default value if empty.
         */
        T front() const {
#ifndef ORO_EMBEDDED
            try {
#endif
                if ( mconn )
                    return mconn->buffer()->front();
#ifndef ORO_EMBEDDED
            } catch (...) {
                mconn = 0;
            }
#endif
            return T();
        }

        /**
         * Push a value into the buffer of this Port's connection.
         * @param data The data to push.
         * @retval this->buffer()->Push(data) if this->connected()
         * @retval false if !this->connected()
         */
        bool Push(const T& data)
        {
#ifndef ORO_EMBEDDED
            try {
#endif
                if ( mconn && mconn->buffer()->Push(data) ) {
                    mconn->signal();
                    return true;
                }
#ifndef ORO_EMBEDDED
            } catch (...) {
                mconn = 0;
            }
#endif
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


        virtual PortInterface::PortType getPortType() const { return PortInterface::ReadWritePort; }

        virtual PortInterface* clone() const {
            return new BufferPort<T>( this->getName(), this->buf_size, this->minitial_value );
        }

        virtual PortInterface* antiClone() const {
            return this->clone();
        }

        virtual TaskObject* createPortObject() {
#ifndef ORO_EMBEDDED
            TaskObject* to = BufferPortBase<T>::createPortObject();
            to->methods()->addMethod(method("Push", &BufferPort<T>::Push, this),
                                     "Push a single value in the Buffer. Returns false if full().",
                                     "Val", "The value.");
            to->methods()->addMethod(method("Pop", &BufferPort<T>::Pop, this),
                                     "Pop a single value from the Buffer. Returns false if empty.",
                                     "Val", "The value returned by argument.");
            to->methods()->addMethod(method("front", &BufferPort<T>::front, this),
                                     "Get the next to be popped value from the buffer. Returns default value if buffer is empty.");
            return to;
#else
            return 0;
#endif
        }

        ConnectionInterface::shared_ptr createConnection(ConnectionTypes::ConnectionType con_type);
    };
}
#endif

#ifndef ORO_BUFFER_PORT_INLINE
#define ORO_BUFFER_PORT_INLINE

#include "ConnectionFactory.hpp"
#include "BufferConnection.hpp"

namespace RTT
{

    template<class T>
    PortInterface* ReadBufferPort<T>::antiClone() const {
        return new WriteBufferPort<T>( this->getName(), 1 );
    }

    template<class T>
    BufferPortBase<T>& BufferPortBase<T>::operator=(BufferInterface<T>* impl)
    {
        if ( !mconn ) {
            ConnectionInterface::shared_ptr con = new BufferConnection<T>( typename BufferInterface<T>::shared_ptr(impl) );
            this->connectTo(con);
            con->connect();
        } else
            mconn->setImplementation(impl);
        return *this;
    }

    template<class T>
    ConnectionInterface::shared_ptr WriteBufferPort<T>::createConnection(ConnectionTypes::ConnectionType con_type )
        {
            ConnectionFactory<T> cf;
            ConnectionInterface::shared_ptr res = cf.createBuffer(buf_size, minitial_value, con_type);
            res->addPort(this);
            return res;
        }

    template<class T>
    ConnectionInterface::shared_ptr BufferPort<T>::createConnection(ConnectionTypes::ConnectionType con_type )
        {
            ConnectionFactory<T> cf;
            ConnectionInterface::shared_ptr res = cf.createBuffer(buf_size, minitial_value, con_type);
            res->addPort(this);
            return res;
        }
}
#endif
