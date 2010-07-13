#ifndef ORO_MWSR_QUEUE_HPP
#define ORO_MWSR_QUEUE_HPP

#include "../rtt-config.h"

/**
 * @file We can't use typedefs since C++ doesn't allow it for
 * templated classes without specifying all the template
 * parameters.
 */
#if defined(OROBLD_OS_NO_ASM)
#include "LockedQueue.hpp"
#else
#include "AtomicMWSRQueue.hpp"
#endif

namespace RTT
{
    namespace internal
    {

        /**
         * This object represents the default Multi-Writer, Single-Reader queue implementation used
         * by Orocos objects.
         */
        template<class T>
        class MWSRQueue
#if defined(OROBLD_OS_NO_ASM)
                : public LockedQueue<T>
#else
                : public AtomicMWSRQueue<T>
#endif
        {
        public:
            /**
             * Create a mw/sr queue of fixed size.
             */
            MWSRQueue(int qsize)
#if defined(OROBLD_OS_NO_ASM)
            : LockedQueue<T>(qsize)
#else
            : AtomicMWSRQueue<T> (qsize)
#endif
            {
            }
        };
    }
}

#endif
