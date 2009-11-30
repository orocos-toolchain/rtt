/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  ListLockFree.hpp

                        ListLockFree.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be

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


#ifndef ORO_LIST_LOCK_FREE_HPP
#define ORO_LIST_LOCK_FREE_HPP

#include <vector>
#include "os/oro_atomic.h"
#include "os/CAS.hpp"
#include <boost/intrusive_ptr.hpp>

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{
    namespace detail {
        struct IntrusiveStorage
        {
            oro_atomic_t ref;
            IntrusiveStorage() {
                oro_atomic_set(&ref,0);
            }
            virtual ~IntrusiveStorage() {
            }
        };
    }
}


void intrusive_ptr_add_ref(RTT::detail::IntrusiveStorage* p );
void intrusive_ptr_release(RTT::detail::IntrusiveStorage* p );

namespace RTT
{
    /**
     * A \a simple lock-free list implementation to \a append or \a erase
     * data of type \a T.
     * No memory allocation is done during read or write, but the maximum number
     * of threads which can access this object is defined by
     * MAX_THREADS.
     * @param T The value type to be stored in the list.
     * Example : ListLockFree<A> is a list which holds values of type A.
     * @ingroup CoreLibBuffers
     */
    template< class T>
    class ListLockFree
    {
    public:
        /**
         * @brief The maximum number of threads.
         *
         * The number of threads which may concurrently access this buffer.
         */
        const unsigned int MAX_THREADS;

        typedef T value_t;
    private:
        typedef std::vector<value_t> BufferType;
        typedef typename BufferType::iterator Iterator;
        typedef typename BufferType::const_iterator CIterator;
        struct Item {
            Item()  {
                //ORO_ATOMIC_INIT(count);
                oro_atomic_set(&count,-1);
            }
            mutable oro_atomic_t count;  // refcount
            BufferType data;
        };

        struct StorageImpl : public detail::IntrusiveStorage
        {
            Item* items;
            StorageImpl(size_t alloc) : items( new Item[alloc] ) {
            }
            ~StorageImpl() {
                delete[] items;
            }
            Item& operator[](int i) {
                return items[i];
            }
        };

        /**
         * The intrusive_ptr is far more thread-safe than the
         * shared_ptr for 'reads' during 'assignments'.
         */
        typedef boost::intrusive_ptr<StorageImpl> Storage;

        Storage newStorage(size_t alloc, size_t items, bool init = true)
        {
            Storage st( new StorageImpl(alloc) );
            for (unsigned int i=0; i < alloc; ++i) {
                (*st)[i].data.reserve( items ); // pre-allocate
            }
            // bootstrap the first list :
            if (init) {
                active = &(*st)[0];
                oro_atomic_inc( &active->count );
            }

            return st;
        }

        Storage bufs;
        Item* volatile active;
        Item* volatile blankp;

        // each thread has one 'working' buffer, and one 'active' buffer
        // lock. Thus we require to allocate twice as much buffers as threads,
        // for all the locks to succeed in a worst case scenario.
        inline size_t BufNum() const {
            return MAX_THREADS * 2;
        }

        size_t required;
    public:
        /**
         * Create a lock-free list wich can store \a lsize elements.
         * @param lsize the capacity of the list.
         * @param threads the number of threads which may concurrently
         * read or write this buffer. Defaults to ORONUM_OS_MAX_THREADS, but you
         * may lower this number in case not all threads will read this buffer.
         * A lower number will consume less memory.
'        */
        ListLockFree(unsigned int lsize, unsigned int threads = ORONUM_OS_MAX_THREADS )
            : MAX_THREADS( threads ), blankp(0), required(lsize)
        {
            const unsigned int BUF_NUM = BufNum();
            bufs = newStorage( BUF_NUM, lsize );
        }

        ~ListLockFree() {
        }

        size_t capacity() const
        {
            size_t res;
            Storage st;
            Item* orig = lockAndGetActive(st);
            res = orig->data.capacity();
            oro_atomic_dec( &orig->count ); // lockAndGetActive
            return res;
        }

        size_t size() const
        {
            size_t res;
            Storage st;
            Item* orig = lockAndGetActive(st);
            res = orig->data.size();
            oro_atomic_dec( &orig->count ); // lockAndGetActive
            return res;
        }

        bool empty() const
        {
            bool res;
            Storage st;
            Item* orig = lockAndGetActive(st);
            res = orig->data.empty();
            oro_atomic_dec( &orig->count ); // lockAndGetActive
            return res;
        }

