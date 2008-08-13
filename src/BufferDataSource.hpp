/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:18 CET 2006  BufferDataSource.hpp

                        BufferDataSource.hpp -  description
                           -------------------
    begin                : Thu March 02 2006
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


#ifndef ORO_EXECUTION_BUFFERDATASOURCE_HPP
#define ORO_EXECUTION_BUFFERDATASOURCE_HPP

#include "DataSource.hpp"
#include "BufferInterface.hpp"

namespace RTT
{
    /**
     * A DataSource which provides read-only access to
     * the next value to be read from the buffer, without
     * removing that value from the buffer.
     */
    template<class T>
    class BufferDataSource
        : public DataSource<T>
    {
        typename BufferInterface<T>::shared_ptr  mbuf;
    public:
        BufferDataSource(typename BufferInterface<T>::shared_ptr buf )
            : mbuf( buf )
        {
        }

        typename DataSource<T>::result_t get() const {
            return mbuf->front();
        }

        typename DataSource<T>::result_t value() const {
            return mbuf->front();
        }

        BufferDataSource<T>* clone() const {
            return new BufferDataSource<T>( mbuf );
        }

        BufferDataSource<T>* copy(std::map<const DataSourceBase*, DataSourceBase*>& clones) const {
            clones[this] = const_cast< BufferDataSource<T>*>(this);
            return const_cast< BufferDataSource<T>*>(this);
        }

    };
}

#endif
