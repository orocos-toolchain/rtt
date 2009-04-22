#ifndef ORO_EXECUTION_PORTS_HPP
#define ORO_EXECUTION_PORTS_HPP

#include "Channels.hpp"
#include "PortInterface.hpp"

#include "DataObjectInterfaces.hpp"
#include "BufferLocked.hpp"
#include "BufferLockFree.hpp"
#include "TaskObject.hpp"
#include "Method.hpp"

namespace RTT
{
    class TypeInfo;
    template<typename T> class OutputPort;
    template<typename T> class InputPort;

    template<typename T>
    class ChannelWriterEndpoint : public ChannelElement<T>
    {
        OutputPort<T>* port;

        /** Helper method for disconnect(bool) */
        bool eraseThisConnection(typename OutputPort<T>::ChannelDescriptor& connection)
        {
            if (connection.template get<1>() == this)
            {
                delete connection.template get<0>();
                return true;
            } else return false;
        }

    public:
        ChannelWriterEndpoint(OutputPort<T>* port)
            : port(port) { }

        /** Writes a new sample on this connection
         * This should never be called, as all connections are supposed to have
         * a data storage element */
        virtual bool read(typename ChannelElement<T>::reference_t sample)
        { return false; }

        virtual void disconnect(bool writer_to_reader)
        {
            if (!writer_to_reader)
            {
                OutputPort<T>* port = this->port;
                if (!port)
                    return;

                port->connections.delete_if(boost::bind(
                            &ChannelWriterEndpoint<T>::eraseThisConnection, this, _1)
                        );
            }
            else
                ChannelElement<T>::disconnect(writer_to_reader);
        }
    };

    template<typename T>
    class ChannelReaderEndpoint : public ChannelElement<T>
    {
        InputPort<T>* port;

    public:
        ChannelReaderEndpoint(InputPort<T>* port)
            : port(port)
        {
            port->writer = this;
        }

        /** Writes a new sample on this connection
         * This should never be called, as all connections are supposed to have
         * a data storage element */
        virtual bool write(typename ChannelElement<T>::param_t sample)
        { return false; }

        virtual void disconnect(bool writer_to_reader)
        {
            if (writer_to_reader)
            {
                InputPort<T>* port = this->port;
                this->port = 0;
                if (port)
                    port->writer = 0;
            }
            else
                ChannelElement<T>::disconnect(writer_to_reader);
        }

        virtual bool signal() const
        {
            InputPort<T>* port = this->port;
            if (port && port->new_data_on_port_event)
                (*port->new_data_on_port_event)(port);
            return true;
        }
    };

    class ChannelFactory
    {
    public:

        /** This method is analoguous to the static ChannelFactory::buildWriterHalf.
         * It is provided for remote connection building: for these connections,
         * no template can be used and therefore the connection setup should be
         * done based on the TypeInfo object
         */
        virtual ChannelElementBase* buildReaderHalf(TypeInfo const* type_info,
                InputPortInterface& reader, ConnPolicy const& policy) = 0;

