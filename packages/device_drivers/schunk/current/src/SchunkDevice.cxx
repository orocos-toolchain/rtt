
#include "corelib/SchunkDevice.hpp"

namespace CBDeviceDriver
{

    SchunkDevice::SchunkDevice( SchunkCommunicationInterface *comm )
            : _comm( comm )
    {
        initializeSchunk();
    }

    /*    SchunkDevice::~SchunkDevice()
        {
            //XXX destructor van _comm oproepen
        } */

    void SchunkDevice::flushInput()
    {
        _comm->flushInput();
    }

    int SchunkDevice::read( SchunkData &data )
    {
        return getLine( &data );
    }

    int SchunkDevice::getLine( SchunkData *data )
    {
        state->getLine( data );
        return 0; // TODO make void
    }

    void SchunkDevice::initializeSchunk()
    {
        state = new SchunkStateBRL3f( this );
        state->imposeState();
    }
}
