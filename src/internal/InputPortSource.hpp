#ifndef ORO_INPUT_PORT_SOURCE_HPP
#define ORO_INPUT_PORT_SOURCE_HPP

#include "Channels.hpp"
#include "../base/PortInterface.hpp"

namespace RTT
{ namespace internal {

    /** This class represents a read port using the data source interface.
     * Beware that, depending on the connection used, ports actually change
     * their state when read. For instance, a buffer connection *will* lose one
     * element when get() or evaluate() are called. For that reason, it is
     * considered bad practice to use a data source on a read port that is bound
     * to a buffer connection.
     *
     * This class should not be used directly in normal code. What you would
     * usually do is create a new read port using OutputPort::antiClone() and
     * call base::InputPortInterface::getDataSource() to get the corresponding data
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
        DataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const
        { return const_cast<InputPortSource<T>*>(this); }
    };
}}

#endif

