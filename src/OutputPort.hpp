#ifndef ORO_OUTPUT_PORT_HPP
#define ORO_OUTPUT_PORT_HPP

#include "base/OutputPortInterface.hpp"
#include "base/DataObject.hpp"
#include "internal/Channels.hpp"
#include "internal/ConnFactory.hpp"
#include "internal/TaskObject.hpp"
#include "Method.hpp"

#include "InputPort.hpp"

namespace RTT
{
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
                    last_written_value = new DataObject<T>();
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

        /** Connects this write port to the given read port, using the given
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
            return true;
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

