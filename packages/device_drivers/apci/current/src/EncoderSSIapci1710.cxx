/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:32 CEST 2004  EncoderSSIapci1710.cxx 

                        EncoderSSIapci1710.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
// Copyright (C) 2003 Johan Rutgeerts <johan.rutgeerts@mech.kuleuven.ac.be>
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  


#include <pkgconf/system.h>
#include <pkgconf/device_drivers_apci.h>

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

#include <device_drivers/EncoderSSIapci1710.hpp>
#include <iostream>

#ifdef OROPKG_CORELIB_TIMING
#include <corelib/TimeService.hpp>
#endif

namespace ORO_DeviceDriver
{

using ORO_CoreLib::TimeService;

// --------------------------
// Constructor for 1 module
// --------------------------
EncoderSSI_apci1710_board::EncoderSSI_apci1710_board( unsigned int mNr1 )
  : NonPreemptibleActivity( 1./ORONUM_DEVICE_DRIVERS_APCI1710_SSI_UPDATE )
  , nr_of_modules(1), moduleNr1( mNr1 )
{

    buffer1 = new unsigned int[3];
    buffer2 = new unsigned int[3];
    readbuffer  = buffer1;
    writebuffer = buffer2;
    
    std::cout << "(EncoderSSI_apci1710) Creating EncoderSSI..." << std::endl;
    dev = apci1710_get_device();
    int res = 0;
    if ( 0 != (res = apci1710_ssi_init( dev, moduleNr1,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_PROFILE,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_POSITION_BITS,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_TURN_BITS,
                                        COUNTINGMODE_BINARY,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_FREQ ) ) )      //in Hz (25kHz is ok)
        std::cout << "\n(EncoderSSI_apci1710) Error "<< res << " : Module " << moduleNr1 << " is not ready for SSI\n" << std::endl;

#ifdef OROPKG_CORELIB_TIMING
    // init one buffer, display some statistics
    TimeService::ticks t=TimeService::Instance()->getTicks();
    readCard( readbuffer );
    t = TimeService::Instance()->ticksSince(t);
    TimeService::nsecs n = TimeService::ticks2nsecs(t);
    std::cout << "(EncoderSSI_apci1710) Reading SSI module takes about " << long(n) << " nanoseconds" << std::endl;
#endif

    this->start();
}





// --------------------------
// Constructor for 2 modules
// --------------------------
EncoderSSI_apci1710_board::EncoderSSI_apci1710_board( unsigned int mNr1, unsigned int mNr2 )
  : NonPreemptibleActivity( 1./ORONUM_DEVICE_DRIVERS_APCI1710_SSI_UPDATE )
  , nr_of_modules(2), moduleNr1( mNr1 ), moduleNr2( mNr2 )
{

    buffer1 = new unsigned int[6];
    buffer2 = new unsigned int[6];
    readbuffer  = buffer1;
    writebuffer = buffer2;
    
    std::cout << "(EncoderSSI_apci1710) Creating EncoderSSI..." << std::endl;
    dev = apci1710_get_device();
    int res = 0;
    if ( 0 != (res = apci1710_ssi_init( dev, moduleNr1,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_PROFILE,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_POSITION_BITS,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_TURN_BITS,
                                        COUNTINGMODE_BINARY,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_FREQ ) ) )      //in Hz (25kHz is ok)
        std::cout << "\n(EncoderSSI_apci1710) Error "<< res << " : Module " << moduleNr1 << " is not ready for SSI\n" << std::endl;

    if ( 0 != (res = apci1710_ssi_init( dev, moduleNr2,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_PROFILE,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_POSITION_BITS,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_TURN_BITS,
                                        COUNTINGMODE_BINARY,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_FREQ ) ) )      //in Hz (25kHz is ok)
        std::cout << "\n(EncoderSSI_apci1710) Error "<< res << " : Module " << moduleNr2 << " is not ready for SSI\n" << std::endl;


#ifdef OROPKG_CORELIB_TIMING
    // init one buffer, display some statistics
    TimeService::ticks t=TimeService::Instance()->getTicks();
    readCard( readbuffer );
    t = TimeService::Instance()->ticksSince(t);
    TimeService::nsecs n = TimeService::ticks2nsecs(t);
    std::cout << "(EncoderSSI_apci1710) Reading SSI modules takes about " << long(n) << " nanoseconds" << std::endl;
#endif

    this->start();
}





// --------------------------
// Constructor for 3 modules
// --------------------------
EncoderSSI_apci1710_board::EncoderSSI_apci1710_board( unsigned int mNr1, unsigned int mNr2, unsigned int mNr3 )
  : NonPreemptibleActivity( 1./ORONUM_DEVICE_DRIVERS_APCI1710_SSI_UPDATE )
  , nr_of_modules(3), moduleNr1( mNr1 ), moduleNr2( mNr2 ), moduleNr3( mNr3 )
{

    buffer1 = new unsigned int[9];
    buffer2 = new unsigned int[9];
    readbuffer  = buffer1;
    writebuffer = buffer2;
    
    std::cout << "(EncoderSSI_apci1710) Creating EncoderSSI..." << std::endl;
    dev = apci1710_get_device();
    int res = 0;
    if ( 0 != (res = apci1710_ssi_init( dev, moduleNr1,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_PROFILE,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_POSITION_BITS,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_TURN_BITS,
                                        COUNTINGMODE_BINARY,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_FREQ ) ) )      //in Hz (25kHz is ok)
        std::cout << "\n(EncoderSSI_apci1710) Error "<< res << " : Module " << moduleNr1 << " is not ready for SSI\n" << std::endl;

    if ( 0 != (res = apci1710_ssi_init( dev, moduleNr2,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_PROFILE,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_POSITION_BITS,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_TURN_BITS,
                                        COUNTINGMODE_BINARY,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_FREQ ) ) )      //in Hz (25kHz is ok)
        std::cout << "\n(EncoderSSI_apci1710) Error "<< res << " : Module " << moduleNr2 << " is not ready for SSI\n" << std::endl;

    if ( 0 != (res = apci1710_ssi_init( dev, moduleNr3,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_PROFILE,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_POSITION_BITS,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_TURN_BITS,
                                        COUNTINGMODE_BINARY,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_FREQ ) ) )      //in Hz (25kHz is ok)
        std::cout << "\n(EncoderSSI_apci1710) Error "<< res << " : Module " << moduleNr3 << " is not ready for SSI\n" << std::endl;


#ifdef OROPKG_CORELIB_TIMING
    // init one buffer, display some statistics
    TimeService::ticks t=TimeService::Instance()->getTicks();
    readCard( readbuffer );
    t = TimeService::Instance()->ticksSince(t);
    TimeService::nsecs n = TimeService::ticks2nsecs(t);
    std::cout << "(EncoderSSI_apci1710) Reading SSI modules takes about " << long(n) << " nanoseconds" << std::endl;
#endif

    this->start();
}





// --------------------------
// Constructor for 4 modules
// --------------------------
EncoderSSI_apci1710_board::EncoderSSI_apci1710_board( unsigned int mNr1, unsigned int mNr2, unsigned int mNr3, unsigned int mNr4 )
  : NonPreemptibleActivity( 1./ORONUM_DEVICE_DRIVERS_APCI1710_SSI_UPDATE )
  , nr_of_modules(3), moduleNr1( mNr1 ), moduleNr2( mNr2 ), moduleNr3( mNr3 ), moduleNr4( mNr4 )
{

    buffer1 = new unsigned int[12];
    buffer2 = new unsigned int[12];
    readbuffer  = buffer1;
    writebuffer = buffer2;
    
    std::cout << "(EncoderSSI_apci1710) Creating EncoderSSI..." << std::endl;
    dev = apci1710_get_device();
    int res = 0;
    if ( 0 != (res = apci1710_ssi_init( dev, moduleNr1,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_PROFILE,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_POSITION_BITS,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_TURN_BITS,
                                        COUNTINGMODE_BINARY,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_FREQ ) ) )      //in Hz (25kHz is ok)
        std::cout << "\n(EncoderSSI_apci1710) Error "<< res << " : Module " << moduleNr1 << " is not ready for SSI\n" << std::endl;

    if ( 0 != (res = apci1710_ssi_init( dev, moduleNr2,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_PROFILE,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_POSITION_BITS,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_TURN_BITS,
                                        COUNTINGMODE_BINARY,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_FREQ ) ) )      //in Hz (25kHz is ok)
        std::cout << "\n(EncoderSSI_apci1710) Error "<< res << " : Module " << moduleNr2 << " is not ready for SSI\n" << std::endl;

    if ( 0 != (res = apci1710_ssi_init( dev, moduleNr3,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_PROFILE,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_POSITION_BITS,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_TURN_BITS,
                                        COUNTINGMODE_BINARY,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_FREQ ) ) )      //in Hz (25kHz is ok)
        std::cout << "\n(EncoderSSI_apci1710) Error "<< res << " : Module " << moduleNr3 << " is not ready for SSI\n" << std::endl;

    if ( 0 != (res = apci1710_ssi_init( dev, moduleNr4,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_PROFILE,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_POSITION_BITS,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_TURN_BITS,
                                        COUNTINGMODE_BINARY,
                                        ORONUM_DEVICE_DRIVERS_APCI1710_SSI_FREQ ) ) )      //in Hz (25kHz is ok)
        std::cout << "\n(EncoderSSI_apci1710) Error "<< res << " : Module " << moduleNr4 << " is not ready for SSI\n" << std::endl;


#ifdef OROPKG_CORELIB_TIMING
    // init one buffer, display some statistics
    TimeService::ticks t=TimeService::Instance()->getTicks();
    readCard( readbuffer );
    t = TimeService::Instance()->ticksSince(t);
    TimeService::nsecs n = TimeService::ticks2nsecs(t);
    std::cout << "(EncoderSSI_apci1710) Reading SSI modules takes about " << long(n) << " nanoseconds" << std::endl;
#endif

    this->start();
}





EncoderSSI_apci1710_board::~EncoderSSI_apci1710_board()
{
    this->stop();
    delete[] buffer1;
    delete[] buffer2;
}




unsigned int EncoderSSI_apci1710_board::read( unsigned int encNr )
{
#ifdef DEBUG
    if ( encNr < 1 || encNr > nr_of_modules * 3 ) std::cout << "(EncoderSSI_apci1710) ERROR: encNr < 1 or encNr > " << nr_of_modules * 3 << "!" << std::endl;
#endif    
    
    ORO_OS::MutexLock Locker( readLock );
    return readbuffer[ encNr - 1 ];
}




void EncoderSSI_apci1710_board::readCard( unsigned int* buffer )
{
    switch(nr_of_modules)
    {
        case 1: 
                apci1710_ssi_refresh(dev, moduleNr1);                         // refresh module1
                apci1710_ssi_read_latest_all( dev, moduleNr1, buffer );       // read out module1
   
                // Somehow, there are 'dirty' bits above the 21 bits, so we clean them
                for ( unsigned int i = 0; i < 3; i++) buffer[i] = buffer[i] & 0x001fffff; // only use 21 bits;
                break;

        case 2:
                apci1710_ssi_refresh(dev, moduleNr1);                         // refresh module1
                apci1710_ssi_refresh(dev, moduleNr2);                         // refresh module2
                apci1710_ssi_read_latest_all( dev, moduleNr1, buffer );       // read out module1
                apci1710_ssi_read_latest_all( dev, moduleNr2, buffer + 3 );   // read out module2
   
                // Somehow, there are 'dirty' bits above the 21 bits, so we clean them
                for ( unsigned int i = 0; i < 6; i++) buffer[i] = buffer[i] & 0x001fffff; // only use 21 bits;
                break;

        case 3:
                apci1710_ssi_refresh(dev, moduleNr1);                         // refresh module1
                apci1710_ssi_refresh(dev, moduleNr2);                         // refresh module2
                apci1710_ssi_refresh(dev, moduleNr3);                         // refresh module3
                apci1710_ssi_read_latest_all( dev, moduleNr1, buffer );       // read out module1
                apci1710_ssi_read_latest_all( dev, moduleNr2, buffer + 3 );   // read out module2
                apci1710_ssi_read_latest_all( dev, moduleNr3, buffer + 6 );   // read out module3
   
                // Somehow, there are 'dirty' bits above the 21 bits, so we clean them
                for ( unsigned int i = 0; i < 9; i++) buffer[i] = buffer[i] & 0x001fffff; // only use 21 bits;
                break;
                
        case 4:
                apci1710_ssi_refresh(dev, moduleNr1);                         // refresh module1  
                apci1710_ssi_refresh(dev, moduleNr2);                         // refresh module2
                apci1710_ssi_refresh(dev, moduleNr3);                         // refresh module3
                apci1710_ssi_refresh(dev, moduleNr4);                         // refresh module4
                apci1710_ssi_read_latest_all( dev, moduleNr1, buffer );       // read out module1
                apci1710_ssi_read_latest_all( dev, moduleNr2, buffer + 3 );   // read out module2
                apci1710_ssi_read_latest_all( dev, moduleNr3, buffer + 6 );   // read out module3
                apci1710_ssi_read_latest_all( dev, moduleNr4, buffer + 9 );   // read out module4
   
                // Somehow, there are 'dirty' bits above the 21 bits, so we clean them
                for ( unsigned int i = 0; i < 12; i++) buffer[i] = buffer[i] & 0x001fffff; // only use 21 bits;
                break;
    }
}




void EncoderSSI_apci1710_board::step()
{
    readCard( writebuffer );
    switchBuffers();
}




void EncoderSSI_apci1710_board::switchBuffers()
{
    // this is called *after* each refresh, so we know that it is safe
    // use spinlocks or implement an 'atomic' swing(b1,b2,p1,p2) function !!!
    
    ORO_OS::MutexTryLock locker( readLock );
    if ( locker.isSuccessful() )
        if ( readbuffer == buffer1 )
        {
            readbuffer = buffer2;
            writebuffer = buffer1;
        }
        else
        {
            readbuffer = buffer1;
            writebuffer = buffer2;
        }
}



}; // Namespace ORO_DeviceDriver
