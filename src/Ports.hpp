#ifndef ORO_EXECUTION_PORTS_HPP
#define ORO_EXECUTION_PORTS_HPP

#include <Connections.hpp>
#include <PortInterface.hpp>

#include <DataObjectInterfaces.hpp>
#include <BufferLocked.hpp>
#include <BufferLockFree.hpp>
#include <TaskObject.hpp>

namespace RTT
{
    class TypeInfo;
    template<typename T> class WritePort;
    template<typename T> class ReadPort;

    template<typename T>
    class ConnWriterEndpoint : public ConnElement<T>
    {
        WritePort<T>* port;

    public:
        ConnWriterEndpoint(WritePort<T>* port)
            : port(port) { }

        /** Writes a new sample on this connection
         * This should never be called, as all connections are supposed to have
         * a data storage element */
        virtual bool read(typename ConnElement<T>::reference_t sample)
        { return false; }

        virtual void disconnect(bool writer_to_reader)
        {
            if (!writer_to_reader)
            {
                WritePort<T>* port = this->port;
                if (!port)
                    return;

                for (std::list<WritePortInterface::ConnDescriptor>::iterator it =
                        port->connections.begin();
                        it != port->connections.end();
                        ++it)
                {
                    if (it->get<1>() == this)
                    {
                        port->connections.erase(it);
                        return;
                    }
                }
            }
            else
                ConnElement<T>::disconnect(writer_to_reader);
        }
    };

    template<typename T>
    class ConnReaderEndpoint : public ConnElement<T>
    {
        ReadPort<T>* port;

    public:
        ConnReaderEndpoint(ReadPort<T>* port)
            : port(port)
        {
            port->writer = this;
        }

        /** Writes a new sample on this connection
         * This should never be called, as all connections are supposed to have
         * a data storage element */
        virtual bool write(typename ConnElement<T>::param_t sample)
        { return false; }

        virtual void disconnect(bool writer_to_reader)
        {
            if (writer_to_reader)
            {
                ReadPort<T>* port = this->port;
                this->port = 0;
                if (port)
                    port->writer = 0;
            }
            else
                ConnElement<T>::disconnect(writer_to_reader);
        }

        virtual void signal() const
        {
            ReadPort<T>* port = this->port;
            if (port && port->new_data_on_port_event)
                (*port->new_data_on_port_event)(port);
        }
    };

    class ConnFactory
    {
    public:

        /** This method is analoguous to the static ConnFactory::buildWriterHalf.
         * It is provided for remote connection building: for these connections,
         * no template can be used and therefore the connection setup should be
         * done based on the TypeInfo object
         */
        virtual ConnElementBase* buildReaderHalf(TypeInfo const* type_info,
                PortInterface& writer, PortInterface& reader, ConnPolicy const& policy) = 0;

        /** This method creates the connection element that will store data
         * inside the connection, based on the given policy
         */
        template<typename T>
        static ConnElementBase* buildDataStorage(ConnPolicy const& policy)
        {
            if (policy.type == ConnPolicy::DATA)
            {
                DataObjectInterface<T>* data_object = 0;
                if (policy.lock_policy == ConnPolicy::LOCKED)
                    data_object = new DataObjectLocked<T>("");
                else
                    data_object = new DataObjectLockFree<T>("");

                return new ConnDataElement<T>(data_object);
            }
            else if (policy.type == ConnPolicy::BUFFER)
            {
                BufferInterface<T>* buffer_object = 0;
                if (policy.lock_policy == ConnPolicy::LOCKED)
                    buffer_object = new BufferLocked<T>(policy.size);
                else
                    buffer_object = new BufferLockFree<T>(policy.size);

                return new ConnBufferElement<T>(typename BufferInterface<T>::shared_ptr(buffer_object));
            }
            return NULL;
        }

        /** During the process of building a connection between two ports, this
         * method builds the first half (starting from the source). In the
         * returned pair, the first element is the connection element that
         * should be connected to the port, and the second element is the one
         * that will be connected to the sink-half of the connection
         *
         * The \c sink argument is the connection element that has been returned
         * by buildWriterHalf.
         *
         * @see buildWriterHalf
         */
        template<typename T>
        static ConnElementBase* buildWriterHalf(WritePort<T>& writer, ReadPort<T>& reader,
                ConnPolicy const& policy, ConnElementBase* sink)
        {
            ConnElementBase* endpoint = new ConnWriterEndpoint<T>(&writer);
            if (policy.pull)
            {
                ConnElementBase* data_object = buildDataStorage<T>(policy);
                endpoint->setReader(data_object);
                data_object->setReader(sink);
            }
            else
                endpoint->setReader(sink);

            return endpoint;
        }

        /** During the process of building a connection between two ports, this
         * method builds the second half (starting from the source). The
         * returned value is the connection element that should be connected to
         * the end of the source-half.
         *
         * @see buildSourceHalf
         */
        template<typename T>
        static ConnElementBase* buildReaderHalf(WritePort<T>& writer, ReadPort<T>& reader, 
                ConnPolicy const& policy)
        {
            ConnElementBase* endpoint = new ConnReaderEndpoint<T>(&reader);
            if (!policy.pull)
            {
                ConnElementBase* data_object = buildDataStorage<T>(policy);
                data_object->setReader(endpoint);
                return data_object;
            }
            else return endpoint;
        }
    };

    template<typename T>
    class ReadPort : public ReadPortInterface
    {
        friend class ConnReaderEndpoint<T>;

