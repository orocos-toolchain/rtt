/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  ArrayPartDataSource.hpp

                        ArrayPartDataSource.hpp -  description
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


#ifndef ORO_ARRAYPARTDATASOURCE_HPP_
#define ORO_ARRAYPARTDATASOURCE_HPP_

#include "DataSource.hpp"

namespace RTT
{
    namespace internal
    {
        /**
         * A DataSource which is used to manipulate a reference to a
         * part of a data source holding a C-style array of elements.
         *
         * This data source only works on a fixed reference in memory of
         * a given array. In case the array's place in memory
         * varies, you need the OffsetPartDataSource, which can access
         * an element at any offset from a given memory location.
         *
         * @param T The data type of an element.
         */
        template<typename T>
        class ArrayPartDataSource
        : public AssignableDataSource<T>
        {
            // a reference to a value_t
            typename AssignableDataSource<T>::value_t* mref;
            // [] index
            DataSource<unsigned int>::shared_ptr mindex;
            // parent data source, for updating after set().
            base::DataSourceBase::shared_ptr mparent;
            // safety: all indexes must be smaller than this.
            unsigned int mmax;
        public:
            ~ArrayPartDataSource() {}

            typedef boost::intrusive_ptr<ArrayPartDataSource<T> > shared_ptr;

            /**
             * The part is constructed from a reference to an existing array of elements and
             * an index for accessing the n'th element.
             * @param ref    Reference to the first element of the array
             * @param index  Datasource pointing to the index for use in operator[]
             * @param parent Parent data source for sending updated() messages. May be null.
             */
            ArrayPartDataSource( typename AssignableDataSource<T>::reference_t ref,
                                    DataSource<unsigned int>::shared_ptr index,
                                    base::DataSourceBase::shared_ptr parent, unsigned int max )
                : mref(&ref), mindex(index), mparent(parent), mmax(max)
            {
            }

            typename DataSource<T>::result_t get() const
            {
                unsigned int i = mindex->get();
                if (i >= mmax)
                    return internal::NA<T>::na();
                return mref[ i ];
            }

            typename DataSource<T>::result_t value() const
            {
                unsigned int i = mindex->get();
                if (i >= mmax)
                    return internal::NA<T>::na();
                return mref[ i ];
            }

            void set( typename AssignableDataSource<T>::param_t t )
            {
                unsigned int i = mindex->get();
                if (i >= mmax)
                    return;
                mref[ i ] = t;
                updated();
            }

            typename AssignableDataSource<T>::reference_t set()
            {
                unsigned int i = mindex->get();
                if (i >= mmax)
                    return internal::NA<typename AssignableDataSource<T>::reference_t>::na();
                return mref[ i ];
            }

            typename AssignableDataSource<T>::const_reference_t rvalue() const
            {
                unsigned int i = mindex->get();
                if (i >= mmax)
                    return internal::NA<typename AssignableDataSource<T>::const_reference_t>::na();
                return mref[ i ];
            }

            void updated() {
                if (mparent) mparent->updated();
            }

            virtual ArrayPartDataSource<T>* clone() const {
                return new ArrayPartDataSource<T>( *mref, mindex, mparent, mmax);
            }

            virtual ArrayPartDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replace ) const {
                // if somehow a copy exists, return the copy, otherwise return this (see Attribute copy)
                if ( replace[this] != 0 ) {
                    assert ( dynamic_cast<ArrayPartDataSource<T>*>( replace[this] ) == static_cast<ArrayPartDataSource<T>*>( replace[this] ) );
                    return static_cast<ArrayPartDataSource<T>*>( replace[this] );
                }
                replace[this] = new ArrayPartDataSource<T>(*mref, mindex->copy(replace), mparent->copy(replace), mmax);
                return static_cast<ArrayPartDataSource<T>*>(replace[this]);

            }
        };

    }
}

#endif /* ORO_PARTDATASOURCE_HPP_ */
