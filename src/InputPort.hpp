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
}

#endif

