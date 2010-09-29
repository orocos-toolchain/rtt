/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  rtt-detail-fwd.hpp

                        rtt-detail-fwd.hpp -  description
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


#ifndef RTT_DETAIL_FWD_HPP
#define RTT_DETAIL_FWD_HPP

namespace RTT {
    /**
     * Base classes of RTT classes. Not to be used directly, but they
     * are part of the public API through inheritance.
     */
    namespace base {}
    /**
     * Classes which contain all implementation code for the RTT.
     */
    namespace internal {}
    /**
     * Extra classes not neccesary to build an Orocos application, but
     * are handy in some cases, or serve as an example on how to extend the RTT.
     */
    namespace extras {}
    /**
     * Classes for reading/writing properties to/from file or data stream.
     */
    namespace marsh {}
    /**
     * OS abstractions.
     */
    namespace os {}
    /**
     * Parser code for the Orocos real-time scripting language.
     */
    namespace scripting {}
    /**
     * Classes for typekits, i.e. describing and handling user data types.
     */
    namespace types {}
    /**
     * Classes and functions for creating and loading plugins.
     */
    namespace plugin {}
    /**
     * CORBA (OmniORB/TAO) code for network data transport.
     */
    namespace corba {}

    /**
     * Convenient short notation for every sub-namespace of RTT.
     *
     * The detail namespace contains all classes of each sub-namespace.
     * It is intended for use in the .cpp files of
     * the RTT (or any advanced user of it). The prefered workflow is
     * to include headers from various sub-packages of RTT \b and
     * include this header.  By writing 'using namespace detail;', all
     * the classes from the include headers become then visible in the
     * current namespace.
     */
    namespace detail {
        using namespace base;
        using namespace internal;
        using namespace extras;
        using namespace marsh;
        using namespace os;
        using namespace scripting;
        using namespace types;
        using namespace plugin;
        using namespace corba;
    }

}

#endif
