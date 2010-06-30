/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  DataObjectUnSync.hpp

                        DataObjectUnSync.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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



#ifndef CORELIB_DATAOBJECT_UNSYNC_HPP
#define CORELIB_DATAOBJECT_UNSYNC_HPP


#include "DataObjectInterface.hpp"

namespace RTT
{ namespace base {

    /**
     * @brief A class which provides unprotected (\b not thread-safe) access to one typed element of data.
     *
     * @ingroup Ports
     */
    template<class T>
    class DataObjectUnSync
        : public DataObjectInterface<T>
    {
        /**
         * One element of Data.
         */
        T data;
    public:
        /**
         * Construct a DataObjectUnSync by name.
         *
         * @param _name The name of this DataObject.
         */
        DataObjectUnSync( const T& initial_value = T() )
            : data(initial_value) {}

        /**
         * The type of the data.
         */
        typedef T DataType;

        virtual void Get( DataType& pull ) const { pull = data; }

        virtual DataType Get() const { DataType cache;  Get(cache); return cache; }

        virtual void Set( const DataType& push ) { data = push; }

        virtual void data_sample( const DataType& sample ) {
            Set(sample);
        }
    };
}}

#endif

