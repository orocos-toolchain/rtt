
#ifndef ORO_LIST_HPP_
#define ORO_LIST_HPP_

#include "rtt-config.h"

/**
 * We can't use typedefs since C++ doesn't allow it for
 * templated classes without specifying all the template
 * parameters.
 */
#if defined(OROBLD_OS_NO_ASM)
#include "ListLocked.hpp"
#else
#include "ListLockFree.hpp"
#endif

namespace RTT
{
    /**
     * This object represents the default queue implementation used
     * by Orocos objects.
     */
    template< class T>
    class List
#if defined(OROBLD_OS_NO_ASM)
        : public ListLocked<T>
#else
        : public ListLockFree<T>
#endif
    {
    public:
        /**
         * Create a queue of fixed size.
         * Optionally, add the number of threads that may
         * concurrently access this queue.
         */
        List( int qsize, int t = ORONUM_OS_MAX_THREADS)
#if defined(OROBLD_OS_NO_ASM)
            : ListLocked<T(qsize,t)
#else
            : ListLockFree<T>(qsize,t)
#endif
        {}
    };
}


#endif /* LIST_HPP_ */
