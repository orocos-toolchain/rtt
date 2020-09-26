/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  InputPort.hpp

                        InputPort.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Sylvain Joyeux
    email                : sylvain.joyeux@m4x.org

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


#ifndef ORO_INPUT_PORT_HPP
#define ORO_INPUT_PORT_HPP

#include "rtt/base/InputPortInterface.hpp"
#include "rtt/internal/Channels.hpp"
#include "rtt/internal/InputPortSource.hpp"
#include "rtt/Service.hpp"
#include "rtt/OperationCaller.hpp"

#include "rtt/OutputPort.hpp"

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
     * @ingroup Ports
     */
    template<typename T>
    class InputPort : public base::InputPortInterface
    {
    private:
        friend class internal::ConnOutputEndpoint<T>;
        typename internal::ConnOutputEndpoint<T>::shared_ptr endpoint;

        virtual bool connectionAdded( base::ChannelElementBase::shared_ptr channel_input, ConnPolicy const& policy ) { return true; }

        /**
         * You are not allowed to copy ports.
         * In case you want to create a container of ports,
         * use pointers to ports instead of the port object
         * itself.
         */
        InputPort(InputPort const& orig);
        InputPort& operator=(InputPort const& orig);

    public:
        InputPort(std::string const& name = "unnamed", ConnPolicy const& default_policy = ConnPolicy())
            : base::InputPortInterface(name, default_policy)
            , endpoint(new internal::ConnOutputEndpoint<T>(this))
        {}

        virtual ~InputPort() { disconnect(); }

        /**
         * Clears the input buffer (or all input buffers), so that read() will return NoData before a new sample has been written.
         */
        void clear()
        {
            getEndpoint()->getReadEndpoint()->clear();
        }

        /** \overload */
        FlowStatus read(base::DataSourceBase::shared_ptr source)
        { return read(source, true); }

        FlowStatus read(base::DataSourceBase::shared_ptr source, bool copy_old_data)
        {
            typename internal::AssignableDataSource<T>::shared_ptr ds =
                boost::dynamic_pointer_cast< internal::AssignableDataSource<T> >(source);
            if (! ds)
            {
                log(Error) << "trying to read to an incompatible data source" << endlog();
                return NoData;
            }
            RTT::FlowStatus status = read(ds->set(), copy_old_data);
            traceRead(status);
            return status;
        }

        /** Read all new samples that are available on this port, and returns
         * the last one.
         *
         * Returns RTT::NewData if at least one new sample was available, and
         * either RTT::OldData or RTT::NoData otherwise.
         */
        FlowStatus readNewest(base::DataSourceBase::shared_ptr source, bool copy_old_data = true)
        {
            typename internal::AssignableDataSource<T>::shared_ptr ds =
                boost::dynamic_pointer_cast< internal::AssignableDataSource<T> >(source);
            if (! ds)
            {
                log(Error) << "trying to read to an incompatible data source" << endlog();
                return NoData;
            }
            return readNewest(ds->set(), copy_old_data);
        }

        /** \overload */
        FlowStatus read(typename base::ChannelElement<T>::reference_t sample)
        { return read(sample, true); }

        /** Reads a sample from the connection. \a sample is a reference which
         * will get updated if a new sample is available. 
         *
         * The method returns an enum FlowStatus, which describes what type of
         * sample (old or new data) or if a sample was returned (no data)
         *
         * With the argument @arg copy_old_data one can specify, if sample should
         * be updated in the case that the return type is equal to RTT::OldData.
         * In case @arg copy_old_data is false and an old sample is available, the
         * method will still return RTT::OldData but the sample will not be updated.
         */
        FlowStatus read(typename base::ChannelElement<T>::reference_t sample, bool copy_old_data)
        {
            return getEndpoint()->getReadEndpoint()->read(sample, copy_old_data);
        }

        /** Read all new samples that are available on this port, and returns
         * the last one.
         *
         * Returns RTT::NewData if at least one new sample was available, and
         * either RTT::OldData or RTT::NoData otherwise.
         */
        FlowStatus readNewest(typename base::ChannelElement<T>::reference_t sample, bool copy_old_data = true)
        {
            FlowStatus result = read(sample, copy_old_data);
            if (result != RTT::NewData)
                return result;

            while (read(sample, false) == RTT::NewData);
            return RTT::NewData;
        }

        /**
         * Get a sample of the data on this port, without actually reading the port's data.
         * It's the complement of OutputPort::setDataSample() and serves to retrieve the size
         * of a variable sized data type T. Returns default T if !connected() or if the 
         * OutputPort did not use setDataSample(). Returns an example T otherwise.
         * In case multiple inputs are connected to this port a sample from the currently read
         * connection will be returned.
         */
        void getDataSample(T& sample)
        {
            sample = getEndpoint()->getReadEndpoint()->data_sample();
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
         * data source is always a new object.
         */
        base::DataSourceBase* getDataSource()
        {
            return new internal::InputPortSource<T>(*this);
        }

        virtual bool createStream(ConnPolicy const& policy)
        {
            return internal::ConnFactory::createStream(*this, policy);
        }

#ifndef ORO_DISABLE_PORT_DATA_SCRIPTING
        /**
         * Create accessor Object for this Port, for addition to a
         * TaskContext Object interface.
         */
        virtual Service* createPortObject()
        {
            Service* object = base::InputPortInterface::createPortObject();
            // Force resolution on the overloaded write method
            typedef FlowStatus (InputPort<T>::*ReadSample)(typename base::ChannelElement<T>::reference_t);
            ReadSample read_m = &InputPort<T>::read;
            object->addSynchronousOperation("read", read_m, this).doc("Reads a sample from the port.").arg("sample", "");
            object->addSynchronousOperation("clear", &InputPortInterface::clear, this).doc("Clears any remaining data in this port. After a clear, a read() will return NoData if no writes happened in between.");
            return object;
        }
#endif

        virtual internal::ConnOutputEndpoint<T>* getEndpoint() const
        {
            assert(endpoint);
            return endpoint.get();
        }

        virtual typename base::ChannelElement<T>::shared_ptr getSharedBuffer() const
        {
            return getEndpoint()->getSharedBuffer();
        }
    };
}

#endif

