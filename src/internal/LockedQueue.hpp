#ifndef ORO_LOCKED_QUEUE_HPP
#define ORO_LOCKED_QUEUE_HPP

#include <deque>
#include "../base/BufferPolicy.hpp"
#include "os/Mutex.hpp"
#include "os/MutexLock.hpp"

namespace RTT
{
    /**
     * A lock-based queue implementation to \a enqueue or \a dequeue
     * a pointer of type \a T.
     * No memory allocation is done during read or write.
     * @param T The pointer type to be stored in the queue.
     * Example : \begincode LockedQueue<A*> \endcode is a queue which holds values of type A.
     * @param ReadPolicy The Policy to block (wait) on \a empty (during dequeue)
     * using \a BlockingPolicy, or to return \a false, using \a NonBlockingPolicy (Default).
     * This does not influence partial filled queue behaviour.
     * @param WritePolicy The Policy to block (wait) on \a full (during enqueue),
     * using \a BlockingPolicy, or to return \a false, using \a NonBlockingPolicy (Default).
     * This does not influence partial filled buffer behaviour.
     * @ingroup CoreLibBuffers
     */
    template< class T, class ReadPolicy = NonBlockingPolicy, class WritePolicy = NonBlockingPolicy>
    class LockedQueue
    {
    public:
        typedef T value_t;
    private:
        typedef std::deque<value_t> BufferType;
        typedef typename BufferType::iterator Iterator;
        typedef typename BufferType::const_iterator CIterator;
        mutable OS::Mutex lock;
        BufferType data;

        int cap;
        WritePolicy write_policy;
        ReadPolicy read_policy;

        int counter;
        int dcounter;
    public:
        typedef unsigned int size_type;

        /**
         * Create a lock-based queue wich can store \a lsize elements.
         * @param lsize the capacity of the queue.
'        */
        LockedQueue(unsigned int lsize, unsigned int unused = 0)
            : cap(lsize),
              write_policy(lsize), read_policy(0),
              counter(0), dcounter(0)
        {
            data.resize(lsize);
            data.resize(0);
        }

        ~LockedQueue() {
        }

        size_type capacity() const
        {
            return cap;
        }

        size_type size() const
        {
            OS::MutexLock locker(lock);
            return data.size();
        }

        /**
         * Inspect if the Queue is empty.
         * @return true if empty, false otherwise.
         */
        bool isEmpty() const
        {
            OS::MutexLock locker(lock);
            return data.empty();
        }

        /**
         * Inspect if the Queue is full.
         * @return true if full, false otherwise.
         */
        bool isFull() const
        {
            OS::MutexLock locker(lock);
            return data.size() ==  cap;
        }

        void clear()
        {
            OS::MutexLock locker(lock);
            data.clear();
        }

        /**
         * Enqueue an item.
         * @param value The value to enqueue.
         * @return false if queue is full, true if queued.
         */
        bool enqueue(const T& value)
        {
            write_policy.pop();
            {
                OS::MutexLock locker(lock);
                if (cap == data.size() )
                    return false;
                data.push_back(value);
            }
            read_policy.push();
            return true;
        }

        /**
         * Enqueue an item and return its 'ticket' number.
         * @param value The value to enqueue.
         * @return zero if the queue is full, the 'ticket' number otherwise.
         * @deprecated <b> Do not use this function </b>
         */
        int enqueueCounted(const T& value)
        {
            int ret;
            write_policy.pop();
            {
                OS::MutexLock locker(lock);
                if (cap == data.size() )
                    return 0;
                data.push_back(value);
                ret = ++counter;
            }
            read_policy.push();
            return ret;
        }

        /**
         * Dequeue an item.
         * @param result The value dequeued.
         * @return false if queue is empty, true if dequeued.
         */
        bool dequeue( T& result )
        {
            read_policy.pop();
            {
                OS::MutexLock locker(lock);
                if ( data.empty() )
                    return false;
                result = data.front();
                data.pop_front();
            }
            write_policy.push();
            return true;
        }

        /**
         * Dequeue an item and return the same 'ticket' number when it was queued.
         * @param value The value dequeued.
         * @return zero if the queue is empty, the 'ticket' number otherwise.
         * @deprecated <b> Do not use this function </b>
         */
        int dequeueCounted( T& result )
        {
            int ret;
            read_policy.pop();
            {
                OS::MutexLock locker(lock);
                if ( data.empty() )
                    return 0;
                result = data.front();
                data.pop_front();
                ret = ++dcounter;
            }
            write_policy.push();
            return ret;
        }

        /**
         * Returns the first element of the queue.
         */
        value_t front() const
        {
            OS::MutexLock locker(lock);
            value_t item = value_t();
            if ( !data.empty() )
                item = data.front();
            return item;
        }

        /**
         * Returns the last element of the queue.
         */
        value_t back() const
        {
            OS::MutexLock locker(lock);
            value_t item = value_t();
            if ( !data.empty() )
                item = data.back();
            return item;
        }

    };

}

#endif
