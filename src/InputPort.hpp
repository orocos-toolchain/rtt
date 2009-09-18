#ifndef ORO_INPUT_PORT_HPP
#define ORO_INPUT_PORT_HPP

#include "base/InputPortInterface.hpp"
#include "internal/Channels.hpp"
#include "internal/InputPortSource.hpp"
#include "internal/TaskObject.hpp"
#include "Method.hpp"

#include "OutputPort.hpp"

namespace RTT
{
    /**
     * A component's data input port. An Orocos input port is used to receive
     * data samples from a distant publisher. The InputPort is read() and returns
     * true if a sample is available.
     *
     * Ideally, your algorithm should not assume a certain connection policy
     * being used from output to input. So it should work on data connections
     * and buffer connections.
     */
    template<typename T>
    class InputPort : public base::InputPortInterface
    {
        friend class internal::ConnOutputEndpoint<T>;
        internal::InputPortSource<T>* data_source;

    public:
        InputPort(std::string const& name, internal::ConnPolicy const& default_policy = internal::ConnPolicy())
            : base::InputPortInterface(name, default_policy)
            , data_source(0) {}

        ~InputPort() { delete data_source; }

        FlowStatus read(base::DataSourceBase::shared_ptr source)
        {
            typename internal::AssignableDataSource<T>::shared_ptr ds =
                boost::dynamic_pointer_cast< internal::AssignableDataSource<T> >(source);
            if (! ds)
            {
                log(Error) << "trying to read to an incompatible data source" << endlog();
                return NoData;
            }
            return read(ds->set());
        }

        /** Reads a sample from the connection. \a sample is a reference which
         * will get updated if a sample is available. The method returns true
         * if a sample was available, and false otherwise. If false is returned,
         * then \a sample is not modified by the method
         */
        FlowStatus read(typename base::ChannelElement<T>::reference_t sample)
        {
            typename base::ChannelElement<T>::shared_ptr input = static_cast< base::ChannelElement<T>* >(this->channel);
            if (input)
                return input->read(sample);
            else
                return NoData;
        }

        /** Returns the types::TypeInfo object for the port's type */
        virtual const types::TypeInfo* getTypeInfo() const
        { return internal::DataSourceTypeInfo<T>::getTypeInfo(); }

        /**
         * Create a clone of this port with the same name
         */
        virtual base::PortInterface* clone() const
        { return new InputPort<T>(this->getName()); }

        /**
         * Create the anti-clone (inverse port) of this port with the same name
         * A port for reading will return a new port for writing and
         * vice versa.
         */
        virtual base::PortInterface* antiClone() const
        { return new OutputPort<T>(this->getName()); }

        /** Returns a base::DataSourceBase interface to read this port. The returned
         * data source is always the same object and will be destroyed when the
         * port is destroyed.
         */
        base::DataSourceBase* getDataSource()
        {
            if (data_source) return data_source;
            data_source = new internal::InputPortSource<T>(*this);
            data_source->ref();
            return data_source;
        }

        virtual bool createStream(internal::ConnPolicy const& policy)
        {
            return internal::ConnFactory::createStream(*this, policy);
        }

        /**
         * Create accessor Object for this Port, for addition to a
         * TaskContext Object interface.
         */
        virtual internal::TaskObject* createPortObject()
        {
            internal::TaskObject* object = base::InputPortInterface::createPortObject();
            // Force resolution on the overloaded write method
            typedef FlowStatus (InputPort<T>::*ReadSample)(typename base::ChannelElement<T>::reference_t);
            ReadSample read_m = &InputPort<T>::read;
            object->methods()->addMethod( method("read", read_m, this),
                    "Reads a sample from the port.",
                    "sample", "");
            return object;
        }
    };
}

#endif