        /**
         * Grow the capacity to contain at least n additional items.
         * This method tries to avoid too much re-allocations, by
         * growing a bit more than required every N invocations and
         * growing nothing in between.
         * @param items The number of items to at least additionally reserve.
         */
        void grow(size_t items = 1) {
            required += items;
            if (required > this->capacity()) {
                this->reserve( required*2 );
            }
        }
        /**
         * Shrink the capacity with at most n items.
         * This method does not actually free memory, it just prevents
         * a (number of) subsequent grow() invocations to allocate more
         * memory.
         * @param items The number of items to at most remove from the capacity.
         */
        void shrink(size_t items = 1) {
            required -= items;
        }

        /**
         * Reserve a capacity for this list.
         * If you wish to invoke this method concurrently, guard it
         * with a mutex. The other methods may be invoked concurrently
         * with this method.
         * @param lsize the \a minimal number of items this list will be
         * able to hold. Will not drop below the current capacity()
         * and this method will do nothing if \a lsize < this->capacity().
         */
        void reserve(size_t lsize)
        {
            if (lsize <= this->capacity() )
                return;

            const unsigned int BUF_NUM = BufNum();
            Storage res( newStorage(BUF_NUM, lsize, false) );

            // init the future 'active' buffer.
            Item* nextbuf = &(*res)[0];
            oro_atomic_inc( &nextbuf->count );

            // temporary for current active buffer.
            Item* orig = 0;

            // prevent current bufs from deletion.
            // will free upon return.
            Storage save = bufs;
            // active points at old, bufs points at new:
            // first the refcount is added to res, then
            // bufs' pointer is switched to res' pointer,
            // and stored in a temporary. Then the temp
            // is destructed and decrements bufs' old reference.
            bufs = res;
            // from now on, any findEmptyBuf will use the new bufs,
            // unless the algorithm was entered before the switch.
            // then, it will write the result to the old buf.
            // if it detects we updated active, it will find an
            // empty buf in the new buf. If it gets there before
            // our CAS, our CAS will fail and we try to recopy
            // everything. This retry may be unnessessary
            // if the data already is in the new buf, but for this
            // cornercase, we must be sure.

            // copy active into new:
            do {
                if (orig)
                    oro_atomic_dec(&orig->count);
                orig = lockAndGetActive(); // active is guaranteed to point in valid buffer ( save or bufs )
                nextbuf->data.clear();
                Iterator it( orig->data.begin() );
                while ( it != orig->data.end() ) {
                    nextbuf->data.push_back( *it );
                    ++it;
                }
                // see explanation above: active could have changed,
                // and still point in old buffer. we could check this
                // with pointer arithmetics, but this is not a performant
                // method.
            } while ( OS::CAS(&active, orig, nextbuf ) == false);
            // now,
            // active is guaranteed to point into bufs.
            assert( pointsTo( active, bufs ) );

            oro_atomic_dec( &orig->count ); // lockAndGetActive
            oro_atomic_dec( &orig->count ); // ref count
        }

        void clear()
        {
            Storage bufptr;
            Item* orig(0);
            Item* nextbuf(0);
            int items = 0;
            do {
                if (orig) {
                    oro_atomic_dec(&orig->count);
                    oro_atomic_dec(&nextbuf->count);
                }
                orig = lockAndGetActive(bufptr);
                items = orig->data.size();
                nextbuf = findEmptyBuf(bufptr); // find unused Item in bufs
                nextbuf->data.clear();
            } while ( OS::CAS(&active, orig, nextbuf ) == false );
            oro_atomic_dec( &orig->count ); // lockAndGetActive
            oro_atomic_dec( &orig->count ); // ref count
        }

        /**
         * Append a single value to the list.
         * @param d the value to write
         * @return false if the list is full.
         */
        bool append( value_t item )
        {
            Item* orig=0;
            Storage bufptr;
            Item* usingbuf(0);
            do {
                if (orig) {
                    oro_atomic_dec(&orig->count);
                    oro_atomic_dec(&usingbuf->count);
                }
                orig = lockAndGetActive( bufptr );
                if ( orig->data.size() == orig->data.capacity() ) { // check for full
                    oro_atomic_dec( &orig->count );
                    return false;
                }
                usingbuf = findEmptyBuf( bufptr ); // find unused Item in bufs
                usingbuf->data = orig->data;
                usingbuf->data.push_back( item );
            } while ( OS::CAS(&active, orig, usingbuf ) ==false);
            oro_atomic_dec( &orig->count ); // lockAndGetActive()
            oro_atomic_dec( &orig->count ); // set list free
            return true;
        }

        /**
         * Returns the first element of the list.
         */
        value_t front() const
        {
            Storage bufptr;
            Item* orig = lockAndGetActive(bufptr);
            value_t ret(orig->data.front());
            oro_atomic_dec( &orig->count ); //lockAndGetActive
            return ret;
        }