    public:
        ReadPort(std::string const& name, ConnPolicy const& default_policy = ConnPolicy())
            : ReadPortInterface(name, default_policy) {}

        bool read(typename ConnElement<T>::reference_t sample)
        {
            typename ConnElement<T>::shared_ptr writer = static_cast< ConnElement<T>* >(this->writer);
            if (writer)
                return writer->read(sample);
            else return false;
        }

        /** Returns the TypeInfo object for the port's type */
        virtual const TypeInfo* getTypeInfo() const
        { return detail::DataSourceTypeInfo<T>::getTypeInfo(); }

        /**
         * Create a clone of this port with the same name
         */
        virtual PortInterface* clone() const
        { return new ReadPort<T>(this->getName()); }

        /**
         * Create the anti-clone (inverse port) of this port with the same name
         * A port for reading will return a new port for writing and
         * vice versa.
         */
        virtual PortInterface* antiClone() const
        { return new WritePort<T>(this->getName()); }

        /**
         * Create accessor Object for this Port, for addition to a
         * TaskContext Object interface.
         */
        virtual TaskObject* createPortObject()
        {
            TaskObject* object = ReadPortInterface::createPortObject();
            object->methods()->addMethod( method("read", &ReadPort<T>::read, this),
                    "Reads a sample from the port.",
                    "sample", "");
            return object;
        }
    };

    template<typename T>
    class WritePort : public WritePortInterface
    {
        friend class ConnWriterEndpoint<T>;

        bool written;
        typename DataObjectInterface<T>::shared_ptr last_written_value;

    public:
        WritePort(std::string const& name, bool keep_last_written_value = false)
            : WritePortInterface(name)
            , written(false)
        {
            if (keep_last_written_value)
                keepLastWrittenValue(true);
        }

        bool keepsLastWrittenValue() const { return last_written_value; }
        void keepLastWrittenValue(bool new_flag)
        {
            if (new_flag)
            {
                if (!last_written_value)
                    last_written_value = new DataObjectLockFree<T>(getName() + "Last");
            }
            else
                last_written_value = 0;
        }
        T getLastWrittenValue() const
        {
            typename DataObjectInterface<T>::shared_ptr last_written_value = this->last_written_value;
            if (written && last_written_value)
                return last_written_value->Get();
            else return T();
        }
        bool getLastWrittenValue(T& sample) const
        {
            typename DataObjectInterface<T>::shared_ptr last_written_value = this->last_written_value;
            if (written && last_written_value)
            {
                sample = last_written_value->Get();
                return true;
            }
            return false;
        }

        bool write(typename ConnElement<T>::param_t sample)
        {
            typename DataObjectInterface<T>::shared_ptr last_written_value = this->last_written_value;
            if (last_written_value)
                last_written_value->Set(sample);
            written = true;

            bool result = false;
            for (std::list<ConnDescriptor>::iterator it = connections.begin();
                    it != connections.end(); ++it)
            {
                typename ConnElement<T>::shared_ptr reader = boost::static_pointer_cast< ConnElement<T> >(it->get<1>());
                if (reader->write(sample))
                    result = true;
            }

            return result;
        }

        /** Returns the TypeInfo object for the port's type */
        virtual const TypeInfo* getTypeInfo() const
        { return detail::DataSourceTypeInfo<T>::getTypeInfo(); }

        /**
         * Create a clone of this port with the same name
         */
        virtual PortInterface* clone() const
        { return new WritePort<T>(this->getName()); }

        /**
         * Create the anti-clone (inverse port) of this port with the same name
         * A port for reading will return a new port for writing and
         * vice versa.
         */
        virtual PortInterface* antiClone() const
        { return new ReadPort<T>(this->getName()); }

        using WritePortInterface::createConnection;

        /** Connects this write port to the given read port, using the given
         * policy */
        virtual bool createConnection( ReadPortInterface& reader_, ConnPolicy const& policy )
        {
            ReadPort<T>* reader = dynamic_cast<ReadPort<T>*>(&reader_);
            if (!reader)
                return false;

            ConnElementBase* reader_endpoint;
            if (reader->isLocal())
                reader_endpoint = ConnFactory::buildReaderHalf(*this, *reader, policy);
            else
            {
                TypeInfo const* type_info = getTypeInfo();
                if (!type_info->getProtocol(reader->serverProtocol()))
                {
                    // This type cannot be marshalled into the right transporter
                    return false;
                }
                else if (type_info)
                {
                    reader_endpoint = reader->
                        getConnFactory()->buildReaderHalf(type_info, *this, *reader, policy);
                }
                else
                {
                    // There is no type info registered for this type
                    return false;
                }
            }

            typename ConnElement<T>::shared_ptr writer_endpoint =
                static_cast< ConnElement<T>* >(ConnFactory::buildWriterHalf(*this, *reader, policy, reader_endpoint));
            connections.push_back( boost::make_tuple(reader, writer_endpoint, policy) );

            if (policy.init && written)
            {
                typename DataObjectInterface<T>::shared_ptr last_written_value = this->last_written_value;
                if (last_written_value)
                {
                    T sample;
                    last_written_value->Get(sample);
                    writer_endpoint->write(sample);
                }
            }
            return true;
        }

        /**
         * Create accessor Object for this Port, for addition to a
         * TaskContext Object interface.
         */
        virtual TaskObject* createPortObject()
        {
            TaskObject* object = WritePortInterface::createPortObject();
            object->methods()->addMethod( method("write", &WritePort<T>::write, this),
                    "Writes a sample on the port.",
                    "sample", "");
            return object;
        }
    };

}

#endif

