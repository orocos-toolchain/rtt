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

    /** This is a channel element that represents the input endpoint of a
     * connection, i.e. the part that is connected to the OutputPort
     */
    template<typename T>
    class ConnInputEndpoint : public ChannelElement<T>
    {
        OutputPort<T>* port;

    public:
        ConnInputEndpoint(OutputPort<T>* port)
            : port(port) { }

        /** Writes a new sample on this connection
         * This should never be called, as all connections are supposed to have
         * a data storage element */
        virtual bool read(typename ChannelElement<T>::reference_t sample)
        { return false; }

        virtual void disconnect(bool forward)
        {
            if (forward)
            {
                this->port = NULL;
                ChannelElement<T>::disconnect(true);
            }
            else
            {
                OutputPort<T>* port = this->port;
                if (!port)
                    return;

                port->removeConnection(this);
            }
        }
    };

    /** This is a channel element that represents the output endpoint of a
     * connection, i.e. the part that is connected to the InputPort
     */
    template<typename T>
    class ConnOutputEndpoint : public ChannelElement<T>
    {
        InputPort<T>* port;

    public:
        ConnOutputEndpoint(InputPort<T>* port)
            : port(port)
        {
            port->setInputChannel(this);
        }

        /** Writes a new sample on this connection
         * This should never be called, as all connections are supposed to have
         * a data storage element */
        virtual bool write(typename ChannelElement<T>::param_t sample)
        { return false; }

        virtual void disconnect(bool forward)
        {
            InputPort<T>* port = this->port;
            this->port = 0;

            if (forward)
            {
                if (port)
                    port->clearInputChannel();
            }
            else
                ChannelElement<T>::disconnect(false);
        }

        virtual bool signal() const
        {
            InputPort<T>* port = this->port;
            if (port && port->new_data_on_port_event)
                (*port->new_data_on_port_event)(port);
            return true;
        }
    };

    /** This class provides the basic tools to create channels that represent
     * connections between two ports
     */
    class ConnFactory
    {
    public:

        /** This method is analoguous to the static ConnFactory::buildOutputHalf.
         * It is provided for remote connection building: for these connections,
         * no template can be used and therefore the connection setup should be
         * done based on the TypeInfo object
         */
        virtual ChannelElementBase* buildOutputHalf(TypeInfo const* type_info,
                InputPortInterface& output, ConnPolicy const& policy) = 0;

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
         * method builds the input half (starting from the OutputPort). In the
         * returned pair, the first element is the connection element that
         * should be connected to the port, and the second element is the one
         * that will be connected to the output-half of the connection
         *
         * The \c output_channel argument is the connection element that has been returned
         * by buildOutputHalf.
         *
         * @see buildOutputHalf
         */
        template<typename T>
        static ChannelElementBase* buildInputHalf(OutputPort<T>& port, ConnPolicy const& policy, ChannelElementBase* output_channel)
        {
            ChannelElementBase* endpoint = new ConnInputEndpoint<T>(&port);
            if (policy.pull)
            {
                ChannelElementBase* data_object = buildDataStorage<T>(policy);
                endpoint->setOutput(data_object);
                data_object->setOutput(output_channel);
            }
            else
                endpoint->setOutput(output_channel);

            return endpoint;
        }

        /** During the process of building a connection between two ports, this
         * method builds the output part of the channel, that is the half that
         * is connected to the input port. The returned value is the connection
         * element that should be connected to the end of the input-half.
         *
         * @see buildInputHalf
         */
        template<typename T>
        static ChannelElementBase* buildOutputHalf(InputPort<T>& port, ConnPolicy const& policy)
        {
            ChannelElementBase* endpoint = new ConnOutputEndpoint<T>(&port);
            if (!policy.pull)
            {
                ChannelElementBase* data_object = buildDataStorage<T>(policy);
                data_object->setOutput(endpoint);
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
        InputPort<T>& port;

    public:
        InputPortSource(InputPort<T>& port)
            : port(port) { }

        void reset() { port.clear(); }
        bool evaluate() const
        {
            T result;
            return port.read(result);
        }

        typename DataSource<T>::result_t value() const
        { return get(); }
        typename DataSource<T>::result_t get() const
        {
            T result;
            if (port.read(result))
                return result;
            else return T();
        }
        DataSource<T>* clone() const
        { return new InputPortSource<T>(port); }
        DataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
        { return const_cast<InputPortSource<T>*>(this); }
    };

    template<typename T>
    class InputPort : public InputPortInterface
    {
        friend class ConnOutputEndpoint<T>;
        InputPortSource<T>* data_source;

    public:
        InputPort(std::string const& name, ConnPolicy const& default_policy = ConnPolicy())
            : InputPortInterface(name, default_policy)
            , data_source(0) {}

        ~InputPort() { delete data_source; }

        bool read(DataSourceBase::shared_ptr source)
        {
            typename AssignableDataSource<T>::shared_ptr ds =
                boost::dynamic_pointer_cast< AssignableDataSource<T> >(source);
            if (! ds)
            {
                log(Error) << "trying to read to an incompatible data source" << endlog();
                return false;
            }
            return read(ds->set());
        }

        /** Reads a sample from the connection. \a sample is a reference which
         * will get updated if a sample is available. The method returns true
         * if a sample was available, and false otherwise. If false is returned,
         * then \a sample is not modified by the method
         */
        bool read(typename ChannelElement<T>::reference_t sample)
        {
            typename ChannelElement<T>::shared_ptr input = static_cast< ChannelElement<T>* >(this->channel);
            if (input)
                return input->read(sample);
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
            // Force resolution on the overloaded write method
            typedef bool (InputPort<T>::*ReadSample)(typename ChannelElement<T>::reference_t);
            object->methods()->addMethod( method("read", static_cast<ReadSample>(&InputPort<T>::read), this),
                    "Reads a sample from the port.",
                    "sample", "");
            return object;
        }
    };
    template<typename T>
    class OutputPort : public OutputPortInterface
    {
        friend class ConnInputEndpoint<T>;

        bool written;
        typename DataObjectInterface<T>::shared_ptr last_written_value;

        bool do_write(typename ChannelElement<T>::param_t sample, ChannelDescriptor descriptor)
        {
            typename ChannelElement<T>::shared_ptr output
                = boost::static_pointer_cast< ChannelElement<T> >(descriptor.get<1>());
            if (output->write(sample))
                return false;
            else
            {
                log(Error) << "a channel of " << getName() << " has been invalidated during write(), it will be removed" << endlog();
                return true;
            }
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

        void write(DataSourceBase::shared_ptr source)
        {
            typename AssignableDataSource<T>::shared_ptr ds =
                boost::dynamic_pointer_cast< AssignableDataSource<T> >(source);
            if (ds)
                write(ds->rvalue());
            else
            {
                typename DataSource<T>::shared_ptr ds =
                    boost::dynamic_pointer_cast< DataSource<T> >(source);
                if (ds)
                    write(ds->get());
                else
                    log(Error) << "trying to write from an incompatible data source" << endlog();
            }
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
        virtual bool createConnection(InputPortInterface& input_port, ConnPolicy const& policy)
        {
            // This is the input channel element of the output half
            ChannelElementBase* output_half = 0;
            if (input_port.isLocal())
            {
                InputPort<T>* input_p = dynamic_cast<InputPort<T>*>(&input_port);
                if (!input_p)
                {
                    log(Error) << "port " << input_port.getName() << " is not compatible with " << getName() << endlog();
                    return false;
                }

                output_half = ConnFactory::buildOutputHalf(*input_p, policy);
            }
            else
            {
                TypeInfo const* type_info = getTypeInfo();
                if (!type_info || input_port.getTypeInfo() != type_info)
                {
                    log(Error) << "type of port " << getName() << " is not registered into the type system, cannot marshal it into the right transporter" << endlog();
                    // There is no type info registered for this type
                    return false;
                }
                else if (!type_info->getProtocol(input_port.serverProtocol()))
                {
                    log(Error) << "type " << type_info->getTypeName() << " cannot be marshalled into the right transporter" << endlog();
                    // This type cannot be marshalled into the right transporter
                    return false;
                }
                else
                {
                    output_half = input_port.
                        getConnFactory()->buildOutputHalf(type_info, input_port, policy);
                }
            }

            if (!output_half)
                return false;

            // This this the input channel element of the whole connection
            typename ChannelElement<T>::shared_ptr channel_input =
                static_cast< ChannelElement<T>* >(ConnFactory::buildInputHalf(*this, policy, output_half));

            // Register the channel's input 
            addConnection( input_port.getPortID(), channel_input, policy );
            return true;
        }

        void addConnection(RTT::PortID* port_id, typename ChannelElement<T>::shared_ptr channel_input, ConnPolicy const& policy)
        {
            OutputPortInterface::addConnection(port_id, channel_input, policy);

            // Initialize the new channel with last written data if requested
            // (and available)
            if (policy.init && written)
            {
                typename DataObjectInterface<T>::shared_ptr last_written_value = this->last_written_value;
                if (last_written_value)
                {
                    T sample;
                    last_written_value->Get(sample);
                    channel_input->write(sample);
                }
            }
        }

        /**
         * Create accessor Object for this Port, for addition to a
         * TaskContext Object interface.
         */
        virtual TaskObject* createPortObject()
        {
            TaskObject* object = OutputPortInterface::createPortObject();
            // Force resolution on the overloaded write method
            typedef void (OutputPort<T>::*WriteSample)(typename ChannelElement<T>::param_t);
            object->methods()->addMethod(
                    method("write", static_cast<WriteSample>(&OutputPort::write), this),
                    "Writes a sample on the port.",
                    "sample", "");
            return object;
        }
    };

}

#endif

