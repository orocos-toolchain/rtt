#include "corelib/ComediLog.hpp"

int ComediLog::loglevel( int loglevel )
{
    return comedi_loglevel( loglevel );
}

void ComediLog::perror( const char* s )
{
    comedi_perror( s );
}

char* ComediLog::strerror( int errnum )
{
    return comedi_strerror( errnum );
}

/*int ComediLog::errno()
{
        return comedi_errno();
}*/

int ComediLog::fileno( comedi_t* it )
{
    return comedi_fileno( it );
}
