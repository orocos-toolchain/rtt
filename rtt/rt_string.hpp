/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:19 CEST 2010  rt_string.hpp

                        rt_string.hpp -  description
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


#ifndef _RTT_RT_STRING_HPP
#define _RTT_RT_STRING_HPP 1

#include "rtt/os/oro_allocator.hpp"
#include "rtt/rtt-config.h"
#include <string>
#include <sstream>

namespace RTT
{
    /// Real-time allocatable, dynamically-sized string
    typedef std::basic_string<char, std::char_traits<char>, RTT::os::rt_allocator<char> > rt_string;

    /// Real-time allocatable, dynamically-size output string stream
    typedef std::basic_ostringstream<char, std::char_traits<char>, RTT::os::rt_allocator<char> > rt_ostringstream;

    //! convert from real-time string to std::string
    inline std::string makeString(const RTT::rt_string& str)
    {
        return std::string(str.c_str());
    }

    /** not provide other conversion, to prevent compiler automatically and silently
     using it. You must _explicitly_ convert with something like

     std::string s1("Hello world");
     OCL::String s2(s1.c_str());
     */
}

namespace std
{
    /**
     * Extern template declaration
     */
    RTT_EXT_IMPL template class basic_string<char, char_traits<char>, RTT::os::rt_allocator<char> >;
    RTT_EXT_IMPL template class basic_ostringstream<char, char_traits<char>, RTT::os::rt_allocator<char> >;
}

#endif

