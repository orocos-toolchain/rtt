/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  carray.hpp

                        carray.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#ifndef ORO_CARRAY_HPP_
#define ORO_CARRAY_HPP_

#include <boost/version.hpp>
#include <boost/serialization/array.hpp>
#include <boost/array.hpp>

namespace RTT
{
    namespace types {

        /**
         * Wraps a C array such that we can return a C array
         * from a DataSource.
         * Inspired on boost::serialization::array.
         *
         * Default copy-constructible (shallow copy).
         *
         * An assignment (operator=) makes a 'deep copy', while copy
         * construction makes a 'shallow copy', where the copy
         * refers to the same C array. This was chosen in the spirit
         * of this class, where it keeps track of the original data,
         * but when assigned from another carray, is meant as copying
         * the data itself.
         *
         * You can also point to parts of arrays. An uninitialized
         * carray object returns null for both count() and address().
         *
         * @param T The data type of the array.
         */
        template<class T>
        class carray
        {
        public:
            typedef T value_type;

            /**
             * Create an C array wrapper.
             * @param t Pointer to first element of array.
             * @param s Length of array. If zero, the parameter \a t
             * will be ignored.
             */
            carray(value_type* t, std::size_t s) :
                m_t( s ? t : 0),
                m_element_count(s)
            {}

            /**
             * Creates an empty carray.
             * You are not allowed to read or write (operator=)
             * to this array until it has been initialized.
             * @see init() in order to initialize it later on.
             */
            carray() : m_t(0), m_element_count(0) {}

            /**
             * We are constructible from boost::serialization::array<T>
             * Makes a shallow copy in order to keep the reference to
             * the original data.
             * @param orig
             */
#if BOOST_VERSION >= 106100
            carray( boost::serialization::array_wrapper<T> const& orig)
#else
            carray( boost::serialization::array<T> const& orig)
#endif
            : m_t( orig.address() ), m_element_count( orig.count() ) {
                if (m_element_count == 0)
                    m_t = 0;
            }

            /**
             * We are constructible from boost::array<T,N>
             * Makes a shallow copy in order to keep the reference to
             * the original data.
             * @param orig
             */
            template<std::size_t N>
            carray( boost::array<T,N> & orig)
            : m_t( orig.c_array() ), m_element_count( N ) {
                if (m_element_count == 0)
                    m_t = 0;
            }

            /**
             * (Re-)initialize this carray to a new address and size.
             */
            void init(value_type* t, std::size_t s) {
                m_t = s ? t : 0;
                m_element_count = s;
            }

            /**
             * The address of the first element of the array
             * @return null if count() == 0, the address otherwise.
             */
            value_type* address() const
            {
              return m_t;
            }

            /**
             * @return The number of elements in the array.
             */
            std::size_t count() const
            {
              return m_element_count;
            }

            /**
             * Assignment only copies max(this->count(), orig.count()) elements
             * from orig to this object. If orig.count() is smaller than this->count()
             * the contents of the remaining elements is left unmodified. If it's greater,
             * the excess elements are ignored.
             */
            template <class OtherT>
            const carray<T>& operator=( const carray<OtherT>& orig ) {
                if (&orig != this)
                    for(std::size_t i = 0; i != orig.count() && i != count(); ++i)
                        m_t[i] = orig.address()[i];
                return *this;
            }

            /**
             * Assignment only copies max(this->count(), orig.count()) elements
             * from orig to this object. If orig.count() is smaller than this->count()
             * the contents of the remaining elements is left unmodified. If it's greater,
             * the excess elements are ignored.
             * @param orig
             */
            template <class OtherT>
#if BOOST_VERSION >= 106100
            const carray<T>& operator=( boost::serialization::array_wrapper<OtherT> const& orig ) {
#else
            const carray<T>& operator=( boost::serialization::array<OtherT> const& orig ) {
#endif
                if (orig.address() != m_t)
                    for(std::size_t i = 0; i != orig.count() && i != count(); ++i)
                        m_t[i] = orig.address()[i];
                return *this;
            }

            /**
             * Assignment only copies max(this->count(), orig.size()) elements
             * from orig to this object. If orig.size() is smaller than this->count()
             * the contents of the remaining elements is left unmodified. If it's greater,
             * the excess elements are ignored.
             * @param orig
             */
            template <class OtherT, std::size_t OtherN>
            const carray<T>& operator=( const boost::array<OtherT,OtherN>& orig ) {
                if (orig.data() != m_t)
                    for(std::size_t i = 0; i != orig.size() && i != count(); ++i)
                        m_t[i] = orig[i];
                return *this;
            }

        private:
            value_type* m_t;
            std::size_t m_element_count;
        };
    }
}

#endif /* ORO_CARRAY_HPP_ */