        /**
         * Returns the last element of the list.
         */
        value_t back() const
        {
            Storage bufptr;
            Item* orig = lockAndGetActive(bufptr);
            value_t ret(orig->data.back());
            oro_atomic_dec( &orig->count ); //lockAndGetActive
            return ret;
        }

        /**
         * Append a sequence of values to the list.
         * @param items the values to append.
         * @return the number of values written (may be less than d.size())
         */
        size_t append(const std::vector<T>& items)
        {
            Item* usingbuf(0);
            Item* orig=0;
            int towrite  = items.size();
            Storage bufptr;
            do {
                if (orig) {
                    oro_atomic_dec(&orig->count);
                    oro_atomic_dec(&usingbuf->count);
                }

                orig = lockAndGetActive( bufptr );
                int maxwrite = orig->data.capacity() - orig->data.size();
                if ( maxwrite == 0 ) {
                    oro_atomic_dec( &orig->count ); // lockAndGetActive()
                    return 0;
                }
                if ( towrite > maxwrite )
                    towrite = maxwrite;
                usingbuf = findEmptyBuf( bufptr ); // find unused Item in bufs
                usingbuf->data = orig->data;
                usingbuf->data.insert( usingbuf->data.end(), items.begin(), items.begin() + towrite );
            } while ( OS::CAS(&active, orig, usingbuf ) ==false );
            oro_atomic_dec( &orig->count ); // lockAndGetActive()
            oro_atomic_dec( &orig->count ); // set list free
            return towrite;
        }


        /**
         * Erase a value from the list.
         * @param item is to be erased from the list.
         * @return true if found and erased.
         */
        bool erase( value_t item )
        {
            Item* orig=0;
            Item* nextbuf(0);
            Storage bufptr;
            do {
                if (orig) {
                    oro_atomic_dec(&orig->count);
                    oro_atomic_dec(&nextbuf->count);
                }
                orig = lockAndGetActive( bufptr ); // find active in bufptr
                // we do this in the loop because bufs can change.
                nextbuf = findEmptyBuf( bufptr ); // find unused Item in same buf.
                Iterator it( orig->data.begin() );
                while (it != orig->data.end() && !( *it == item ) ) {
                    nextbuf->data.push_back( *it );
                    ++it;
                }
                if ( it == orig->data.end() ) {
                    oro_atomic_dec( &orig->count );
                    oro_atomic_dec( &nextbuf->count );
                    return false; // item not found.
                }
                ++it; // skip item.
                while ( it != orig->data.end() ) {
                    nextbuf->data.push_back( *it );
                    ++it;
                }
            } while ( OS::CAS(&active, orig, nextbuf ) ==false );
            oro_atomic_dec( &orig->count ); // lockAndGetActive
            oro_atomic_dec( &orig->count ); // ref count
            return true;
        }

        /**
         * Apply a function to the elements of the whole list.
         * @param func The function to apply.
         */
        template<class Function>
        void apply(Function func )
        {
            Storage st;
            Item* orig = lockAndGetActive(st);
            Iterator it( orig->data.begin() );
            while ( it != orig->data.end() ) {
                func( *it );
                ++it;
            }
            oro_atomic_dec( &orig->count ); //lockAndGetActive
        }

        /**
         * Apply a function to the non-blanked elements of the list.
         * If during an apply_and_blank, the erase_and_blank function
         * is called, that element will not be subject to \a func if
         * not yet processed. You must not call this function concurrently
         * from multiple threads.
         * @param func The function to apply.
         * @param blank The 'blank' item. Each item of this list will
         * be compared to this item using operator==(), if it matches,
         * it is considered blank, and func is \b not applied.
         * @see erase_and_blank
         * @deprecated This complicated function is nowhere used.
         */
        template<class Function>
        void apply_and_blank(Function func, value_t blank )
        {
            Storage st;
            Item* orig = lockAndGetActive(st);
            Item* newp = findEmptyBuf(st);
            Iterator it( orig->data.begin() );
            // first copy the whole list.
            while ( it != orig->data.end() ) {
                newp->data.push_back( *it );
                ++it;
            }
            blankp = newp;
            it = blankp->data.begin();
            // iterate over copy and skip blanks.
            while ( it != blankp->data.end() ) {
                // XXX Race condition: 'it' can be blanked after
                // comparison or even during func.
                value_t a = *it;
                if ( !(a == blank) )
                    func( a );
                ++it;
            }
            blankp = 0;

            oro_atomic_dec( &orig->count ); //lockAndGetActive
            oro_atomic_dec( &newp->count ); //findEmptyBuf
        }

