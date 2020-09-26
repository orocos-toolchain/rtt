/***************************************************************************
  tag: Peter Soetens Wed Apr 13 22:35:37 2011 +0200 DataObjectDataSource.hpp

                        DataObjectDataSource.hpp -  description
                           -------------------
    begin                : Wed Apr 13 2011
    copyright            : (C) 2011 Peter Soetens
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


#ifndef DATAOBJECTDATASOURCE_HPP_
#define DATAOBJECTDATASOURCE_HPP_

#include "rtt/internal/DataSource.hpp"
#include "rtt/base/DataObjectInterface.hpp"

namespace RTT
{
    namespace internal {
        /**
         * A DataSource which is used to read a DataObject.
         * When it returns a const reference, it returns a
         * reference to a copy, since we can't get a reference
         * to the internals of a DataObject.
         * @param T The result data type of get().
         */
        template<typename T>
        class DataObjectDataSource
            : public DataSource<T>
        {
            typename base::DataObjectInterface<T>::shared_ptr mobject;
            mutable T mcopy;
        public:
            typedef boost::intrusive_ptr<DataObjectDataSource<T> > shared_ptr;

            DataObjectDataSource(typename base::DataObjectInterface<T>::shared_ptr obj)
            : mobject(obj)
              {}

            virtual ~DataObjectDataSource() { }

            bool evaluate() const {
                mobject->Get( mcopy );
                return true;
            }

            typename DataSource<T>::result_t get() const
            {
                mobject->Get(mcopy);
                return mcopy;
            }

            typename DataSource<T>::result_t value() const
            {
                return mcopy;
            }

            typename DataSource<T>::const_reference_t rvalue() const
            {
                return mcopy;
            }

            virtual void reset() { }

            virtual DataObjectDataSource<T>* clone() const {
                return new DataObjectDataSource(mobject);
            }
            virtual DataObjectDataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
                return new DataObjectDataSource(mobject);
            }
        };
    }
}

#endif /* DATAOBJECTDATASOURCE_HPP_ */
