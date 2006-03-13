/***************************************************************************
  tag: Peter Soetens  Thu Mar 2 08:30:18 CET 2006  BufferDataSource.hpp 

                        BufferDataSource.hpp -  description
                           -------------------
    begin                : Thu March 02 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef ORO_EXECUTION_BUFFERDATASOURCE_HPP
#define ORO_EXECUTION_BUFFERDATASOURCE_HPP

#include <corelib/DataSource.hpp>
#include <corelib/BufferInterface.hpp>

namespace ORO_Execution
{
    /**
     * A DataSource which provides read-only access to
     * the next value to be read from the buffer, without
     * removing that value from the buffer.
     */
    template<class T>
    class BufferDataSource
        : public ORO_CoreLib::DataSource<T>
    {
        typename ORO_CoreLib::BufferInterface<T>::shared_ptr  mbuf;
    public:
        BufferDataSource(typename ORO_CoreLib::BufferInterface<T>::shared_ptr buf )
            : mbuf( buf )
        {
        }

        typename ORO_CoreLib::DataSource<T>::result_t get() const {
            return mbuf->front();
        }

        BufferDataSource<T>* clone() const {
            return new BufferDataSource<T>( mbuf );
        }

        BufferDataSource<T>* copy(std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& clones) {
            clones[this] = this;
            return this;
        }

    };
}

#endif