        /**
         * Erase an element from the list and blank it if possible.
         * If during an apply_and_blank, the erase_and_blank function
         * is called, that element will not be subject to \a func if
         * not yet processed. You may call this function concurrently
         * from multiple threads.
         * @warning It is possible that \a item is being processed
         * within apply_and_blank. In that case the 'blank' operation
         * has no effect.
         * @param item The item to erase from the list.
         * @param blank The 'blank' item to use to blank \a item
         * from the list.
         * @see apply_and_blank
         * @deprecated This complicated function is nowhere used.
         */
        bool erase_and_blank(value_t item, value_t blank )
        {
            Storage st;
            bool res = this->erase(item);
            Item* orig = lockAndGetBlank(st);
            if (orig) {
                Iterator it( orig->data.begin() );
                // item may still not be present in the blank-list.
                while ( *it != item ) {
                    ++it;
                    if (it == orig->data.end() ) {
                        oro_atomic_dec( &orig->count ); //lockAndGetBlank
                        return res;
                    }
                }
                (*it) = blank;
                oro_atomic_dec( &orig->count ); //lockAndGetBlank
            }
            return res;
        }

        /**
         * Find an item in the list such that func( item ) == true.
         * @param blank The value to return if not found.
         * @return The item that matches func(item) or blank if none matches.
         */
        template<class Function>
        value_t find_if( Function func, value_t blank = value_t() )
        {
            Storage st;
            Item* orig = lockAndGetActive(st);
            Iterator it( orig->data.begin() );
            while ( it != orig->data.end() ) {
                if (func( *it ) == true ) {
                    oro_atomic_dec( &orig->count ); //lockAndGetActive
                    return *it;
                }
                ++it;
            }
            oro_atomic_dec( &orig->count ); //lockAndGetActive
            return blank;
        }
    private:
        /**
         * Item returned is guaranteed to point into bufptr.
         */
        Item* findEmptyBuf(Storage& bufptr) {
            // These two functions are copy/pasted from BufferLockFree.
            // If MAX_THREADS is large enough, this will always succeed :
            Item* start = &(*bufptr)[0];
            while( true ) {
                if ( oro_atomic_inc_and_test( &start->count ) )
                    break;
                oro_atomic_dec( &start->count );
                ++start;
                if (start == &(*bufptr)[0] + BufNum() )
                    start = &(*bufptr)[0]; // in case of races, rewind
            }
            assert( pointsTo(start, bufptr) );
            start->data.clear();
            return start; // unique pointer across all threads
        }

        /**
         * Item returned is guaranteed to point into bufptr.
         */
        Item* lockAndGetActive(Storage& bufptr) const {
            // This is a kind-of smart-pointer implementation
            // We could move it into Item itself and overload operator=
            Item* orig=0;
            do {
                if (orig)
                    oro_atomic_dec( &orig->count );
                bufptr = bufs;
                orig = active;
                // also check that orig points into bufptr.
                if ( pointsTo(orig, bufptr) )
                    oro_atomic_inc( &orig->count );
                else {
                    orig = 0;
                }
                // this synchronisation point is 'aggressive' (a _sufficient_ condition)
                // if active is still equal to orig, the increase of orig->count is
                // surely valid, since no contention (change of active) occured.
            } while ( active != orig );
            assert( pointsTo(orig, bufptr) );
            return orig;
        }

        /**
         * Only to be used by reserve() !
         * Caller must guarantee that active points to valid memory.
         */
        Item* lockAndGetActive() const {
            // only operates on active's refcount.
            Item* orig=0;
            do {
                if (orig)
                    oro_atomic_dec( &orig->count );
                orig = active;
                oro_atomic_inc( &orig->count );
                // this synchronisation point is 'aggressive' (a _sufficient_ condition)
                // if active is still equal to orig, the increase of orig->count is
                // surely valid, since no contention (change of active) occured.
            } while ( active != orig );
            return orig;
        }

        /**
         * Get the blank buffer or null if no apply_and_blank operation going on.
         */
        Item* lockAndGetBlank(Storage& bufptr) const {
            Item* orig=0;
            do {
                if (orig)
                    oro_atomic_dec( &orig->count );
                bufptr = bufs;
                orig = blankp;
                if (orig == 0)
                    return 0; // no blankp.
                // also check that orig points into bufptr.
                if ( pointsTo(orig, bufptr) )
                    oro_atomic_inc( &orig->count );
                else {
                    orig = 0;
                }
                // this synchronisation point is 'aggressive' (a _sufficient_ condition)
                // if active is still equal to orig, the increase of orig->count is
                // surely valid, since no contention (change of active) occured.
            } while ( blankp != orig );
            assert( pointsTo(orig, bufptr) );
            return orig;
        }

        inline bool pointsTo( Item* p, const Storage& bf ) const {
            return p >= &(*bf)[0] && p <= &(*bf)[ BufNum() - 1 ];
        }

    };
}

#endif
