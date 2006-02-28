#ifndef ORO_EXECUTION_BUFFERDATASOURCE_HPP
#define ORO_EXECUTION_BUFFERDATASOURCE_HPP

#include <corelib/DataSource.hpp>
#include <corelib/BufferInterface.hpp>

namespace ORO_Execution
{
    /**
     * A DataSource which provides read-only access to
     * the next value to be read from the buffer, without
     * removing that value from the buffer.
     */
    template<class T>
    class BufferDataSource
        : public ORO_CoreLib::DataSource<T>
    {
        typename BufferInterface<T>::shared_ptr  mbuf;
    public:
        BufferDataSource(typename BufferInterface<T>::shared_ptr buf )
            : mbuf( buf )
        {
        }

        typename ORO_CoreLib::DataSource<T>::result_t get() const {
            return mbuf->front();
        }

        BufferDataSource<T>* clone() const {
            return new BufferDataSource<T>( mbuf );
        }

        BufferDataSource<T>* copy(std::map<const DataSourceBase*, DataSourceBase*>& clones) {
            clones[this] = this;
            return this;
        }

    };
}

#endif
