#ifndef ORO_CORELIB_ATOMIC_QUEUE_HPP
#define ORO_CORELIB_ATOMIC_QUEUE_HPP

#include "../os/CAS.hpp"
#include "../base/BufferPolicy.hpp"
#include <utility>

namespace RTT
{
    namespace internal {
    /**
     * Create an atomic, non-blocking single ended queue (FIFO) for storing
     * a pointer \a T by value. It is a
     * Many Readers, Many Writers implementation
     * based on the atomic Compare And Swap instruction. Any number of threads
     * may access the queue concurrently.
     * @warning You can not store null pointers.
     * @param T The pointer type to be stored in the Queue.
     * Example : AtomicQueue< A* > is a queue of pointers to A.
     * @param ReadPolicy The Policy to block (wait) on \a empty (during dequeue)
     * using \a BlockingPolicy, or to return \a false, using \a NonBlockingPolicy (Default).
     * This does not influence partial filled queue behaviour.
     * @param WritePolicy The Policy to block (wait) on \a full (during enqueue),
     * using \a BlockingPolicy, or to return \a false, using \a NonBlockingPolicy (Default).
     * This does not influence partial filled buffer behaviour.
     * @ingroup CoreLibBuffers
     */
    template<class T, class ReadPolicy = base::NonBlockingPolicy, class WritePolicy = base::NonBlockingPolicy>
    class AtomicQueue
    {
        //typedef _T* T;
        const int _size;
        typedef std::pair<T, int>  C;
//        typedef volatile C* volatile WriteType;
//        typedef volatile C* volatile const ReadType;
        typedef volatile C* CachePtrType;
        typedef C* volatile CacheObjType;
        typedef C  ValueType;
        typedef C* PtrType;

        union SIndexes
        {
        	unsigned long _value;
        	unsigned short _index[2];
        };

        /**
         * The pointer to the buffer can be cached,
         * the contents are volatile.
         */
        CachePtrType  _buf;

        /**
         * The indexes are packed into one double word.
         * Therefore the read and write index can be read and written atomically.
         */
        volatile SIndexes _indxes;

        WritePolicy write_policy;
        ReadPolicy read_policy;

        /**
         * Atomic advance and wrap of the Write pointer.
         * Return the old position or zero if queue is full.
         */
        CachePtrType advance_w()
        {
        	SIndexes oldval, newval;
        	bool full=false;
            do {
            	oldval._value = _indxes._value; /*Points to a free writable pointer.*/
                newval._value = oldval._value; /*Points to the next writable pointer.*/
                // check for full :
                if ( (newval._index[0] == newval._index[1] - 1) || (newval._index[0] == newval._index[1] + _size - 1) )
                {
                    return 0;
                }
                newval._index[0]++;
                if ( newval._index[0] >= _size )
                	newval._index[0] = 0;
                // if ptr is unchanged, replace it with newval.
            } while ( !os::CAS( &_indxes._value, oldval._value, newval._value) );
            // frome here on :
            // oldval is 'unique', other preempting threads
            // will have a different value for oldval, as
            // _wptr advances.
            if(full==true)
            	return 0;
            // return the old position to write to :
            return &_buf[oldval._index[0] ];
        }
        /**
         * Atomic advance and wrap of the Read pointer.
         * Return the data position or zero if queue is empty.
         */
        CachePtrType advance_r()
        {
        	SIndexes oldval, newval;
            do {
            	oldval._value = _indxes._value;
            	newval._value = oldval._value;
                // check for empty :
                if ( newval._index[0] == newval._index[1] )
                {
                	//EnableIrq(ic);
                    return 0;
                	//empty=true;
                }
                newval._index[1]++;
                if ( newval._index[1] >= _size )
                	newval._index[1] = 0;
                // if indexes are unchanged, replace them with newval.
            } while ( !os::CAS( &_indxes._value, oldval._value, newval._value) );
            // frome here on :
            // oldval is 'unique', other preempting threads
            // will have a different value for oldval, as
            // _rptr advances.

            // return the old position to read from :
            return &_buf[oldval._index[1] ];
        }

        // non-copyable !
        AtomicQueue( const AtomicQueue<T>& );
    public:
        typedef unsigned int size_type;

