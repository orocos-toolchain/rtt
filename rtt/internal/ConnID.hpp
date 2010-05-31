/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  ConnID.hpp

                        ConnID.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Sylvain Joyeux
    email                : sylvain.joyeux@m4x.org

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


/*
 * ConnID.hpp
 *
 *  Created on: Oct 14, 2009
 *      Author: kaltan
 */

#ifndef ORO_CONNID_HPP_
#define ORO_CONNID_HPP_

#include "../rtt-config.h"

namespace RTT
{
    namespace internal {
        /** This class is used in places where a permanent representation of a
         * reference to a connection is needed.
         *
         */
        class RTT_API ConnID
        {
        public:
            virtual bool isSameID(ConnID const& id) const = 0;
            virtual ConnID* clone() const = 0;
            virtual ~ConnID() {}
        };

        /**
         * A simplistic id that is only same with its own clones (and clones of clones).
         */
        class RTT_API SimpleConnID : public ConnID
        {
            const ConnID* cid;
        public:
            SimpleConnID(const ConnID* orig = 0);
            virtual bool isSameID(ConnID const& id) const;
            virtual ConnID* clone() const;
        };
    }
}
#endif /* ORO_CONNID_HPP_ */
