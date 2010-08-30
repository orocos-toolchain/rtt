#ifndef _RTT_RT_STRING_HPP
#define _RTT_RT_STRING_HPP 1

#include "os/oro_allocator.hpp"
#include <string>

namespace RTT
{
    /// Real-time allocatable, dynamically-sized string
    typedef std::basic_string<char, std::char_traits<char>, RTT::os::rt_allocator<char> > rt_string;

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

#endif

