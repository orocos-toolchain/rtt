#ifndef ORO_EXECUTION_DATA_CONNECTION_INTERFACE_HPP
#define ORO_EXECUTION_DATA_CONNECTION_INTERFACE_HPP

#include <corelib/DataObjectInterfaces.hpp>
#include "ConnectionInterface.hpp"

namespace ORO_Execution
{
    template<class T>
    struct DataConnectionInterface
        : public ConnectionInterface
    {
        typedef boost::intrusive_ptr< DataConnectionInterface<T> > shared_ptr;

        virtual const DataObjectInterface<T>* data() const = 0;

        virtual DataObjectInterface<T>* data() = 0;
    };

}

#endif
