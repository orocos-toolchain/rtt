#include "corelib/WriteCout.hpp"

#include <os/rtstreams.hpp>

namespace ORO_CoreLib
{
    
    int WriteCout::write( const char* buf, size_t length )
    {
        rt_std::cout << buf;
        // TODO: Check if cout DID write buf, and if not, return -1
        return length;
    }

    int WriteCout::put( char c)
    {
        rt_std::cout << c;
        return 0;
    }

    WriteCout::WriteCout()
    {
    }

    WriteCout::~WriteCout()
    {
    }

}

