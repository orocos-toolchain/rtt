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


#include "DataObjectBase.hpp"
#include "../ConnPolicy.hpp"

using namespace RTT;
using namespace RTT::base;

DataObjectBase::Options::Options()
    : max_threads_(2)
    , multiple_writers_(false)
    , multiple_readers_(false)
{}

DataObjectBase::Options::Options(unsigned int max_threads)
    : max_threads_(max_threads)
    , multiple_writers_(false)
    , multiple_readers_(true)
{}

DataObjectBase::Options::Options(const ConnPolicy &policy)
    : max_threads_(2)
    , multiple_writers_(policy.buffer_policy == PerInputPort || policy.buffer_policy == Shared)
    , multiple_readers_(policy.buffer_policy == PerOutputPort || policy.buffer_policy == Shared)
{
    if (policy.max_threads == 0) {
        if (multiple_writers_) {
            max_threads_ += 10;
        }
        if (multiple_readers_) {
            max_threads_ += 10;
        }
    } else {
        max_threads_ = policy.max_threads;
    }
}
