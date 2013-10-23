
/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  DataSources.hpp

                        DataSources.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

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


#ifndef RTT_INTERNAL_ARRAYDATASOURCE_HPP
#define RTT_INTERNAL_ARRAYDATASOURCE_HPP

#include "AssignableDataSource.hpp"

namespace RTT
{
    namespace internal {
    /**
     * A DataSource that holds a fixed size array,
     * using the types::carray class.
     * @param T A carray<U>
     */
    template<typename T>
    class ArrayDataSource
        : public AssignableDataSource<T>
    {
    protected:
        typename T::value_type* mdata;
        T marray;

    public:
        /**
         * Use shared_ptr.
         */
        ~ArrayDataSource();

        typedef boost::intrusive_ptr<ArrayDataSource<T> > shared_ptr;

        /**
         * Create and allocate an Array of a fixed size.
         * In case you create an empty/default ArrayDataSource, you can
         * assign it an array size later-on with newArray( size )
         */
        ArrayDataSource( std::size_t size = 0);

        /**
         * Creates an ArrayDataSource and initializes the array
         * with the contents of another carray.
         * A deep copy is made from odata.
         */
        ArrayDataSource( T const& odata );
        /**
         * Clears the array of this data source and creates a new one.
         * Note that all references to this array will become invalid
         * (types::carray may make shallow copies!) so only use this
         * if you are sure no other object has a reference to the
         * contained array.
         */
        void newArray( std::size_t size );

        typename DataSource<T>::result_t get() const
		{
			return marray;
		}

        typename DataSource<T>::result_t value() const
		{
			return marray;
		}

        void set( typename AssignableDataSource<T>::param_t t );

        typename AssignableDataSource<T>::reference_t set()
		{
			return marray;
		}

        typename AssignableDataSource<T>::const_reference_t rvalue() const
		{
			return marray;
		}

        virtual ArrayDataSource<T>* clone() const;

        virtual ArrayDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replace ) const;
    };

    template<typename T>
    ArrayDataSource<T>::~ArrayDataSource() { delete[] mdata; }

    template<typename T>
    ArrayDataSource<T>::ArrayDataSource( std::size_t size )
        : mdata(size ? new typename T::value_type[size] : 0 ), marray(mdata,size)
    {
    }

    template<typename T>
    ArrayDataSource<T>::ArrayDataSource( T const& oarray )
        : mdata( oarray.count() ? new typename T::value_type[oarray.count()] : 0 ), marray(mdata, oarray.count())
    {
        marray = oarray; // deep copy!
    }

    template<typename T>
    void ArrayDataSource<T>::newArray( std::size_t size )
    {
        delete[] mdata;
        mdata = size ? new typename T::value_type[size] : 0;
        for(std::size_t i=0; i!= size; ++i) mdata[i] = typename T::value_type();
        marray.init(mdata,size);
    }

    template<typename T>
    void ArrayDataSource<T>::set( typename AssignableDataSource<T>::param_t t )
    {
        // makes a deep copy !
        marray = t;
    }

    template<typename T>
    ArrayDataSource<T>* ArrayDataSource<T>::clone() const
    {
        ArrayDataSource<T>* ret = new ArrayDataSource<T>( marray.count() );
        ret->set( marray );
        return ret;
    }

    template<typename T>
    ArrayDataSource<T>* ArrayDataSource<T>::copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replace ) const {
        // if somehow a copy exists, return the copy, otherwise return this (see Attribute copy)
        if ( replace[this] != 0 ) {
            assert ( dynamic_cast<ArrayDataSource<T>*>( replace[this] ) == static_cast<ArrayDataSource<T>*>( replace[this] ) );
            return static_cast<ArrayDataSource<T>*>( replace[this] );
        }
        // Other pieces in the code rely on insertion in the map :
        replace[this] = const_cast<ArrayDataSource<T>*>(this);
        // return this instead of a copy.
        return const_cast<ArrayDataSource<T>*>(this);
    }

    }
}
#endif

