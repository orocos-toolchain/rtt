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
#include "OperationInterface.hpp"
#include "Method.hpp"

namespace RTT
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

        /**
         * Get the next value to be Pop()'ed, or
         * the default value if empty.
         */
        T front() const {
            if (mconn)
                return mconn->read()->front();
            return T();
        }

        virtual PortType getPortType() const { return ReadPort; }

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
            return !mconn && other->addReader( this );
        }

        /**
         * Clears all contents of this buffer.
         */
        void clear() {
            if (mconn)
                return mconn->read()->clear();
        }

        /**
         * Returns the actual number of items that are stored in the
         * buffer.
         * @return number of items.
         */
        BufferBase::size_type size() const {
            if (mconn)
                return mconn->read()->size();
            return 0;
        }

        /**
         * Returns the maximum number of items that can be stored in the
         * buffer.
         * @return maximum number of items.
         */
        BufferBase::size_type capacity() const {
            if (mconn)
                return mconn->read()->capacity();
            return 0;
        }
            
        /**
         * Check if this buffer is empty.
         * @return true if size() == 0
         */
        bool empty() const {
            if (mconn)
                return mconn->read()->empty();
            return 0;
        }
            
        /**
         * Check if this buffer is full.
         * @return true if size() == capacity()
         */
        bool full() const {
            if (mconn)
                return mconn->read()->full();
            return 0;
        }

        virtual PortInterface* clone() const {
            return new ReadBufferPort<T>( this->getName() );
        }

        /**
         * The anti-Clone of a ReadBufferPort is a WriteBufferPort with
         * prefered buffer size '1'.
         */
        virtual PortInterface* antiClone() const;

        virtual OperationInterface* createPortObject() {
#ifndef ORO_EMBEDDED
            TaskObject* to = new TaskObject( this->getName() );
            to->methods()->addMethod(method("Pop", &ReadBufferPort<T>::Pop, this),
                                     "Pop a single value from the Buffer. Returns false if empty.",
                                     "Val", "The value returned by argument.");
            to->methods()->addMethod(method("front", &ReadBufferPort<T>::front, this),
                                     "Get the next to be popped value from the buffer. Returns default value if buffer is empty.");
            to->methods()->addMethod(method("size", &ReadBufferPort<T>::size, this),
                                     "Get the used size of the buffer.");
            to->methods()->addMethod(method("capacity", &ReadBufferPort<T>::capacity, this),
                                     "Get the capacity of the buffer.");
            to->methods()->addMethod(method("empty", &ReadBufferPort<T>::empty, this),
                                     "Inspect if the buffer is empty.");
            to->methods()->addMethod(method("full", &ReadBufferPort<T>::full, this),
                                     "Inspect if the buffer is full.");
            to->methods()->addMethod(method("clear", &ReadBufferPort<T>::clear, this),
                                     "Clear the contents of the buffer.");
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
        virtual WriteInterface<T>* write() const { return mconn ? mconn->write() : 0; }

        virtual ConnectionInterface::shared_ptr connection() const { return mconn; }

        virtual PortType getPortType() const { return WritePort; }

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
            return !mconn && other->addWriter( this );
        }

        void disconnect() {
            mconn = 0;
        }


        /**
         * Clears all contents of this buffer.
         */
        void clear() {
            if (mconn)
                return mconn->write()->clear();
        }

        /**
         * Returns the actual number of items that are stored in the
         * buffer.
         * @return number of items.
         */
        BufferBase::size_type size() const {
            if (mconn)
                return mconn->write()->size();
            return 0;
        }

        /**
         * Returns the maximum number of items that can be stored in the
         * buffer.
         * @return maximum number of items.
         */
        BufferBase::size_type capacity() const {
            if (mconn)
                return mconn->write()->capacity();
            return 0;
        }
            
        /**
         * Check if this buffer is empty.
         * @return true if size() == 0
         */
        bool empty() const {
            if (mconn)
                return mconn->write()->empty();
            return 0;
        }
            
        /**
         * Check if this buffer is full.
         * @return true if size() == capacity()
         */
        bool full() const {
            if (mconn)
                return mconn->write()->full();
            return 0;
        }

        virtual PortInterface* clone() const {
            return new WriteBufferPort<T>( this->getName(), buf_size, minitial_value );
        }

        virtual PortInterface* antiClone() const {
            return new ReadBufferPort<T>( this->getName() );
        }

        virtual OperationInterface* createPortObject() {
#ifndef ORO_EMBEDDED
            TaskObject* to = new TaskObject( this->getName() );
            to->methods()->addMethod(method("Push", &WriteBufferPort<T>::Push, this),
                                     "Push a single value in the Buffer. Returns false if full().",
                                     "Val", "The value.");
            to->methods()->addMethod(method("size", &WriteBufferPort<T>::size, this),
                                     "Get the used size of the buffer.");
            to->methods()->addMethod(method("capacity", &WriteBufferPort<T>::capacity, this),
                                     "Get the capacity of the buffer.");
            to->methods()->addMethod(method("empty", &WriteBufferPort<T>::empty, this),
                                     "Inspect if the buffer is empty.");
            to->methods()->addMethod(method("full", &WriteBufferPort<T>::full, this),
                                     "Inspect if the buffer is full.");
            to->methods()->addMethod(method("clear", &WriteBufferPort<T>::clear, this),
                                     "Clear the contents of the buffer.");
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
        typedef PortInterface::PortType PortType;

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
        virtual BufferInterface<T>* buffer() const { return mconn ? mconn->buffer() : 0; }

        virtual ConnectionInterface::shared_ptr connection() const { return mconn; }

        virtual PortType getPortType() const { return PortInterface::ReadWritePort; }

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

        /**
         * Clears all contents of this buffer.
         */
        void clear() {
            if (mconn)
                return mconn->buffer()->clear();
        }

        /**
         * Returns the actual number of items that are stored in the
         * buffer.
         * @return number of items.
         */
        BufferBase::size_type size() const {
            if (mconn)
                return mconn->buffer()->size();
            return 0;
        }

        /**
         * Returns the maximum number of items that can be stored in the
         * buffer.
         * @return maximum number of items.
         */
        BufferBase::size_type capacity() const {
            if (mconn)
                return mconn->buffer()->capacity();
            return 0;
        }
            
        /**
         * Check if this buffer is empty.
         * @return true if size() == 0
         */
        bool empty() const {
            if (mconn)
                return mconn->buffer()->empty();
            return 0;
        }
            
        /**
         * Check if this buffer is full.
         * @return true if size() == capacity()
         */
        bool full() const {
            if (mconn)
                return mconn->buffer()->full();
            return 0;
        }

        virtual PortInterface* clone() const {
            return new BufferPort<T>( this->getName(), this->buf_size, this->minitial_value );
        }

        virtual PortInterface* antiClone() const {
            return new BufferPort<T>( this->getName(), this->buf_size, this->minitial_value );
        }

        virtual OperationInterface* createPortObject() {
#ifndef ORO_EMBEDDED
            TaskObject* to = new TaskObject( this->getName() );
            to->methods()->addMethod(method("Push", &WriteBufferPort<T>::Push, this),
                                     "Push a single value in the Buffer. Returns false if full().",
                                     "Val", "The value.");
            to->methods()->addMethod(method("Pop", &ReadBufferPort<T>::Pop, this),
                                     "Pop a single value from the Buffer. Returns false if empty.",
                                     "Val", "The value returned by argument.");
            to->methods()->addMethod(method("front", &ReadBufferPort<T>::front, this),
                                     "Get the next to be popped value from the buffer. Returns default value if buffer is empty.");
            to->methods()->addMethod(method("size", &BufferPort<T>::size, this),
                                     "Get the used size of the buffer.");
            to->methods()->addMethod(method("capacity", &BufferPort<T>::capacity, this),
                                     "Get the capacity of the buffer.");
            to->methods()->addMethod(method("empty", &BufferPort<T>::empty, this),
                                     "Inspect if the buffer is empty.");
            to->methods()->addMethod(method("full", &BufferPort<T>::full, this),
                                     "Inspect if the buffer is full.");
            to->methods()->addMethod(method("clear", &BufferPort<T>::clear, this),
                                     "Clear the contents of the buffer.");
            return to;
#else
            return 0;
#endif
        }
    };
}

#include "ConnectionFactory.hpp"

namespace RTT
{

    template<class T>
    PortInterface* ReadBufferPort<T>::antiClone() const {
        return new WriteBufferPort<T>( this->getName(), 1 );
    }

    template<class T>
    ConnectionInterface::shared_ptr WriteBufferPort<T>::createConnection(PortInterface* other, ConnectionTypes::ConnectionType con_type )
        {
            ConnectionFactory<T> cf;
            return ConnectionInterface::shared_ptr (cf.createBuffer(this, other, buf_size, minitial_value, con_type));
        }
}
#endif
