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


#ifndef RTT_INTERNAL_LATECONSTREFERENCEDATASOURCE_HPP
#define RTT_INTERNAL_LATECONSTREFERENCEDATASOURCE_HPP

#include "DataSource.hpp"

namespace RTT
{
    namespace internal {
            /**
             * A DataSource which is used to manipulate a const reference to an
             * external value, by means of a pointer, which can be set after
             * the data source was created. It's the responsibility of the creator
             * of this object that the data source is not used before the pointer
             * is set using setPointer.
             * @param T The result data type of get().
             */
            template<typename T>
            class LateConstReferenceDataSource
                : public DataSource<T>
            {
                // a reference to a value_t
                const typename DataSource<T>::value_t* mptr;
            public:

                typedef boost::intrusive_ptr<LateConstReferenceDataSource<T> > shared_ptr;

                LateConstReferenceDataSource(const typename DataSource<T>::value_t* ptr = 0)
                :mptr(ptr) {}

                void setPointer(const typename AssignableDataSource<T>::value_t* ptr ) {
                    mptr = ptr;
                }

                void const* getRawDataConst()
                {
                    return mptr;
                }

                typename DataSource<T>::result_t get() const
                {
                    return *mptr;
                }

                typename DataSource<T>::result_t value() const
                {
                    return *mptr;
                }

                typename DataSource<T>::const_reference_t rvalue() const
                {
                    return *mptr;
                }

                virtual LateConstReferenceDataSource<T>* clone() const {
                    return new LateConstReferenceDataSource<T>( mptr );
                }

                virtual LateConstReferenceDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>&  ) const {
                    return const_cast<LateConstReferenceDataSource<T>* >(this);
                }
            };
    }
}
#endif

