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

#include <boost/serialization/array.hpp>


namespace RTT
{
    namespace internal {

        /**
         * Wraps a C array such that we can return a C array
         * from a DataSource.
         * Inspired on boost::serialization::array.
         *
         * Default copy-constructible and assignable.
         * An assignment makes a 'shallow copy', all copies
         * refer to the same C array.
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

            carray() : m_t(0), m_element_count(0) {}

            /**
             * We are constructible from boost::serialization::array<T>
             * @param orig
             */
            carray( boost::serialization::array<T> const& orig)
            : m_t( orig.address() ), m_element_count( orig.count() ) {
                if (m_element_count == 0)
                    m_t = 0;
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

        private:
            value_type* m_t;
            std::size_t m_element_count;
        };
    }
}

#endif /* ORO_CARRAY_HPP_ */
