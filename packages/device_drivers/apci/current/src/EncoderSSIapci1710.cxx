/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:23:37 CEST 2002  EncoderSSI.cpp 

                       EncoderSSI.cpp -  description
                          -------------------
   begin                : Mon June 10 2002
   copyright            : (C) 2002 Peter Soetens
   email                : peter.soetens@mech.kuleuven.ac.be

***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/


#include <pkgconf/system.h>
#include <pkgconf/device_drivers_apci.h>
#include <device_drivers/EncoderSSIapci1710.hpp>

#include <iostream>

#ifdef OROPKG_CORELIB_TIMING
#include <corelib/HeartBeatGenerator.hpp>
#endif

// include lib first, if not present, include KM headers.
#if defined (OROINT_DEVICE_DRIVERS_APCI1710_SSI) && defined(OROPKG_OS_LXRT)
#include "apci_lxrt.h"
#elif defined (OROBLD_DEVICE_DRIVERS_APCI1710_SSI_KM)
    extern "C"
    {
        #define new _new
        #define class _class
        #define LINUX_PCI_H
            #include "apci1710/apci1710.h"
            #include "apci1710/ssi.h"
        #undef class
        #undef new
    }
#endif

namespace CBDeviceDriver
{
    using std::endl;
    using ORO_CoreLib::HeartBeatGenerator;

    EncoderSSI::EncoderSSI( unsigned int mNr, const std::string& name ) :
#ifdef OROPKG_CORELIB_TASKS
        TaskNonPreemptible( 1./ORONUM_DEVICE_DRIVERS_APCI1710_SSI_UPDATE ),
#endif
        moduleNr( mNr ), positions( buf1 ), store( buf2 )
    {
#ifdef OROINT_DEVICE_DRIVERS_APCI1710_SSI
        std::cout << "Creating EncoderSSI..."<<endl;
        dev = apci1710_get_device();
        int res=0;
        if ( 0 != (res = apci1710_ssi_init( dev, moduleNr,
                                            ORONUM_DEVICE_DRIVERS_APCI1710_SSI_PROFILE,
                                            ORONUM_DEVICE_DRIVERS_APCI1710_SSI_POSITION_BITS,
                                            ORONUM_DEVICE_DRIVERS_APCI1710_SSI_TURN_BITS,
                                            COUNTINGMODE_BINARY,
                                            ORONUM_DEVICE_DRIVERS_APCI1710_SSI_FREQ ) ) ) // in Hz(25kHz is ok)
            std::cout << "\nError "<< res <<" : Module "<<moduleNr<<" is not ready for SSI" <<endl<<endl;

#ifdef OROPKG_CORELIB_TIMING
        // init one buffer, display some statistics
        HeartBeatGenerator::ticks t=HeartBeatGenerator::Instance()->ticksGet();
        readCard( positions );
        t = HeartBeatGenerator::Instance()->ticksSince(t);
        HeartBeatGenerator::nsecs n = HeartBeatGenerator::ticks2nsecs(t);
        std::cout << "Reading SSI module takes "<<long(n)<<" nanoseconds"<<endl;
#endif

#ifdef OROPKG_CORELIB_TASKS
        this->start();
#endif

#endif
    }

    EncoderSSI::~EncoderSSI()
    {
#ifdef OROPKG_CORELIB_TASKS
        this->stop();
#endif
    }

    int EncoderSSI::read( unsigned int encNr, unsigned int& position )
    {
        if ( encNr < 0 || encNr > 2 )
            return -1;

#ifdef OROPKG_CORELIB_TASKS
        ORO_OS::MutexLock Locker( readLock );
#else
        refresh();
#endif
        position = positions[ encNr ];

        return 0;
    }

    void EncoderSSI::readCard( unsigned int* buffie )
    {
#ifdef OROINT_DEVICE_DRIVERS_APCI1710_SSI
        apci1710_ssi_read_latest_all( dev, moduleNr, buffie );
        apci1710_ssi_refresh(dev, moduleNr);

        /**
         * Somehow, there are 'dirty' bits above the 21 bits, so we clean them
         */

        for ( int i = 0; i < 3; ++i )
            buffie[ i ] = buffie[ i ] & 0x001fffff; // only use 21 bits;

        // DEBUGGING
        /*
           unsigned int turn, pos, scratch;
           
           for(int i=0; i< 3; ++i)
           {
               scratch = store[i] >> 0;
               
               pos = scratch & 0xfff;      // selects 12 lowest bits, this is pos
               
               scratch = scratch >> 12; // remove pos from scratch
               turn = scratch & 0x1ff;           // selects 9 bits, this is turn
               
        //        rtos_printf("position: %u, turn: %u\n", pos, turn);
           }*/ 
        //rtos_printf("enc0:position: %u, turn: %u\n", store[0], turn);
#endif
    }

    void EncoderSSI::refresh()
    {
        readCard( store );
        switchBuffers();
    }

    void EncoderSSI::switchBuffers()
    {
        // this is called *after* each refresh, so we know that it is safe
        // use spinlocks or implement an 'atomic' swing(b1,b2,p1,p2) function !!!
        //
        //
#ifdef OROPKG_CORELIB_TASKS
        ORO_OS::MutexTryLock locker( readLock );
        if ( locker.isSuccessful() )
#endif
            if ( positions == buf1 )
                {
                    positions = buf2;
                    store = buf1;
                }
            else
                {
                    positions = buf1;
                    store = buf2;
                }
    }

};