        /**
         * Create an AtomicQueue with queue size \a size.
         * @param size The size of the queue, should be 1 or greater.
         */
        AtomicQueue( unsigned int size )
            : _size(size+1), write_policy(size), read_policy(0)
        {
            _buf= new C[_size];
            this->clear();
        }

        ~AtomicQueue()
        {
            delete[] _buf;
        }

        /**
         * Inspect if the Queue is full.
         * @return true if full, false otherwise.
         */
        bool isFull() const
        {
            // two cases where the queue is full :
            // if wptr is one behind rptr or if wptr is at end
            // and rptr at beginning.
            return _indxes._index[0] == _indxes._index[1] - 1 || _indxes._index[0] == _indxes._index[1] + _size - 1;
        }

        /**
         * Inspect if the Queue is empty.
         * @return true if empty, false otherwise.
         */
        bool isEmpty() const
        {
            // empty if nothing to read.
            return _indxes._index[0] == _indxes._index[1];
        }

        /**
         * Return the maximum number of items this queue can contain.
         */
        size_type capacity() const
        {
            return _size -1;
        }

        /**
         * Return the number of elements in the queue.
         */
        size_type size() const
        {
            int c = (_indxes._index[0] - _indxes._index[1]);
            return c >= 0 ? c : c + _size;
        }

        /**
         * Enqueue an item.
         * @param value The value to enqueue.
         * @return false if queue is full, true if queued.
         */
        bool enqueue(const T& value)
        {
            if ( value == 0 )
                return false;
            write_policy.pop();
            CachePtrType loc = advance_w();
            if ( loc == 0 )
                return false;
            loc->first = value;
            read_policy.push();
            return true;
        }

        /**
         * Enqueue an item and return its 'ticket' number.
         * @param value The value to enqueue.
         * @return zero if the queue is full, the 'ticket' number otherwise.
         */
        int enqueueCounted(const T& value)
        {
            if ( value == 0 )
                return 0;
            write_policy.pop();
            CachePtrType loc = advance_w();
            if ( loc == 0 )
                return 0;
            loc->first = value;
            read_policy.push();
            return loc->second;
        }

        /**
         * Dequeue an item.
         * @param value The value dequeued.
         * @return false if queue is empty, true if dequeued.
         */
        bool dequeue( T& result )
        {
            read_policy.pop();
            CachePtrType loc = advance_r();
            if ( loc == 0 )
                return false;
            result = loc->first;
            loc->second += _size; // give the cell a new number.
            loc->first   = 0; // this releases the cell to write to.
            write_policy.push();
            return true;
        }

        /**
         * Return the next to be read value.
         */
        const T front() const
        {
            return _buf[_indxes._index[1] ];
        }

        /**
         * Return the next to be read value and lock
         * it in a MemoryPool, such that it is not freed.
         * The returned pointer must be unlock()'ed by the
         * user's code.
         */
        template<class MPoolType>
        T lockfront(MPoolType& mp) const
        {
            CachePtrType loc=0;
            bool was_locked = false;
            do {
                if (was_locked)
                    mp.unlock(loc->first);
                loc = &_buf[_indxes._index[1] ];
                if (loc->first == 0)
                    return 0;
                was_locked = mp.lock(loc->first);
                // retry if lock failed or read moved.
            } while( !was_locked || loc != &_buf[_indxes._index[1] ] ); // obstruction detection.
            return loc->first;
        }

        /**
         * Dequeue an item and return the same 'ticket' number when it was queued.
         * @param value The value dequeued.
         * @return zero if the queue is empty, the 'ticket' number otherwise.
         */
        int dequeueCounted( T& result )
        {
            read_policy.pop();
            CachePtrType loc = advance_r();
            if ( loc == 0 )
                return 0;
            result = loc->first;
            int nr = loc->second;
            loc->second += _size; // give the cell a new number.
            loc->first = 0; // this releases the cell to write to.
            write_policy.push();
            return nr;
        }

        /**
         * Clear all contents of the Queue and thus make it empty.
         */
        void clear()
        {
            for(int i = 0 ; i != _size; ++i) {
                if ( _buf[i].first != 0 ) {
                    _buf[i].first  = 0;
                }
                _buf[i].second = i+1; // init the counters
            }
            _indxes._value = 0;
            write_policy.reset( _size - 1 );
            read_policy.reset(0);
        }

    };

}}

#endif
