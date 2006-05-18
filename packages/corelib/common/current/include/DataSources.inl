#ifndef ORO_CORELIB_DATASOURCES_INL
#define ORO_CORELIB_DATASOURCES_INL

#include "DataSources.hpp"
#include "DataSource.inl"

namespace ORO_CoreLib
{
    template<typename T>
    ValueDataSource<T>::~ValueDataSource() {}

    template<typename T>
    ValueDataSource<T>::ValueDataSource( T data )
        : mdata( data )
    {
    }

    template<typename T>
    ValueDataSource<T>::ValueDataSource( )
        : mdata()
    {
    }

    template<typename T>
    typename DataSource<T>::result_t ValueDataSource<T>::value() const
    {
        return mdata;
    }

    template<typename T>
    typename DataSource<T>::result_t ValueDataSource<T>::get() const
    {
        return mdata;
    }

    template<typename T>
    void ValueDataSource<T>::set( typename AssignableDataSource<T>::param_t t )
    {
        mdata = t;
    }

    template<typename T>
    typename AssignableDataSource<T>::reference_t ValueDataSource<T>::set() {
        return mdata;
    }

    template<typename T>
    typename AssignableDataSource<T>::const_reference_t ValueDataSource<T>::rvalue() const {
        return mdata;
    }

    template<typename T>
    ValueDataSource<T>* ValueDataSource<T>::clone() const
    {
        return new ValueDataSource<T>( mdata );
    }

    template<typename T>
    ValueDataSource<T>* ValueDataSource<T>::copy( std::map<const DataSourceBase*, DataSourceBase*>& replace ) const {
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
    typename DataSource<T>::result_t ConstantDataSource<T>::get() const
    {
        return mdata;
    }

    template<typename T>
    typename DataSource<T>::result_t ConstantDataSource<T>::value() const
    {
        return mdata;
    }

    template<typename T>
    ConstantDataSource<T>* ConstantDataSource<T>::clone() const
    {
        return new ConstantDataSource<T>(mdata);
    }

    template<typename T>
    ConstantDataSource<T>* ConstantDataSource<T>::copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
        // no copy needed, share this with all instances.
        return const_cast<ConstantDataSource<T>*>(this);
    }

    template<typename T>
    ReferenceDataSource<T>::~ReferenceDataSource() {}

    template<typename T>
    ReferenceDataSource<T>::ReferenceDataSource( typename AssignableDataSource<T>::reference_t ref )
        : mref( ref )
    {
    }

    template<typename T>
    typename DataSource<T>::result_t ReferenceDataSource<T>::get() const
    {
        return mref;
    }

    template<typename T>
    typename DataSource<T>::result_t ReferenceDataSource<T>::value() const
    {
        return mref;
    }

    template<typename T>
    void ReferenceDataSource<T>::set( typename AssignableDataSource<T>::param_t t )
    {
        mref = t;
    }

    template<typename T>
    typename AssignableDataSource<T>::reference_t ReferenceDataSource<T>::set() {
        return mref;
    }

    template<typename T>
    typename AssignableDataSource<T>::const_reference_t ReferenceDataSource<T>::rvalue() const {
        return mref;
    }

    template<typename T>
    ReferenceDataSource<T>* ReferenceDataSource<T>::clone() const
    {
        return new ReferenceDataSource<T>(mref);
    }

    template<typename T>
    ReferenceDataSource<T>* ReferenceDataSource<T>::copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
        return const_cast<ReferenceDataSource<T>*>(this); // no copy needed, data is outside.
    }

	namespace detail {

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
        UnboundDataSource<BoundT>* UnboundDataSource<BoundT>::copy( std::map<const DataSourceBase*, DataSourceBase*>& replace) const {
            if ( replace[this] != 0 )
                return static_cast<UnboundDataSource<BoundT>*>(replace[this]);
            replace[this] = new UnboundDataSource<BoundT>( this->get() );
            return static_cast<UnboundDataSource<BoundT>*>(replace[this]);
        }
    }
}

#endif
