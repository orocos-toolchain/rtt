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


#ifndef RTT_INTERNAL_CONSTANTDATASOURCE_HPP
#define RTT_INTERNAL_CONSTANTDATASOURCE_HPP

#include "DataSource.hpp"

namespace RTT
{
    namespace internal {

    /**
     * A DataSource which holds a constant value and
     * returns it in its get() method. It can not be changed after creation.
     * @param T Any type of data, except being a non-const reference.
     */
    template<typename T>
    class ConstantDataSource
        : public DataSource<T>
    {
        /**
         * Assure that mdata is const, such that T is forced
         * to not be a non-const reference.
         */
        typename boost::add_const<typename DataSource<T>::value_t>::type mdata;

    public:
        /**
         * Use shared_ptr.
         */
        ~ConstantDataSource();

        typedef boost::intrusive_ptr< ConstantDataSource<T> > shared_ptr;

        ConstantDataSource( T value );

        typename DataSource<T>::result_t get() const
		{
			return mdata;
		}

        typename DataSource<T>::result_t value() const
		{
			return mdata;
		}

        typename DataSource<T>::const_reference_t rvalue() const
        {
            return mdata;
        }

        virtual ConstantDataSource<T>* clone() const;

        virtual ConstantDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const;
    };

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

    }
}
#endif

