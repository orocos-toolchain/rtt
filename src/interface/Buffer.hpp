
#ifndef ORO_LIST_HPP
#define ORO_LIST_HPP

#include "rtt-config.h"

/**
 * We can't use typedefs since C++ doesn't allow it for
 * templated classes without specifying all the template
 * parameters.
 */
#if defined(OROBLD_OS_NO_ASM)
#include "BufferLocked.hpp"
#else
#include "BufferLockFree.hpp"
#endif

namespace RTT
{
    /**
     * This object represents the default queue implementation used
     * by Orocos objects.
     */
    template< class T, class ReadPolicy = NonBlockingPolicy, class WritePolicy = NonBlockingPolicy >
    class Buffer
#if defined(OROBLD_OS_NO_ASM)
        : public BufferLocked<T,ReadPolicy,WritePolicy>
#else
        : public BufferLockFree<T,ReadPolicy,WritePolicy>
#endif
    {
    public:
        typedef typename ReadInterface<T>::reference_t reference_t;
        typedef typename WriteInterface<T>::param_t param_t;
        typedef typename BufferInterface<T>::size_type size_type;
        typedef T value_t;

    public:
        /**
         * Create a queue of fixed size.
         * Optionally, add the number of threads that may
         * concurrently access this queue.
         */
        Buffer( int qsize, const T& initial_value = T())
#if defined(OROBLD_OS_NO_ASM)
            : BufferLocked<T,ReadPolicy,WritePolicy>(qsize, initial_value)
#else
            : BufferLockFree<T,ReadPolicy,WritePolicy>(qsize, initial_value)
#endif
        {}
    };
}


#endif /* LIST_HPP_ */
