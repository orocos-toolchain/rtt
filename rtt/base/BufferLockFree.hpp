/***************************************************************************
  tag: Peter Soetens  Thu Jan 13 10:24:51 CET 2005  BufferLockFree.hpp

                        BufferLockFree.hpp -  description
                           -------------------
    begin                : Thu January 13 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef ORO_BUFFER_LOCK_FREE_HPP
#define ORO_BUFFER_LOCK_FREE_HPP

#include "../os/oro_arch.h"
#include "../os/Atomic.hpp"
#include "../os/CAS.hpp"
#include "BufferInterface.hpp"
#include "../internal/AtomicMWSRQueue.hpp"
#include "../internal/AtomicMWMRQueue.hpp"
#include "../internal/TsPool.hpp"
#include <vector>

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{ namespace base {


    /**
     * A Lock-free buffer implementation to read and write
     * data of type \a T in a FIFO way.
     * No memory allocation is done during read or write.
     * One thread may read this buffer, unless the multiple_readers flag
     * in the \ref base::BufferBase::Options struct is set.
     * Any number of threads may write this buffer.
     * @param T The value type to be stored in the Buffer.
     * Example : BufferLockFree<A> is a buffer which holds values of type A.
     * @ingroup PortBuffers
     */
    template<class T>
    class BufferLockFree
        : public BufferInterface<T>
    {
    public:
        typedef typename BufferBase::Options Options;
        typedef typename BufferInterface<T>::reference_t reference_t;
        typedef typename BufferInterface<T>::param_t param_t;
        typedef typename BufferInterface<T>::size_type size_type;
        typedef T value_t;

        /**
         * @brief The maximum number of threads.
         */
        const unsigned int MAX_THREADS;

    private:
        typedef value_t Item;
        const bool mcircular;
        bool initialized;

        internal::AtomicQueue<Item*> *const bufs;
        internal::TsPool<Item> *const mpool;

        RTT::os::AtomicInt droppedSamples;

    public:
        /**
         * Create an uninitialized lock-free buffer which can store \a bufsize elements.
         * @param bufsize the capacity of the buffer.
'         */
        BufferLockFree( unsigned int bufsize, const Options &options = Options() )
            : MAX_THREADS(options.max_threads())
            , mcircular(options.circular()), initialized(false)
            , bufs((!options.circular() && !options.multiple_readers()) ?
                       static_cast<internal::AtomicQueue<Item*> *>(new internal::AtomicMWSRQueue<Item*>(bufsize)) :
                       static_cast<internal::AtomicQueue<Item*> *>(new internal::AtomicMWMRQueue<Item*>(bufsize)))
            , mpool(new internal::TsPool<Item>(bufsize + options.max_threads()))
            , droppedSamples(0)
        {
        }

        /**
         * Create a lock-free buffer which can store \a bufsize elements.
         * @param bufsize the capacity of the buffer.
         * @param initial_value A data sample with which each preallocated data element is initialized.
         */
        BufferLockFree( unsigned int bufsize, param_t initial_value, const Options &options = Options() )
            : MAX_THREADS(options.max_threads())
            , mcircular(options.circular()), initialized(false)
            , bufs((!options.circular() && !options.multiple_readers()) ?
                       static_cast<internal::AtomicQueue<Item*> *>(new internal::AtomicMWSRQueue<Item*>(bufsize)) :
                       static_cast<internal::AtomicQueue<Item*> *>(new internal::AtomicMWMRQueue<Item*>(bufsize)))
            , mpool(new internal::TsPool<Item>(bufsize + options.max_threads()))
            , droppedSamples(0)
        {
            data_sample( initial_value );
        }

        ~BufferLockFree() {
            // free all items still in the buffer.
            clear();

            delete mpool;
            delete bufs;
        }

        virtual bool data_sample( param_t sample, bool reset = true )
        {
            if (!initialized || reset) {
                mpool->data_sample(sample);
                initialized = true;
                return true;
            } else {
                return initialized;
            }

        }

        virtual value_t data_sample() const
        {
            value_t result = value_t();
            Item* mitem = mpool->allocate();
            if (mitem != 0) {
                result = *mitem;
                mpool->deallocate( mitem );
            }
            return result;
        }


        size_type capacity() const
        {
            return bufs->capacity();
        }

        size_type size() const
        {
            return bufs->size();
        }

        bool empty() const
        {
            return bufs->isEmpty();
        }

        bool full() const
        {
            return bufs->isFull();
        }

        void clear()
        {
            Item* item;
            while ( bufs->dequeue(item) )
                mpool->deallocate( item );
        }

        virtual size_type dropped() const
        {
            return droppedSamples.read();
        }
        
        bool Push( param_t item)
        {
            if (!mcircular && ( capacity() == (size_type)bufs->size() )) {
                droppedSamples.inc();
                return false;
                // we will recover below in case of circular
            }
            Item* mitem = mpool->allocate();
            if ( mitem == 0 ) { // queue full ( rare but possible in race with PopWithoutRelease )
                if (!mcircular) {
                    droppedSamples.inc();
                    return false;
                }
                else {
                    if (bufs->dequeue( mitem ) == false ) {
                        droppedSamples.inc();
                        return false; // assert(false) ???
                    }
                    // we keep mitem to write item to next
                }
            }

            // copy over.
            *mitem = item;
            if (bufs->enqueue( mitem ) == false ) {
                //got memory, but buffer is full
                //this can happen, as the memory pool is
                //bigger than the buffer
                if (!mcircular) {
                    mpool->deallocate( mitem );
                    droppedSamples.inc();
                    return false;
                } else {
                    // pop & deallocate until we have free space.
                    Item* itmp = 0;
                    do {
                        if ( bufs->dequeue( itmp ) ) {
                            mpool->deallocate( itmp );
                            droppedSamples.inc();
                        } else {
                            // Both operations, enqueue() and dequeue() failed on the buffer:
                            // We could free the allocated pool item return false here,
                            // but in fact this can only happen during massive concurrent
                            // access to the circular buffer or in the trivial case that
                            // the buffer size is zero. So just keep on trying...
                        }
                    } while ( bufs->enqueue( mitem ) == false );
                }
            }
            return true;
        }

        size_type Push(const std::vector<value_t>& items)
        {
            // @todo Make this function more efficient as in BufferLocked.
            int towrite  = items.size();
            size_type written = 0;
            typename std::vector<value_t>::const_iterator it;
            for(  it = items.begin(); it != items.end(); ++it) {
                if ( this->Push( *it ) == false ) {
                    break; // will only happen in non-circular case !
                }
                written++;
            }
            droppedSamples.add(towrite - written);
            return written;
        }


        FlowStatus Pop( reference_t item )
        {
            Item* ipop;
            if (bufs->dequeue( ipop ) == false )
                return NoData;
            item = *ipop;
            if (mpool->deallocate( ipop ) == false )
                assert(false);
            return NewData;
        }

        size_type Pop(std::vector<value_t>& items )
        {
            Item* ipop;
            items.clear();
            while( bufs->dequeue(ipop) ) {
                items.push_back( *ipop );
                if (mpool->deallocate(ipop) == false)
                    assert(false);
            }
            return items.size();
        }
        
        value_t* PopWithoutRelease()
        {
            Item* ipop;
            if (bufs->dequeue( ipop ) == false )
                return 0;
            return ipop;
        }

        void Release(value_t *item)
        {
            if (mpool->deallocate( item ) == false )
                assert(false);
        }
    };
}}

#endif
