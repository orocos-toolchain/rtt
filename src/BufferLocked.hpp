/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  BufferLocked.hpp

                        BufferLocked.hpp -  description
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





#ifndef ORO_CORELIB_BUFFER_LOCKED_HPP
#define ORO_CORELIB_BUFFER_LOCKED_HPP

#include "os/Mutex.hpp"
#include "os/MutexLock.hpp"
#include "BufferInterface.hpp"

namespace RTT
{

    /**
     * Implements a very simple blocking threadsafe buffer, using mutexes (locks).
     *
     * @see BufferLockFree
     * @ingroup Ports
     */
    template<class T>
    class BufferLocked
        :public BufferInterface<T>
    {
    public:

        typedef typename ReadInterface<T>::reference_t reference_t;
        typedef typename WriteInterface<T>::param_t param_t;
        typedef typename BufferInterface<T>::size_type size_type;
        typedef T value_t;

        /**
         * Create a buffer of size \a size.
         */
        BufferLocked( size_type size, const T& initial_value = T() )
            : buf()
        {
            buf.resize(size, initial_value);
            buf.resize(0);
        }

        /**
         * Destructor
         */
        ~BufferLocked() {}

        bool Push( param_t item )
        {
            OS::MutexLock locker(lock);
            if ( buf.capacity() == buf.size() )
                return false;
            buf.push_back( item );
            return true;
        }

        size_type Push(const std::vector<T>& items)
        {
            OS::MutexLock locker(lock);
            typename std::vector<T>::const_iterator itl( items.begin() );
            while ( buf.size() != buf.capacity() && itl != items.end() ) {
                buf.push_back( *itl );
                ++itl;
            }
            return (itl - items.begin());

        }
        bool Pop( reference_t item )
        {
            OS::MutexLock locker(lock);
            if ( buf.empty() )
                return false;
            item = buf.front();
            buf.erase( buf.begin() );
            return true;
        }

        size_type Pop(std::vector<T>& items )
        {
            OS::MutexLock locker(lock);
            int quant = 0;
            while ( !buf.empty() ) {
                items.push_back( buf.front() );
                buf.erase( buf.begin() );
                ++quant;
            }
            return quant;
        }

        value_t front() const
        {
            OS::MutexLock locker(lock);
            value_t item = value_t();
            if ( !buf.empty() )
                item = buf.front();
            return item;
        }

        size_type capacity() const {
            OS::MutexLock locker(lock);
            return buf.capacity();
        }

        size_type size() const {
            OS::MutexLock locker(lock);
            return buf.size();
        }

        void clear() {
            OS::MutexLock locker(lock);
            buf.clear();
        }

        bool empty() const {
            return buf.empty();
        }

        bool full() const {
            OS::MutexLock locker(lock);
            return buf.size() ==  buf.capacity();
        }
    private:
        std::vector<T> buf;
        mutable OS::Mutex lock;

    };
}

#endif // BUFFERSIMPLE_HPP