        /** This method creates the connection element that will store data
         * inside the connection, based on the given policy
         */
        template<typename T>
        static ChannelElementBase* buildDataStorage(ConnPolicy const& policy)
        {
            if (policy.type == ConnPolicy::DATA)
            {
                DataObjectInterface<T>* data_object = 0;
                if (policy.lock_policy == ConnPolicy::LOCKED)
                    data_object = new DataObjectLocked<T>("");
                else
                    data_object = new DataObjectLockFree<T>("");

                ChannelDataElement<T>* result = new ChannelDataElement<T>(data_object);
                data_object->deref(); // data objects are initialized with a refcount of 1
                return result;
            }
            else if (policy.type == ConnPolicy::BUFFER)
            {
                BufferInterface<T>* buffer_object = 0;
                if (policy.lock_policy == ConnPolicy::LOCKED)
                    buffer_object = new BufferLocked<T>(policy.size);
                else
                    buffer_object = new BufferLockFree<T>(policy.size);

                return new ChannelBufferElement<T>(typename BufferInterface<T>::shared_ptr(buffer_object));
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
        static ChannelElementBase* buildWriterHalf(OutputPort<T>& writer, ConnPolicy const& policy, ChannelElementBase* sink)
        {
            ChannelElementBase* endpoint = new ChannelWriterEndpoint<T>(&writer);
            if (policy.pull)
            {
                ChannelElementBase* data_object = buildDataStorage<T>(policy);
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
        static ChannelElementBase* buildReaderHalf(InputPort<T>& reader, ConnPolicy const& policy)
        {
            ChannelElementBase* endpoint = new ChannelReaderEndpoint<T>(&reader);
            if (!policy.pull)
            {
                ChannelElementBase* data_object = buildDataStorage<T>(policy);
                data_object->setReader(endpoint);
                return data_object;
            }
            else return endpoint;
        }
    };

    /** This class represents a read port using the data source interface.
     * Beware that, depending on the connection used, ports actually change
     * their state when read. For instance, a buffer connection *will* lose one
     * element when get() or evaluate() are called. For that reason, it is
     * considered bad practice to use a data source on a read port that is bound
     * to a buffer connection.
     *
     * This class should not be used directly in normal code. What you would
     * usually do is create a new read port using OutputPort::antiClone() and
     * call InputPortInterface::getDataSource() to get the corresponding data
     * source.  This is your duty to destroy the port when it is not needed
     * anymore.
     */
    template<typename T>
    class InputPortSource : public DataSource<T>
    {
        InputPort<T>& reader;

    public:
        InputPortSource(InputPort<T>& port)
            : reader(port) { }

        void reset() { reader.clear(); }
        bool evaluate() const
        {
            T result;
            return reader.read(result);
        }

        typename DataSource<T>::result_t value() const
        { return get(); }
        typename DataSource<T>::result_t get() const
        {
            T result;
            if (reader.read(result))
                return result;
            else return T();
        }
        DataSource<T>* clone() const
        { return new InputPortSource<T>(reader); }
        DataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
        { return const_cast<InputPortSource<T>*>(this); }
    };

    template<typename T>
    class InputPort : public InputPortInterface
    {
        friend class ChannelReaderEndpoint<T>;
        InputPortSource<T>* data_source;

    public:
        InputPort(std::string const& name, ConnPolicy const& default_policy = ConnPolicy())
            : InputPortInterface(name, default_policy)
            , data_source(0) {}

        ~InputPort() { delete data_source; }

        /** Reads a sample from the connection. \a sample is a reference which
         * will get updated if a sample is available. The method returns true
         * if a sample was available, and false otherwise. If false is returned,
         * then \a sample is not modified by the method
         */
        bool read(typename ChannelElement<T>::reference_t sample)
        {
            typename ChannelElement<T>::shared_ptr writer = static_cast< ChannelElement<T>* >(this->writer);
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
        { return new InputPort<T>(this->getName()); }

        /**
         * Create the anti-clone (inverse port) of this port with the same name
         * A port for reading will return a new port for writing and
         * vice versa.
         */
        virtual PortInterface* antiClone() const
        { return new OutputPort<T>(this->getName()); }

        /** Returns a DataSourceBase interface to read this port. The returned
         * data source is always the same object and will be destroyed when the
         * port is destroyed.
         */
        DataSourceBase* getDataSource()
        {
            if (data_source) return data_source;
            data_source = new InputPortSource<T>(*this);
            data_source->ref();
            return data_source;
        }

        /**
         * Create accessor Object for this Port, for addition to a
         * TaskContext Object interface.
         */
        virtual TaskObject* createPortObject()
        {
            TaskObject* object = InputPortInterface::createPortObject();
            object->methods()->addMethod( method("read", &InputPort<T>::read, this),
                    "Reads a sample from the port.",
                    "sample", "");
            return object;
        }
    };
    template<typename T>
    class OutputPort : public OutputPortInterface
    {
        friend class ChannelWriterEndpoint<T>;

        bool written;
        typename DataObjectInterface<T>::shared_ptr last_written_value;

        bool do_write(typename ChannelElement<T>::param_t sample, ChannelDescriptor descriptor)
        {
            typename ChannelElement<T>::shared_ptr reader
                = boost::static_pointer_cast< ChannelElement<T> >(descriptor.get<1>());
            if (reader->write(sample))
                return false;
            else return true;
        }

    public:
        OutputPort(std::string const& name, bool keep_last_written_value = false)
            : OutputPortInterface(name)
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
                {
                    last_written_value = new DataObjectLockFree<T>(getName() + "Last");
                    last_written_value->deref(); // Data objects are initialized with a refcount of 1
                }
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

        void write(typename ChannelElement<T>::param_t sample)
        {
            typename DataObjectInterface<T>::shared_ptr last_written_value = this->last_written_value;
            if (last_written_value)
                last_written_value->Set(sample);
            written = true;

            connections.delete_if( boost::bind(
                        &OutputPort<T>::do_write, this, boost::ref(sample), _1)
                    );
        }

        /** Returns the TypeInfo object for the port's type */
        virtual const TypeInfo* getTypeInfo() const
        { return detail::DataSourceTypeInfo<T>::getTypeInfo(); }

        /**
         * Create a clone of this port with the same name
         */
        virtual PortInterface* clone() const
        { return new OutputPort<T>(this->getName()); }

        /**
         * Create the anti-clone (inverse port) of this port with the same name
         * A port for reading will return a new port for writing and
         * vice versa.
         */
        virtual PortInterface* antiClone() const
        { return new InputPort<T>(this->getName()); }

        using OutputPortInterface::createConnection;

        /** Channelects this write port to the given read port, using the given
         * policy */
        virtual bool createConnection( InputPortInterface& reader_, ConnPolicy const& policy )
        {
            ChannelElementBase* reader_endpoint = 0;
            if (reader_.isLocal())
            {
                InputPort<T>* reader = dynamic_cast<InputPort<T>*>(&reader_);
                if (!reader)
                {
                    log(Error) << "port " << reader_.getName() << " is not of the wrong specialization" << endlog();
                    return false;
                }

                reader_endpoint = ChannelFactory::buildReaderHalf(*reader, policy);
            }
            else
            {
                TypeInfo const* type_info = getTypeInfo();
                if (!type_info || reader_.getTypeInfo() != type_info)
                {
                    log(Error) << "type of port " << getName() << " is not registered into the type system, cannot marshal it into the right transporter" << endlog();
                    // There is no type info registered for this type
                    return false;
                }
                else if (!type_info->getProtocol(reader_.serverProtocol()))
                {
                    log(Error) << "type " << type_info->getTypeName() << " cannot be marshalled into the right transporter" << endlog();
                    // This type cannot be marshalled into the right transporter
                    return false;
                }
                else
                {
                    reader_endpoint = reader_.
                        getConnFactory()->buildReaderHalf(type_info, reader_, policy);
                }
            }

            typename ChannelElement<T>::shared_ptr writer_endpoint =
                static_cast< ChannelElement<T>* >(ChannelFactory::buildWriterHalf(*this, policy, reader_endpoint));
            addConnection( boost::make_tuple(reader_.getPortID(), writer_endpoint, policy) );

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
            TaskObject* object = OutputPortInterface::createPortObject();
            object->methods()->addMethod( method("write", &OutputPort<T>::write, this),
                    "Writes a sample on the port.",
                    "sample", "");
            return object;
        }
    };

}

#endif

