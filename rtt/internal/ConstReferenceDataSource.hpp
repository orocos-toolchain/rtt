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


#ifndef RTT_INTERNAL_CONSTREFERENCEDATASOURCE_HPP
#define RTT_INTERNAL_CONSTREFERENCEDATASOURCE_HPP

#include "DataSource.hpp"

namespace RTT
{
    namespace internal {
        /**
         * A DataSource which is used to read a const reference to an
         * external value.
         * @param T The result data type of get().
         */
        template<typename T>
        class ConstReferenceDataSource
            : public DataSource<T>
        {
            // a reference to a value_t
            typename DataSource<T>::const_reference_t mref;
        public:
            /**
             * Use shared_ptr.
             */
            ~ConstReferenceDataSource();

            typedef boost::intrusive_ptr<ConstReferenceDataSource<T> > shared_ptr;

            ConstReferenceDataSource( typename DataSource<T>::const_reference_t ref );

            typename DataSource<T>::result_t get() const
            {
                return mref;
            }

            typename DataSource<T>::result_t value() const
            {
                return mref;
            }

            typename DataSource<T>::const_reference_t rvalue() const
            {
                return mref;
            }

            virtual ConstReferenceDataSource<T>* clone() const;

            virtual ConstReferenceDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const;
        };

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

    }
}

#endif
