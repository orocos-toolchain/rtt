/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  BufferBase.cpp

                        BufferBase.cpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
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
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/



#include "BufferBase.hpp"
#include "../ConnPolicy.hpp"

using namespace RTT;
using namespace RTT::base;

BufferBase::~BufferBase() {}

BufferBase::Options::Options()
    : circular_(false)
    , max_threads_(2)
    , multiple_writers_(false)
    , multiple_readers_(false)
{}

BufferBase::Options::Options(bool circular)
    : circular_(circular)
    , max_threads_(2)
    , multiple_writers_(false)
    , multiple_readers_(false)
{}

BufferBase::Options::Options(const ConnPolicy &policy)
    : circular_(policy.type == ConnPolicy::CIRCULAR_BUFFER)
    , max_threads_(2)
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
