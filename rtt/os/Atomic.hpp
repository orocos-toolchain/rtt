/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:04 CET 2004  Atomic.hpp 

                        Atomic.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
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
 
 
#ifndef OS_COMMON_ORO_ATOMIC_HPP
#define OS_COMMON_ORO_ATOMIC_HPP

#include "oro_arch.h"

namespace RTT
{ namespace os {
    /**
     * C++ abstraction of atomic integer operations.
     */
    class RTT_API AtomicInt
    {
        oro_atomic_t _val;
    public:
        AtomicInt( int value = 0 )
        {
            ORO_ATOMIC_SETUP( &_val, value);
        }

		AtomicInt(const AtomicInt& orig)
		{
			ORO_ATOMIC_SETUP( &_val, oro_atomic_read( &(orig._val) ) );
		}

        ~AtomicInt()
        {
            ORO_ATOMIC_CLEANUP( &_val );
        }

		const AtomicInt& operator=(const AtomicInt& orig)
		{
			oro_atomic_set( &_val, oro_atomic_read( &(orig._val)));
			return *this;
		}

        /**
         * Read the current value of the integer.
         */
        int read() const { return oro_atomic_read( &_val); }

        /**
         * Set the integer to a new value.
         */
        void set(int i) { oro_atomic_set( &_val, i ); }

        void add(int i) { oro_atomic_add( &_val, i ); }

        void sub(int i) { oro_atomic_sub( &_val, i ); }

        /** Subtract a value and test if the result is zero
         *
         * @return true if the atomic is zero after the decrement, and false
         *   otherwise
         */
        bool sub_and_test(int i) { return oro_atomic_sub_and_test( &_val, i) != 0; }

        void inc() { oro_atomic_inc( &_val ); }

        void dec() { oro_atomic_dec( &_val ); }

        /** Decrement and test if the result is zero
         *
         * @return true if the atomic is zero after the decrement, and false
         *   otherwise
         */
        bool dec_and_test() { return oro_atomic_dec_and_test( &_val ) != 0; }

        /** Increment and test if the result is zero
         *
         * @return true if the atomic is zero after the increment, and false
         *   otherwise
         */
        bool inc_and_test() { return oro_atomic_inc_and_test( &_val ) != 0; }
    };

}}

#endif
