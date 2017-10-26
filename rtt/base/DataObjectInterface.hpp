/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  DataObjectInterface.hpp

                        DataObjectInterface.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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

#ifndef CORELIB_DATAOBJECTINTERFACE_HPP
#define CORELIB_DATAOBJECTINTERFACE_HPP

#include "DataObjectBase.hpp"
#include "../FlowStatus.hpp"
#include <boost/call_traits.hpp>
#include <boost/shared_ptr.hpp>

namespace RTT
{ namespace base {


    /**
     * @brief A DataObjectInterface implements multi-threaded read/write solutions.
     *
     * @see DataObject
     * @param T The \a DataType which can be Get() or Set() with this DataObject.
     * @ingroup PortBuffers
     */
    template <class T>
    class DataObjectInterface : public DataObjectBase
    {
    public:
        typedef T value_t;
        typedef typename boost::call_traits<T>::param_type param_t;
        typedef typename boost::call_traits<T>::reference reference_t;

        /**
         * Used for shared_ptr management.
         */
        typedef typename boost::shared_ptr<DataObjectInterface<T> > shared_ptr;

        /**
         * Create a DataObject which is initially not reference counted.
         */
        DataObjectInterface() {}

        /**
         * Destructor.
         */
        virtual ~DataObjectInterface() {}

        /**
         * Get a copy of the Data of this data object.
         *
         * @param pull A copy of the data.
         * @param copy_old_data If true, also copy the data if the data object
         *                      has not been updated since the last call.
         */
        virtual FlowStatus Get( reference_t pull, bool copy_old_data = true ) const = 0;

        /**
         * Get a copy of the data of this data object.
         *
         * @return A copy of the data.
         */
        virtual value_t Get() const = 0;

        /**
         * Set the data to a certain value.
         *
         * @param push The data which must be set.
         */
        virtual bool Set( param_t push ) = 0;

        /**
         * Provides a data sample to initialize this data object.
         * As such enough storage
         * space can be allocated before the actual writing begins.
         *
         * @param sample the data sample
         * @param reset enforce reinitialization even if this operation clears the stored data.
         * @return true if the data object was successfully (re)initialized.
         */
        virtual bool data_sample( param_t sample, bool reset = true ) = 0;

        /**
         * Reads back a data sample.
         */
        virtual value_t data_sample() const = 0;

        /**
         * Clears any data stored by this data object, so that any subsequent Get() without
         * a new Set() will return NoData.
         */
        virtual void clear() = 0;
    };
}}

#endif

