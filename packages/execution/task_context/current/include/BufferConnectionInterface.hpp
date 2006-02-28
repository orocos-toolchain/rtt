#ifndef ORO_EXECUTION_BUFFER_CONNECTION_INTERFACE_HPP
#define ORO_EXECUTION_BUFFER_CONNECTION_INTERFACE_HPP

#include "ConnectionInterface.hpp"
#include <corelib/BufferInterface.hpp>

namespace ORO_Execution
{
    template<class T>
    struct ReadConnectionInterface
        : public virtual ConnectionInterface
    {
        typedef boost::intrusive_ptr< ReadConnectionInterface<T> > shared_ptr;
        virtual ReadInterface<T>* read() = 0;
    };

    template<class T>
    struct WriteConnectionInterface
        : public virtual ConnectionInterface
    {
        typedef boost::intrusive_ptr< WriteConnectionInterface<T> > shared_ptr;
        virtual WriteInterface<T>* write() = 0;
    };

    template<class T>
    struct BufferConnectionInterface
        : public ReadConnectionInterface<T>, public WriteConnectionInterface<T>
    {
        typedef boost::intrusive_ptr< BufferConnectionInterface<T> > shared_ptr;
        virtual BufferInterface<T>* buffer() = 0;
    };

}

#endif
