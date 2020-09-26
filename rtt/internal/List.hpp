/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  List.hpp

                        List.hpp -  description
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



#ifndef ORO_LIST_HPP_
#define ORO_LIST_HPP_

#include "rtt/rtt-config.h"

/**
 * @file List.hpp
 * We can't use typedefs since C++ doesn't allow it for
 * templated classes without specifying all the template
 * parameters.
 */
#if defined(OROBLD_OS_NO_ASM)
#include "rtt/internal/ListLocked.hpp"
#else
#include "rtt/internal/ListLockFree.hpp"
#endif

namespace RTT
{ namespace internal {

    /**
     * This object represents the default queue implementation used
     * by Orocos objects.
     */
    template< class T>
    class List
#if defined(OROBLD_OS_NO_ASM)
        : public ListLocked<T>
#else
        : public ListLockFree<T>
#endif
    {
    public:
        /**
         * Create a queue of fixed size.
         * Optionally, add the number of threads that may
         * concurrently access this queue.
         */
        List( int qsize, int t = ORONUM_OS_MAX_THREADS)
#if defined(OROBLD_OS_NO_ASM)
            : ListLocked<T>(qsize,t)
#else
            : ListLockFree<T>(qsize,t)
#endif
        {}
    };
}}


#endif /* LIST_HPP_ */
