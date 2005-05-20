#ifndef ORO_COMEDI_THREAD_SCOPE_HPP
#define ORO_COMEDI_THREAD_SCOPE_HPP

#include <pkgconf/device_drivers_comedi.h>
#include "comedi/ComediDevice.hpp"
#include "comedi/ComediSubDeviceDOut.hpp"

namespace ORO_DeviceDriver
{

    /**
     * A ThreadScope implementatation for Comedi Digital Outputs.
     * Enable the Comedi Thread Scope in the configtool to use
     * a comedi device as a thread monitoring scope.
     * @see ParallelPort
     */
    class ComediThreadScope
        : public ComediDevice,
          public ComediSubDeviceDOut
    {
    public:
        ComediThreadScope()
            : ComediDevice( ORODAT_DEVICE_DRIVERS_COMEDI_THREAD_SCOPE_MINOR ),
              ComediSubDeviceDOut( this, ORODAT_DEVICE_DRIVERS_COMEDI_THREAD_SCOPE_SUBDEVICE )
        {}

        ComediThreadScope(const std::string& name)
            : ComediDevice( ORODAT_DEVICE_DRIVERS_COMEDI_THREAD_SCOPE_MINOR ),
              ComediSubDeviceDOut( this, name, ORODAT_DEVICE_DRIVERS_COMEDI_THREAD_SCOPE_SUBDEVICE )
        {}
    };

}

#endif
