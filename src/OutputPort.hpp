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
    /**
     * A component's data output port. An Orocos OutputPort is a send-and-forget
     * mechanism. The publisher writes data samples into the OutputPort and the
     * underlying middleware will communicate it to all subscribers. An
     * output port without subscribers is not an error on the component level (it may be at the system
     * level, which can inspect the status with calling connected() ).
     *
     * The data written into an OutputPort should be copyable and should provide
     * a copy constructor in case it's not plain old data. If you want the RTT
     * to transport your data over the network, or use it in scripting, you need
     * to register your data class with the RTT type system.
     *
     * @see RTT::types::TemplateTypeInfo for adding custom data classes to the RTT.
     */
    template<typename T>
    class OutputPort : public base::OutputPortInterface
    {
        friend class internal::ConnInputEndpoint<T>;

        bool written;
        typename base::DataObjectInterface<T>::shared_ptr last_written_value;

        bool do_write(typename base::ChannelElement<T>::param_t sample, const internal::ConnectionManager::ChannelDescriptor& descriptor)
        {
            typename base::ChannelElement<T>::shared_ptr output
                = boost::static_pointer_cast< base::ChannelElement<T> >(descriptor.get<1>());
            if (output->write(sample))
                return false;
            else
            {
                log(Error) << "A channel of port " << getName() << " has been invalidated during write(), it will be removed" << endlog();
                return true;
            }
        }

        bool do_init(typename base::ChannelElement<T>::param_t sample, const internal::ConnectionManager::ChannelDescriptor& descriptor)
        {
            typename base::ChannelElement<T>::shared_ptr output
                = boost::static_pointer_cast< base::ChannelElement<T> >(descriptor.get<1>());
            if (output->data_sample(sample))
                return false;
            else
            {
                log(Error) << "A channel of port " << getName() << " has been invalidated during setDataSample(), it will be removed" << endlog();
                return true;
            }
        }

        virtual bool connectionAdded( base::ChannelElementBase::shared_ptr channel_input, ConnPolicy const& policy ) {
            // Initialize the new channel with last written data if requested
            // (and available)

            // This this the input channel element of the whole connection
            typename base::ChannelElement<T>::shared_ptr channel_el_input =
                static_cast< base::ChannelElement<T>* >(channel_input.get());


            if (written)
            {
                typename base::DataObjectInterface<T>::shared_ptr last_written_value = this->last_written_value;
                if (last_written_value)
                {
                    T sample;
                    last_written_value->Get(sample);
                    if ( channel_el_input->data_sample(sample) ) {
                        if ( policy.init )
                            return channel_el_input->write(sample);
                        return true;
                    } else
                        return false;
                }
            }
            // even if we're not written, test the connection with a default sample.
            return channel_el_input->data_sample( T() );
        }

    public:
        /**
         * Creates a named Output port.
         * @param name The name of this port, unique among the ports of a TaskContext.
         * @param keep_last_written_value Defaults to \a true. You need keep_last_written_value == true
         * in two cases:
         * * You're sending dynamically sized objects through this port in real-time. In that case,
         * you need to write() to this port such an object before a connection is created. That object
         * will be used to allocate enough data storage in each there-after created connection. If you would
         * set keep_last_written_value == false in this use case, several memory allocations will happen
         * during the initial writes, after which non will happen anymore.
         * * You want to have an input to have the last written data available from before its connection
         * was created, such that it is immediately initialized.
         * The keep_last_written_value incurs a space overhead of one thread-safe data storage container.
         * This is about the same as an extra connection.
         *
         */
        OutputPort(std::string const& name, bool keep_last_written_value = true)
            : base::OutputPortInterface(name)
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
                    last_written_value = new base::DataObject<T>();
                    last_written_value->deref(); // Data objects are initialized with a refcount of 1
                }
            }
            else
                last_written_value = 0;
        }
        T getLastWrittenValue() const
        {
            typename base::DataObjectInterface<T>::shared_ptr last_written_value = this->last_written_value;
            if (written && last_written_value)
                return last_written_value->Get();
            else return T();
        }
        bool getLastWrittenValue(T& sample) const
        {
            typename base::DataObjectInterface<T>::shared_ptr last_written_value = this->last_written_value;
            if (written && last_written_value)
            {
                sample = last_written_value->Get();
                return true;
            }
            return false;
        }

        /**
         * Provides this port a data sample that is representative for the
         * samples being used in write(). The sample will not be delivered
         * to receivers, and only passed on to the underlying communication channel
         * to allow it to allocate enough memory to hold the sample. You
         * only need to call this in case you want to transfer dynamically
         * sized objects in real-time over this OutputPort.
         * @param sample
         */
        void setDataSample(const T& sample)
        {
            keepLastWrittenValue(true);
            typename base::DataObjectInterface<T>::shared_ptr last_written_value = this->last_written_value;
            if (last_written_value)
                last_written_value->Set(sample);
            written = true;

            cmanager.delete_if( boost::bind(
                        &OutputPort<T>::do_init, this, boost::ref(sample), _1)
                    );
        }

        /**
         * Writes a new sample to all receivers (if any).
         * @param sample The new sample to send out.
         */
        void write(const T& sample)
        {
            typename base::DataObjectInterface<T>::shared_ptr last_written_value = this->last_written_value;
            if (last_written_value)
                last_written_value->Set(sample);
            written = true;

            cmanager.delete_if( boost::bind(
                        &OutputPort<T>::do_write, this, boost::ref(sample), _1)
                    );
        }

        void write(base::DataSourceBase::shared_ptr source)
        {
            typename internal::AssignableDataSource<T>::shared_ptr ds =
                boost::dynamic_pointer_cast< internal::AssignableDataSource<T> >(source);
            if (ds)
                write(ds->rvalue());
            else
            {
                typename internal::DataSource<T>::shared_ptr ds =
                    boost::dynamic_pointer_cast< internal::DataSource<T> >(source);
                if (ds)
                    write(ds->get());
                else
                    log(Error) << "trying to write from an incompatible data source" << endlog();
            }
        }

        /** Returns the types::TypeInfo object for the port's type */
        virtual const types::TypeInfo* getTypeInfo() const
        { return internal::DataSourceTypeInfo<T>::getTypeInfo(); }

        /**
         * Create a clone of this port with the same name
         */
        virtual base::PortInterface* clone() const
        { return new OutputPort<T>(this->getName()); }

        /**
         * Create the anti-clone (inverse port) of this port with the same name
         * A port for reading will return a new port for writing and
         * vice versa.
         */
        virtual base::PortInterface* antiClone() const
        { return new InputPort<T>(this->getName()); }

        using base::OutputPortInterface::createConnection;

        /** Connects this write port to the given read port, using the given
         * policy */
        virtual bool createConnection(base::InputPortInterface& input_port, ConnPolicy const& policy)
        {
            return internal::ConnFactory::createConnection(*this, input_port, policy);
        }

        virtual bool createStream(ConnPolicy const& policy)
        {
            return internal::ConnFactory::createStream(*this, policy);
        }

        /**
         * Create accessor Object for this Port, for addition to a
         * TaskContext Object interface.
         */
        virtual internal::TaskObject* createPortObject()
        {
            internal::TaskObject* object = base::OutputPortInterface::createPortObject();
            // Force resolution on the overloaded write method
            typedef void (OutputPort<T>::*WriteSample)(T const&);
            WriteSample write_m = &OutputPort::write;
            object->methods()->addMethod(
                    method("write", write_m, this),
                    "Writes a sample on the port.",
                    "sample", "");
            return object;
        }
    };

}

#endif

