
#ifndef ORO_QUEUE_HPP
#define ORO_QUEUE_HPP

#include "rtt-config.h"

/**
 * We can't use typedefs since C++ doesn't allow it for
 * templated classes without specifying all the template
 * parameters.
 */
#if defined(OROBLD_OS_NO_ASM)
#include "LockedQueue.hpp"
#else
#include "AtomicQueue.hpp"
#endif

namespace RTT
{
    /**
     * This object represents the default queue implementation used
     * by Orocos objects.
     */
    template< class T, class ReadPolicy = NonBlockingPolicy, class WritePolicy = NonBlockingPolicy>
    class Queue
#if defined(OROBLD_OS_NO_ASM)
        : public LockedQueue<T,ReadPolicy,WritePolicy>
#else
        : public AtomicQueue<T,ReadPolicy,WritePolicy>
#endif
    {
    public:
        /**
         * Create a queue of fixed size.
         * Optionally, add the number of threads that may
         * concurrently access this queue.
         */
        Queue( int qsize, int t = ORONUM_OS_MAX_THREADS)
#if defined(OROBLD_OS_NO_ASM)
            : LockedQueue<T,ReadPolicy,WritePolicy>(qsize,t)
#else
            : AtomicQueue<T,ReadPolicy,WritePolicy>(qsize,t)
#endif
        {}
    };
}

#endif
