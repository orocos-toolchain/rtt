/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  DataObjectLocked.hpp

                        DataObjectLocked.hpp -  description
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

#ifndef CORELIB_DATAOBJECT_LOCKED_HPP
#define CORELIB_DATAOBJECT_LOCKED_HPP


#include "../os/MutexLock.hpp"
#include "DataObjectInterface.hpp"

namespace RTT
{ namespace base {

    /**
     * @brief A class which provides locked/protected access to one typed element of data.
     *
     * It allows multiple read/write requests using a single lock. This is the in any case
     * threadsafe implementation, and can be blocking in situations where you do not want
     * that.
     * @ingroup PortBuffers
     */
    template<class T>
    class DataObjectLocked
        : public DataObjectInterface<T>
    {
        typedef typename DataObjectInterface<T>::value_t value_t;
        typedef typename DataObjectInterface<T>::reference_t reference_t;
        typedef typename DataObjectInterface<T>::param_t param_t;

        mutable os::Mutex lock;

        /**
         * One element of Data.
         */
        value_t data;

        mutable FlowStatus status;
        bool initialized;

    public:
        /**
         * Construct an uninitialized DataObjectLocked.
         */
        DataObjectLocked()
            : data(), status(NoData), initialized(false) {}

        /**
         * Construct a DataObjectLocked with initial value.
         */
        DataObjectLocked( param_t initial_value )
            : data(initial_value), status(NoData), initialized(true) {}

        /**
         * The type of the data.
         */
        typedef T DataType;

        virtual FlowStatus Get( reference_t pull, bool copy_old_data = true ) const {
            os::MutexLock locker(lock);
            FlowStatus result = status;
            if (status == NewData) {
                pull = data;
                status = OldData;
            } else if ((status == OldData) && copy_old_data) {
                pull = data;
            }
            return result;
        }

        virtual value_t Get() const {
            DataType cache = DataType();
            Get(cache);
            return cache;
        }

        virtual bool Set( param_t push ) {
            os::MutexLock locker(lock);
            data = push;
            status = NewData;
            return true;
        }

        virtual bool data_sample( param_t sample, bool reset ) {
            os::MutexLock locker(lock);
            if (!initialized || reset) {
                data = sample;
                status = NoData;
                initialized = true;
                return true;
            } else {
                return initialized;
            }
        }

        /**
         * Reads back a data sample.
         */
        virtual value_t data_sample() const {
            os::MutexLock locker(lock);
            return data;
        }

        virtual void clear() {
            os::MutexLock locker(lock);
            status = NoData;
        }
    };
}}

#endif

