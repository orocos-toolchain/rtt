/***************************************************************************
  tag: Peter Soetens Mon Jun 28 15:46:27 2010 +0200 rtt-mqueue-fwd.hpp

                        rtt-mqueue-fwd.hpp -  description
                           -------------------
    begin                : Mon Jun 28 2010
    copyright            : (C) 2010 Peter Soetens
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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef ORO_RTT_mqueue_FWD_HPP
#define ORO_RTT_mqueue_FWD_HPP

namespace RTT {
    namespace mqueue {
        class Dispatcher;
        template<class T>
        class MQSerializationProtocol;
        template<class T>
        class MQTemplateProtocol;
        template<typename T>
        class MQChannelElement;
    }
    namespace detail {
        using namespace mqueue;
    }
}
#endif
