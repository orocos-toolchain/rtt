/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:21:19 CEST 2002  MultiVector.hpp 

                        MultiVector.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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
 

#ifndef MULTIVECTOR_HPP
#define MULTIVECTOR_HPP

#include "pkgconf/os.h"

#if OROINT_OS_STDIOSTREAM
#include <ostream>
#endif

#pragma interface

namespace ORO_CoreLib
{

    /**
     * @brief A static allocated Vector.
     *
     * The MultiVector is an n (defaults to 6) dimensional vector
     * of any type T, mostly used for holding primitive types.
     * Most common casting and operators are defined on this class.
     *
     * @param S
     *        The number of elements in the vector
     * @param T
     *        The type of all elements in the vector
     */
    template <unsigned S = 6, class T = double>
    struct MultiVector
    {
        /**
         * You can use DataType if you want to refer to 
         * a pointer holding S elements
         */
        typedef T DataType[ S ];

        /**
         * The data contained in the MultiVector. You can access
         * it freely.
         */
        DataType data;

        /**
         * Returns a pointer to the data.
         */
        operator const T* () const
        {
            return data;
        }

        /**
         * Returns a reference to the data.
         */
        operator T&()
        {
            return *data;
        }

        /**
         * Creates a vector with all elements initialized to <d>.
         *
         * @param d
         *        The value to be used for initialisation of all elements
         */
        MultiVector( const T d )
        {
            for ( unsigned int i = 0; i < S;++i )
                data[ i ] = d;
        }

        /**
         * Creates a vector with all elements initialized to zero
         */
        MultiVector()
        {
            for ( unsigned int i = 0; i < S;++i )
                data[ i ] = 0;
        }

        /**
         * Adds another MultiVector to this MultiVector
         *
         * @param d
         *        The other MultiVector to be used.
         */
        MultiVector& operator += ( const MultiVector& d )
        {
            for ( unsigned int i = 0; i < S; ++i )
                data[ i ] += d.data[ i ];

            return *this;
        }

        /**
         * Vector product of another MultiVector with this MultiVector
         *
         * @param d
         *        The other MultiVector to be used.
         */
        MultiVector& operator *= ( const MultiVector& d )
        {
            for ( unsigned int i = 0; i < S; ++i )
                data[ i ] *= d.data[ i ];

            return *this;
        }

        /**
         * Scalar product of a scalar with this MultiVector
         *
         * @param d
         *        The scalar
         */
        MultiVector& operator *= ( const T d )
        {
            for ( unsigned int i = 0; i < S; ++i )
                data[ i ] *= d;

            return *this;
        }

        /**
         * Returns the difference between this and another MultiVector
         *
         * @param d
         *        The other MultiVector to be used.
         */
        MultiVector operator - ( const MultiVector& d ) const
        {
            MultiVector tmp;

            for ( unsigned int i = 0; i < S; ++i )
                tmp.data[ i ] = data[ i ] - d.data[ i ];

            return tmp;
        }

        MultiVector operator - () const
        {
            MultiVector tmp;

            for ( unsigned int i = 0; i < S; ++i )
                tmp.data[ i ] = - data[ i ];

            return tmp;
        }

        /**
         * Returns the addition of this and another MultiVector
         *
         * @param d
         *        The other MultiVector to be used.
         */
        MultiVector operator + ( const MultiVector& d ) const
        {
            MultiVector tmp;

            for ( unsigned int i = 0; i < S; ++i )
                tmp[ i ] = data[ i ] + d.data[ i ];

            return tmp;
        }

        /**
         * Returns the vector product of this and another MultiVector
         *
         * @param d
         *        The other MultiVector to be used.
         */
        MultiVector operator * ( const MultiVector& d ) const
        {
            MultiVector tmp;

            for ( unsigned int i = 0; i < S; ++i )
                tmp[ i ] = data[ i ] * d.data[ i ];

            return tmp;
        }

        MultiVector operator / ( const T d ) const
        {
            MultiVector tmp;

            for ( unsigned int i = 0; i < S; ++i )
                tmp[ i ] = data[ i ] / d;

            return tmp;
        }

        /**
         * Returns the scalar product of this and a scalar value
         *
         * @param d
         *        The scalar
         */
        MultiVector operator * ( const T d ) const
        {
            MultiVector tmp;

            for ( unsigned int i = 0; i < S; ++i )
                tmp[ i ] = d * data[ i ];

            return tmp;
        }

        /** 
         * Assigns all elements of this MultiVector to all elements
         * of another MultiVector.
         *
         * @param d
         *        The other MultiVector to be used.
         */
        MultiVector& operator = ( const MultiVector& d )
        {
            for ( unsigned int i = 0; i < S; ++i )
                data[ i ] = d.data[ i ];

            return *this;
        }

        /*
         * This is just plain wrong since assignment with integer
         * would call this method.
         * Actually, it's the standard that's wrong for doing
         * int == pointer
         *
            MultiVector& operator = (const DataType d)
            {
        //        if (d != 0)
                    for (int i=0; i<S; ++i)
                        data[i] = d[i];
                return *this;
            }
         *
         */
        
        /**
         * Assigns a scalar value to all elements of this Multivector
         * and returns a reference of the result.
         *
         * @param d
         *        The scalar
         */
        MultiVector& operator = ( const T d )
        {
            for ( unsigned int i = 0; i < S; ++i )
                data[ i ] = d;

            return *this;
        }

        /**
         * Not checked index operator to the internal data
         * 
         * @param i
         *        The element to be accessed starting from zero.
         */
        T& operator[] ( int i )
        {
            return data[ i ];
        }
    };

    template<unsigned S, typename D>
    MultiVector<S,D> operator * ( const D d, const MultiVector<S,D>& v ) 
    {
        return v*d;
    }

    template<unsigned S, typename D>
    MultiVector<S,D> operator + ( const D d, const MultiVector<S,D>& v ) 
    {
        return v+d;
    }

    template<unsigned S, typename D>
    MultiVector<S,D> operator - ( const D d, const MultiVector<S,D>& v ) 
    {
        return v-d;
    }


    /**
     * A MultiVector consisting of 6 doubles
     */
    typedef MultiVector<6, double> Double6D;

    /**
     * A MultiVector consisting of 6 integers
     */
    typedef MultiVector<6, int> Int6D;

    /**
     * A MultiVector consisting of 6 booleans
     */
	typedef MultiVector<6, bool> Bool6D;
	
    /**
     * A MultiVector consisting of 6 longs
     */
    typedef MultiVector<6, long> Long6D;

    extern template class MultiVector<6, double>;
    extern template class MultiVector<6, int>;
    extern template class MultiVector<6, bool>;
    extern template class MultiVector<6, long>;

#if OROINT_OS_STDIOSTREAM

    /**
     * Output to standard C++ output stream
     */
    template <unsigned int S, class T>
    std::ostream &operator<<(std::ostream &os, MultiVector<S,T> &q) 
    { 
        os << "(";
        for (unsigned int i = 0; i < (S - 1) ; i++)
            os << q[i] << ", ";
        os << q[ S - 1 ] << ")";
        return os; 
    } 
#endif

}

#endif
