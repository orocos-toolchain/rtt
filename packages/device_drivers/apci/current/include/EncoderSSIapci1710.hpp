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


#ifndef _EncoderSSI_apci1710_HPP
#define _EncoderSSI_apci1710_HPP

#include <pkgconf/system.h>

#include <os/MutexLock.hpp>
#include <corelib/TaskNonPreemptible.hpp>
#include <device_interface/EncoderInterface.hpp>
#include <pkgconf/device_drivers_apci.h>

#include <string>

/**
 */


namespace ORO_DeviceDriver
{


/**
 * An EncoderSSI_apci1710_board is a 'data-refresh' class for an
 * apci1710 board with 1 or more modules configured as SSI encoder
 * inputs. Maximum nr of SSI modules is 4, since there are 4 modules
 * on an apci1710.
 * 
 * It is a data-refresh class, since it reads new encoder data 
 * periodically from the SSI modules and stores it into a local buffer.
 * 
 *
 * unsigned int read( unsigned int encNr ) returns the encoder ticks
 * of encoder <encNr>.
 * 
 * Since there are 3 encoders pro SSI module, <encNr> is:
 *      1,  2 or  3 for the encoders of module <mNr1>;
 *
 * and, if available:
 *      4,  5 or  6 for the encoders of module <mNr2>;
 *      7,  8 or  9 for the encoders of module <mNr3>;
 *     10, 11 or 12 for the encoders of module <mNr4>;
 * 
 *
 * 
 * @TODO : It probably doesn't work when there are multiple apci1710's.
 *         The 'dirty' bits above bit 21 is probably related to our SSI encoders
 *         and probably has nothing to do with the apci1710 card.
 *         Dependency on CoreLib_Tasks
 *         Dependency on Device_Interface package
 *         Dependency on apci1710 package
 *        
 */

class EncoderSSI_apci1710_board : public ORO_CoreLib::TaskNonPreemptible
{
public:
    EncoderSSI_apci1710_board( unsigned int mNr1 );
    EncoderSSI_apci1710_board( unsigned int mNr1, unsigned int mNr2 );
    EncoderSSI_apci1710_board( unsigned int mNr1, unsigned int mNr2, unsigned int mNr3 );
    EncoderSSI_apci1710_board( unsigned int mNr1, unsigned int mNr2, unsigned int mNr3, unsigned int mNr4 );
    virtual ~EncoderSSI_apci1710_board();
    
    unsigned int read( unsigned int encNr );

    
    
protected:
    virtual void step();
    void readCard( unsigned int* );
    void switchBuffers();
    
    unsigned int nr_of_modules;
    unsigned int moduleNr1;
    unsigned int moduleNr2;
    unsigned int moduleNr3;
    unsigned int moduleNr4;

    OROIMP_DEVICE_DRIVERS_APCI1710_T* dev;

    unsigned int* readbuffer;
    unsigned int* writebuffer;
    unsigned int* buffer1;
    unsigned int* buffer2;

    ORO_OS::Mutex readLock;
};




/**
 * This class implements the EncoderInterface for an encoder
 * on an apci1710 board.
 *
 * 
 * Its encoder number <encNr> should be 1 to 3 * <nrSSI>, with
 * <nrSSI> the number of modules on the apci1710 that are
 * configured for SSI.
 *
 * @TODO: positionSet()
 *        turnSet()
 *        resolution()
 *
 */

class EncoderSSI_apci1710 : public ORO_DeviceInterface::EncoderInterface
{
public:
    EncoderSSI_apci1710( unsigned int encNr, EncoderSSI_apci1710_board* board )
    {
        _board = board;
        _encNr = encNr;
    }
    
    virtual ~EncoderSSI_apci1710() {};

    
    // implementations of EncoderInterface methods:
    virtual int positionGet() const { return (int) _board->read( _encNr ); }
                                                                                                                                          
    virtual int turnGet() const { return 0; }   // absolute encoder without turns
                                                                                                                                      
    virtual void positionSet( int p ) {};
                                                                                                                                      
    virtual void turnSet( int t ) {};
                                                                                                                                      
    virtual int resolution() const {return 0;};
                                                                                                                                      
    virtual bool upcounting() const {return true;};

    
private:
    EncoderSSI_apci1710_board* _board;
    unsigned int _encNr;
};

}; // Namespace ORO_DeviceDriver

#endif // _EncoderSSI_apci1710_HPP
