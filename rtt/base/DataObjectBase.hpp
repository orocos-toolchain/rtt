/***************************************************************************
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

#ifndef CORELIB_DATAOBJECTBASE_HPP
#define CORELIB_DATAOBJECTBASE_HPP

#include "../rtt-fwd.hpp"

namespace RTT
{ namespace base {

    /**
     * @brief Base class for all data object classes.
     *
     * @ingroup PortBuffers
     */
    class DataObjectBase
    {
    public:
        /**
         * A helper class to pass optional arguments to the constructor of \ref DataObjectLockFree<T>
         * in order to avoid ambiguity.
         */
        class Options {
        private:
            unsigned int max_threads_;
            bool multiple_writers_;
            bool multiple_readers_;

        public:
            Options();
            Options(const ConnPolicy &policy);
            Options(unsigned int max_threads);

            unsigned int max_threads() const { return max_threads_; }
            Options &max_threads(unsigned int value) { max_threads_ = value; return *this; }
            bool multiple_writers() const { return multiple_writers_; }
            Options &multiple_writers(bool value) { multiple_writers_ = value; return *this; }
            bool multiple_readers() const { return multiple_readers_; }
            Options &multiple_readers(bool value) { multiple_readers_ = value; return *this; }
        };

        virtual ~DataObjectBase() {}

        /**
         * Clears any data stored by this data object, so that any subsequent Get() without
         * a new Set() will return NoData.
         */
        virtual void clear() = 0;
    };
}}

#endif

