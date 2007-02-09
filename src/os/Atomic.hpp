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

#include "oro_atomic.h"

namespace RTT
{ namespace OS {
    /**
     * C++ abstraction of atomic integer operations.
     */
    class AtomicInt
    {
        oro_atomic_t _val;
    public:
        AtomicInt( int value = 0 )
        {
            oro_atomic_set(&_val, value);
        }

        /**
         * Read the current value of the integer.
         */
        int read() { return oro_atomic_read( &_val); }

        /**
         * Set the integer to a new value.
         */
        void set(int i) { oro_atomic_set( &_val, i ); }
        /**
         * oro_atomic_add - add integer to atomic variable
         * @i: integer value to add
         * 
         * Atomically adds @i to @v.  Note that the guaranteed useful range
         * of an oro_atomic_t is only 24 bits.
         */
        void add(int i) { oro_atomic_add( i, &_val ); }
        /**
         * oro_atomic_sub - subtract the atomic variable
         * @i: integer value to subtract
         * 
         * Atomically subtracts @i from @v.  Note that the guaranteed
         * useful range of an oro_atomic_t is only 24 bits.
         */
        void sub(int i) { oro_atomic_sub( i, &_val ); }
        /**
         * oro_atomic_sub_and_test - subtract value from variable and test result
         * @i: integer value to subtract
         * 
         * Atomically subtracts @i from @v and returns
         * true if the result is zero, or false for all
         * other cases.  Note that the guaranteed
         * useful range of an oro_atomic_t is only 24 bits.
         */
        bool sub_and_test(int i) { return oro_atomic_sub_and_test( i, &_val) != 0; }
        /**
         * oro_atomic_inc - increment atomic variable
         * 
         * Atomically increments @v by 1.  Note that the guaranteed
         * useful range of an oro_atomic_t is only 24 bits.
         */ 
        void inc() { oro_atomic_inc( &_val ); }
        /**
         * oro_atomic_dec - decrement atomic variable
         * 
         * Atomically decrements @v by 1.  Note that the guaranteed
         * useful range of an oro_atomic_t is only 24 bits.
         */ 
        void dec() { oro_atomic_dec( &_val ); }
        /**
         * oro_atomic_dec_and_test - decrement and test
         * 
         * Atomically decrements @v by 1 and
         * returns true if the result is 0, or false for all other
         * cases.  Note that the guaranteed
         * useful range of an oro_atomic_t is only 24 bits.
         */ 
        bool dec_and_test() { return oro_atomic_dec_and_test( &_val ) != 0; }
        /**
         * oro_atomic_inc_and_test - increment and test 
         * 
         * Atomically increments @v by 1
         * and returns true if the result is zero, or false for all
         * other cases.  Note that the guaranteed
         * useful range of an oro_atomic_t is only 24 bits.
         */ 
        bool inc_and_test() { return oro_atomic_inc_and_test( &_val ) != 0; }
        /**
         * oro_atomic_add_negative - add and test if negative
         * @i: integer value to add
         * 
         * Atomically adds @i to @v and returns true
         * if the result is negative, or false when
         * result is greater than or equal to zero.  Note that the guaranteed
         * useful range of an oro_atomic_t is only 24 bits.
         */ 
        bool add_negative(int i) { return oro_atomic_add_negative(i, &_val ) != 0; }
    };

}}

#endif
