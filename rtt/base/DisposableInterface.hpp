/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  DisposableInterface.hpp

                        DisposableInterface.hpp -  description
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


#ifndef ORO_DISPOSABLEINTERFACE_HPP
#define ORO_DISPOSABLEINTERFACE_HPP

#include <boost/shared_ptr.hpp>

namespace RTT
{
    namespace base
    {
        /**
         * @brief An object that is executable and
         * is freed after execution.
         */
        class DisposableInterface
        {
        public:
            /**
             * Use this type for shared pointer storage of an
             * DisposableInterface object.
             */
            typedef boost::shared_ptr<DisposableInterface> shared_ptr;

            virtual ~DisposableInterface() {}

            /**
             * Execute functionality and free this object.
             * You may no longer
             * use this object after calling this method.
             */
            virtual void executeAndDispose() = 0;

            /**
             * Just free this object without executing it.
             */
            virtual void dispose() = 0;
        };
    }
}

#endif // EXECUTABLEINTERFACE_HPP
