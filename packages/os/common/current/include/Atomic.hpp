/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:04 CET 2004  Atomic.hpp 

                        Atomic.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef ORO_OS_COMMON_ATOMIC_HPP
#define ORO_OS_COMMON_ATOMIC_HPP

#include "oro_atomic.hpp"

namespace ORO_OS
{
    /**
     * C++ abstraction of atomic integer operations.
     */
    class AtomicInt
    {
        atomic_t _val;
    public:
        AtomicInt( int value = 0 )
        {
            atomic_set(_val, value);
        }

        /**
         * Read the current value of the integer.
         */
        int read() { return atomic_read( &_val); }

        /**
         * Set the integer to a new value.
         */
        void set(int v) { atomic_set( &_val, v ); }
        /**
         * atomic_add - add integer to atomic variable
         * @i: integer value to add
         * @v: pointer of type atomic_t
         * 
         * Atomically adds @i to @v.  Note that the guaranteed useful range
         * of an atomic_t is only 24 bits.
         */
        void add(int v) { atomic_add( v, &_val ); }
        /**
         * atomic_sub - subtract the atomic variable
         * @i: integer value to subtract
         * @v: pointer of type atomic_t
         * 
         * Atomically subtracts @i from @v.  Note that the guaranteed
         * useful range of an atomic_t is only 24 bits.
         */
        void sub(int v) { atomic_sub( v, &_val ); }
        /**
         * atomic_sub_and_test - subtract value from variable and test result
         * @i: integer value to subtract
         * @v: pointer of type atomic_t
         * 
         * Atomically subtracts @i from @v and returns
         * true if the result is zero, or false for all
         * other cases.  Note that the guaranteed
         * useful range of an atomic_t is only 24 bits.
         */
        bool sub_and_test(int v) { return atomic_sub_and_test( v, &_val) != 0; }
        /**
         * atomic_inc - increment atomic variable
         * @v: pointer of type atomic_t
         * 
         * Atomically increments @v by 1.  Note that the guaranteed
         * useful range of an atomic_t is only 24 bits.
         */ 
        void inc() { atomic_inc( &_val ); }
        /**
         * atomic_dec - decrement atomic variable
         * @v: pointer of type atomic_t
         * 
         * Atomically decrements @v by 1.  Note that the guaranteed
         * useful range of an atomic_t is only 24 bits.
         */ 
        void dec() { atomic_dec( &_val ); }
        /**
         * atomic_dec_and_test - decrement and test
         * @v: pointer of type atomic_t
         * 
         * Atomically decrements @v by 1 and
         * returns true if the result is 0, or false for all other
         * cases.  Note that the guaranteed
         * useful range of an atomic_t is only 24 bits.
         */ 
        bool dec_and_test() { return atomic_dec_and_test( &_val ) != 0; }
        /**
         * atomic_inc_and_test - increment and test 
         * @v: pointer of type atomic_t
         * 
         * Atomically increments @v by 1
         * and returns true if the result is zero, or false for all
         * other cases.  Note that the guaranteed
         * useful range of an atomic_t is only 24 bits.
         */ 
        bool inc_and_test() { return atomic_inc_and_test( &_val ) != 0; }
        /**
         * atomic_add_negative - add and test if negative
         * @v: pointer of type atomic_t
         * @i: integer value to add
         * 
         * Atomically adds @i to @v and returns true
         * if the result is negative, or false when
         * result is greater than or equal to zero.  Note that the guaranteed
         * useful range of an atomic_t is only 24 bits.
         */ 
        bool add_negative() { return atomic_add_negative( &_val ) != 0; }
    };

}

#endif
