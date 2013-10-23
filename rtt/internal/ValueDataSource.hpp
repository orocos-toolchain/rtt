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


#ifndef ORO_CORELIB_VALUEDATASOURCE_HPP
#define ORO_CORELIB_VALUEDATASOURCE_HPP

#include "AssignableDataSource.hpp"
#include <string>

namespace RTT
{
    namespace internal {

    /**
     * A simple, yet very useful DataSource, which keeps a value, and
     * returns it in its get() method.
     * This is an AssignableDataSource, which
     * thus can be changed.
     * @param T The result data type of get().
     */
    template<typename T>
    class ValueDataSource
        : public AssignableDataSource<T>
    {
    protected:
        mutable typename DataSource<T>::value_t mdata;

    public:
        /**
         * Use shared_ptr.
         */
        ~ValueDataSource();

        typedef boost::intrusive_ptr<ValueDataSource<T> > shared_ptr;

        ValueDataSource( T data );

        ValueDataSource( );

        typename DataSource<T>::result_t get() const
		{
			return mdata;
		}

        typename DataSource<T>::result_t value() const
		{
			return mdata;
		}

        void set( typename AssignableDataSource<T>::param_t t );

        typename AssignableDataSource<T>::reference_t set()
		{
			return mdata;
		}

        typename AssignableDataSource<T>::const_reference_t rvalue() const
		{
			return mdata;
		}

        virtual ValueDataSource<T>* clone() const;

        virtual ValueDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replace ) const;
    };

    /**
     * Specialisation for std::string to keep capacity when set( ... ) is called.
     */
    template<>
    inline RTT_API void ValueDataSource<std::string>::set(  AssignableDataSource<std::string>::param_t t )
    {
        mdata = t.c_str();
    }

    /**
     * Specialisation for std::string to keep capacity when clone() is called.
     */
    template<>
    inline RTT_API ValueDataSource<std::string>::ValueDataSource(std::string t )
        : mdata( t.c_str() )
    {
    }

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
    void ValueDataSource<T>::set( typename AssignableDataSource<T>::param_t t )
    {
        mdata = t;
    }

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


    }
}

#endif

