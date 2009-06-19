
#ifndef ORO_LIST_LOCKED_HPP
#define ORO_LIST_LOCKED_HPP

#include <boost/intrusive/list.hpp>
#include <boost/bind.hpp>
#include <boost/bind/protect.hpp>
#include <stack>
#include <vector>
#include <algorithm>
#include "os/Mutex.hpp"
#include "os/MutexLock.hpp"

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{
    /**
     * A \a simple lock-based list implementation to \a append or \a erase
     * data of type \a T.
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
        int required;

        mutable OS::Mutex m;
    public:
        /**
         * Create a lock-based list wich can store \a lsize elements.
         * @param lsize the initial capacity of the list.
'        */
        ListLocked(unsigned int lsize, unsigned int unused = 0 )
            :required(lsize)
        {
            for(int i=0; i <lsize; ++i)
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
            OS::MutexLock lock(m);
            return mreserved.size() + mlist.size();
        }

        size_t size() const
        {
            OS::MutexLock lock(m);
            return mlist.size();
        }

        bool empty() const
        {
            OS::MutexLock lock(m);
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
            OS::MutexLock lock(m);
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
            OS::MutexLock lock(m);
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
            OS::MutexLock lock(m);
            while ( mreserved.size() + mlist.size() < lsize) {
                mreserved.push( new Cont() );
            }
        }

        void clear()
        {
            OS::MutexLock lock(m);
            mlist.clear_and_dispose( boost::bind(&ListLocked::give_back, this, _1) );
        }

        /**
         * Append a single value to the list.
         * @param d the value to write
         * @return false if the list is full.
         */
        bool append( value_t item )
        {
            OS::MutexLock lock(m);
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
            OS::MutexLock lock(m);
            return mlist.front().data;
        }

        /**
         * Returns the last element of the list.
         */
        value_t back() const
        {
            OS::MutexLock lock(m);
            return mlist.back().data;
        }

        /**
         * Append a sequence of values to the list.
         * @param items the values to append.
         * @return the number of values written (may be less than items.size())
         */
        size_t append(const std::vector<T>& items)
        {
            OS::MutexLock lock(m);
            int max = mreserved.size();
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
            OS::MutexLock lock(m);
            mlist.remove_and_dispose_if( boost::bind(&ListLocked::is_item, this, item, _1), boost::bind(&ListLocked::give_back, this, _1) );
            return true;
        }

        /**
         * Apply a function to the elements of the whole list.
         * @param func The function to apply.
         */
        template<class Function>
        void apply(Function func )
        {
            OS::MutexLock lock(m);
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
            OS::MutexLock lock(m);
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

}

#endif
