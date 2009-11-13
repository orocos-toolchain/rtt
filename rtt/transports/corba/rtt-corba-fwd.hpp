/***************************************************************************
  tag: Peter Soetens  Fri Oct 23 11:02:37 CEST 2009  rtt-corba-fwd.hpp

                        rtt-corba-fwd.hpp -  description
                           -------------------
    begin                : Fri October 23 2009
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


#ifndef ORO_RTT_corba_FWD_HPP
#define ORO_RTT_corba_FWD_HPP

namespace RTT {
    namespace corba {
        class CorbaDispatcher;
        class CorbaTypeTransporter;
        class RemoteInputPort;
        class RemoteOutputPort;
        struct ApplicationServer;
        struct IllegalServer;
        struct RemoteConnID;
        struct UpdatedCommand;
        template<class T>
        class CORBAAssignableExpression;
        template<class T>
        class CORBAExpression;
        template<class T>
        class CorbaTemplateProtocol;
        template<class T>
        struct CreateConstantHelper;
        template<class Type, class _CorbaType>
        struct AnyConversionHelper;
        template<class Type>
        struct AnyConversion;
        template<typename PortClass>
        class RemotePort;
        template<typename T>
        class RemoteChannelElement;
    }
    namespace detail {
        using namespace corba;
    }
}
#endif
