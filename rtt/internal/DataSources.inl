/***************************************************************************
  tag: psoetens Thu May 4 14:08:47 2006 +0000 DataSources.inl

                        DataSources.inl -  description
                           -------------------
    begin                : Thu May 4 2006
    copyright            : (C) 2006 Peter Soetens
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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_CORELIB_DATASOURCES_INL
#define ORO_CORELIB_DATASOURCES_INL

#include "rtt/internal/DataSources.hpp"
#include "rtt/internal/DataSource.inl"

namespace RTT
{ namespace internal {

    template<typename T>
    ValueDataSource<T>::~ValueDataSource() {}

    template<typename T>
    ValueDataSource<T>::ValueDataSource( T data )
        : mdata( data )
    {
    }

    /**
     * Specialisation for std::string to keep capacity when clone() is called.
     */
    template<>
    ValueDataSource<std::string>::ValueDataSource(std::string t );

    template<typename T>
    ValueDataSource<T>::ValueDataSource( )
        : mdata()
    {
    }

    template<typename T>
    void ValueDataSource<T>::set( typename AssignableDataSource<T>::param_t t )
    {
        mdata = t;
    }

    /**
     * Specialisation for std::string to keep capacity when set( ... ) is called.
     */
    template<>
    void ValueDataSource<std::string>::set(  AssignableDataSource<std::string>::param_t t );

    template<typename T>
    ValueDataSource<T>* ValueDataSource<T>::clone() const
    {
        return new ValueDataSource<T>( mdata );
    }

    template<typename T>
    ValueDataSource<T>* ValueDataSource<T>::copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replace ) const {
        // if somehow a copy exists, return the copy, otherwise return this (see Attribute copy)
        if ( replace[this] != 0 ) {
            assert ( dynamic_cast<ValueDataSource<T>*>( replace[this] ) == static_cast<ValueDataSource<T>*>( replace[this] ) );
            return static_cast<ValueDataSource<T>*>( replace[this] );
        }
        // Other pieces in the code rely on insertion in the map :
        replace[this] = const_cast<ValueDataSource<T>*>(this);
        // return this instead of a copy.
        return const_cast<ValueDataSource<T>*>(this);
    }

    template<typename T>
    ConstantDataSource<T>::~ConstantDataSource() {}

    template<typename T>
    ConstantDataSource<T>::ConstantDataSource( T value )
        : mdata( value )
    {
    }

    template<typename T>
    ConstantDataSource<T>* ConstantDataSource<T>::clone() const
    {
        return new ConstantDataSource<T>(mdata);
    }

    template<typename T>
    ConstantDataSource<T>* ConstantDataSource<T>::copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
        // no copy needed, share this with all instances.
        return const_cast<ConstantDataSource<T>*>(this);
    }

    template<typename T>
    ConstReferenceDataSource<T>::~ConstReferenceDataSource() {}

    template<typename T>
    ConstReferenceDataSource<T>::ConstReferenceDataSource( typename DataSource<T>::const_reference_t ref )
        : mref( ref )
    {
    }

    template<typename T>
    ConstReferenceDataSource<T>* ConstReferenceDataSource<T>::clone() const
    {
        return new ConstReferenceDataSource<T>(mref);
    }

    template<typename T>
    ConstReferenceDataSource<T>* ConstReferenceDataSource<T>::copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
        return const_cast<ConstReferenceDataSource<T>*>(this); // no copy needed, data is outside.
    }

    template<typename T>
    ReferenceDataSource<T>::~ReferenceDataSource() {}

    template<typename T>
    ReferenceDataSource<T>::ReferenceDataSource( typename AssignableDataSource<T>::reference_t ref )
        : mptr( &ref )
    {
    }
    template<typename T>
    void ReferenceDataSource<T>::set( typename AssignableDataSource<T>::param_t t )
    {
        *mptr = t;
    }

    template<typename T>
    ReferenceDataSource<T>* ReferenceDataSource<T>::clone() const
    {
        return new ReferenceDataSource<T>(*mptr);
    }

    template<typename T>
    ReferenceDataSource<T>* ReferenceDataSource<T>::copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
        return const_cast<ReferenceDataSource<T>*>(this); // no copy needed, data is outside.
    }

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

        template< typename BoundT>
        UnboundDataSource<BoundT>::UnboundDataSource( typename BoundT::result_t data )
            : BoundT( data )
        {
        }

        template< typename BoundT>
        UnboundDataSource<BoundT>::UnboundDataSource( )
        {
        }

        template< typename BoundT>
        UnboundDataSource<BoundT>* UnboundDataSource<BoundT>::copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replace) const {
            if ( replace[this] != 0 )
                return static_cast<UnboundDataSource<BoundT>*>(replace[this]);
            replace[this] = new UnboundDataSource<BoundT>( this->get() );
            return static_cast<UnboundDataSource<BoundT>*>(replace[this]);
        }
    }
}

#endif
