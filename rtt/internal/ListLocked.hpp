/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  ListLocked.hpp

                        ListLocked.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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



#ifndef ORO_LIST_LOCKED_HPP
#define ORO_LIST_LOCKED_HPP

#include <boost/intrusive/list.hpp>
#include <boost/bind.hpp>
#include <boost/bind/protect.hpp>
#include <stack>
#include <vector>
#include <algorithm>
#include "rtt/os/Mutex.hpp"
#include "rtt/os/MutexLock.hpp"

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{ namespace internal {

    /**
     * A \a simple lock-based list implementation to \a append or \a erase
     * data of type \a T.
     *
     * @note The mutex used by this implementation is recursive in order
     * to allow apply() to call apply() recursively. Erasing self (using clear() or erase()) from within apply()
     * is not supported and may lead to abnormal program termination.
     *
     * @param T The value type to be stored in the list.
     * Example : ListLocked<A> is a list which holds values of type A.
     * @ingroup CoreLibBuffers
     */
    template< class T>
    class ListLocked
    {
    public:

        typedef T value_t;
    private:
        struct Cont :  public boost::intrusive::list_base_hook<> {
            T data;
        };
        typedef boost::intrusive::list<Cont> BufferType;
        typedef std::stack<Cont*> StackType;
        typedef typename BufferType::iterator Iterator;
        typedef typename BufferType::const_iterator CIterator;

        BufferType mlist;
        StackType  mreserved;
        unsigned int required;

        mutable os::MutexRecursive m;
    public:
        /**
         * Create a lock-based list wich can store \a lsize elements.
         * @param lsize the initial capacity of the list.
         */
        ListLocked(unsigned int lsize, unsigned int unused = 0 )
            :required(lsize)
        {
            for(unsigned int i=0; i <lsize; ++i)
                mreserved.push(new Cont());
        }

        ~ListLocked() {
            this->clear();
            while( !mreserved.empty() ) {
                delete mreserved.top();
                mreserved.pop();
            }
        }

        size_t capacity() const
        {
            os::MutexLock lock(m);
            return mreserved.size() + mlist.size();
        }

        size_t size() const
        {
            os::MutexLock lock(m);
            return mlist.size();
        }

        bool empty() const
        {
            os::MutexLock lock(m);
            return mlist.empty();
        }

        /**
         * Grow the capacity to contain at least n additional items.
         * This method tries to avoid too much re-allocations, by
         * growing a bit more than required every N invocations and
         * growing nothing in between.
         * @param items The number of items to at least additionally reserve.
         */
        void grow(size_t items = 1) {
            os::MutexLock lock(m);
            required += items;
            if (required > mreserved.size() + mlist.size() ) {
                while ( mreserved.size() + mlist.size() < required * 2) {
                    mreserved.push( new Cont() );
                }
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
            os::MutexLock lock(m);
            required -= items;
        }

        /**
         * Reserve a capacity for this list.
         * @param lsize the \a minimal number of items this list will be
         * able to hold. Will not drop below the current capacity()
         * and this method will do nothing if \a lsize < this->capacity().
         */
        void reserve(size_t lsize)
        {
            os::MutexLock lock(m);
            while ( mreserved.size() + mlist.size() < lsize) {
                mreserved.push( new Cont() );
            }
        }

        void clear()
        {
            os::MutexLock lock(m);
            mlist.clear_and_dispose( boost::bind(&ListLocked::give_back, this, _1) );
        }

        /**
         * Append a single value to the list.
         * @param d the value to write
         * @return false if the list is full.
         */
        bool append( value_t item )
        {
            os::MutexLock lock(m);
            if ( mreserved.empty() )
                return false;
            mlist.push_back( this->get_item(item) );
            return true;
        }

        /**
         * Returns the first element of the list.
         */
        value_t front() const
        {
            os::MutexLock lock(m);
            return mlist.front().data;
        }

        /**
         * Returns the last element of the list.
         */
        value_t back() const
        {
            os::MutexLock lock(m);
            return mlist.back().data;
        }

        /**
         * Append a sequence of values to the list.
         * @param items the values to append.
         * @return the number of values written (may be less than items.size())
         */
        size_t append(const std::vector<T>& items)
        {
            os::MutexLock lock(m);
            unsigned int max = mreserved.size();
            typename std::vector<T>::const_iterator it = items.begin();
            for(; it != items.end() && max != 0; ++it, --max )
                mlist.push_back( this->get_item(*it) );
            return it - items.begin();
        }


        /**
         * Erase a value from the list.
         * @param item is to be erased from the list.
         * @return true if found and erased.
         */
        bool erase( value_t item )
        {
            os::MutexLock lock(m);
            mlist.remove_and_dispose_if( boost::bind(&ListLocked::is_item, this, item, _1), boost::bind(&ListLocked::give_back, this, _1) );
            return true;
        }

        /**
         * Erase a value from the list.
         * @param function each elements for which pred returns true are removed
         * @return true if at least one element has been removed
         * @note This function is only real-time if the destructor and copy-constructor of
         * of \a T is real-time.
         */
        template<typename Pred>
        bool delete_if(Pred pred)
        {
            os::MutexLock lock(m);
            bool deleted = false;

            typename BufferType::iterator cur(mlist.begin());
            typename BufferType::iterator last(mlist.end());

            while(cur != last) 
            {
            if(pred(cur->data))
            {
                cur = mlist.erase_and_dispose(cur, boost::bind(&ListLocked::give_back, this, _1) );
                deleted = true;
            }
            else
                ++cur;
            }

            return deleted;
        }

        /**
         * Apply a function to the elements of the whole list.
         * @param func The function to apply.
         * @note func may not call \a clear() or \a erase() on the current element
         * of this list.
         */
        template<class Function>
        void apply(Function func )
        {
            os::MutexLock lock(m);
            // A free beer for the one that can express this with a for_each construct.
            for (Iterator it = mlist.begin(); it != mlist.end(); ++it)
                func( it->data );
        }

        /**
         * Find an item in the list such that func( item ) == true.
         * @param blank The value to return if not found.
         * @return The item that matches func(item) or blank if none matches.
         */
        template<class Function>
        value_t find_if( Function func, value_t blank = value_t() )
        {
            os::MutexLock lock(m);
            Iterator it = std::find_if(mlist.begin(), mlist.end(), boost::bind(func, boost::bind(&ListLocked::get_data, this,_1)));
            if (it != mlist.end() )
                return it->data;
            return blank;
        }
    private:
        /**
         * This is a dispose function for remove_and_dispose_if
         * It takes a pointer as argument.
         * @param cont
         */
        void give_back(Cont* cont)
        {
            mreserved.push( cont );
        }

        Cont& get_item(value_t item)
        {
            Cont* c = mreserved.top();
            mreserved.pop();
            c->data = item;
            return *c;
        }

        value_t& get_data(Cont& c)
        {
            return c.data;
        }

        /**
         * This is a predicate function for remove_and_dispose_if
         * It takes a reference as argument.
         * @param item
         * @param cont
         * @return
         */
        bool is_item(value_t item, const Cont& cont)
        {
            return item == cont.data;
        }
    };

}}

#endif
