// Copyright (C) 2003 Klaas Gadeyne <klaas dot gadeyne at mech dot kuleuven dot ac dot be>
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


#ifndef __K600_POSITION_INTERFACE_HPP__
#define __K600_POSITION_INTERFACE_HPP__

#include <device_interface/PositionInterface.hpp>
#include <os/SingleThread.hpp>
#include <os/MutexLock.hpp>

// Priority of the SingleThread.
#define SINGLE_THREAD_PRIORITY 0

#define MAX_NUM_LEDS 20 // Maximum number of leds
#define NUM_COORD_PER_LED 3 // Number of coordinates per led 
#define MAX_NUM_COORD (MAX_NUM_LEDS*NUM_COORD_PER_LED)

namespace ORO_DeviceDriver
{
  /**
   * A class for processing messages sent by the Krypton K600
   * Measuring device and Putting them in a standard
   * PositionInterface format
   */
  template <class Data>
  class K600PositionInterface
    : public ORO_OS::SingleThread, 
      public ORO_DeviceInterface::PositionInterface<Data>
  {
  public:
    K600PositionInterface(int num_leds);
    virtual ~K600PositionInterface();

    // Redefine PositionGet()
    virtual int PositionGet(std::vector<Data> & p);

  protected:
    virtual void loop();

    // Should make a seperate class from these...
    void switchBuffers();
    /// Points to the buffer containing the last Sensor data
    std::vector<Data> * _writeBuffer_p;
    /// Points to the read buffer (overwritten with new values)
    std::vector<Data> * _readBuffer_p;
    
    std::vector<Data> _buf1;
    std::vector<Data> _buf2;

    ORO_OS::Mutex _readLock;

  private:
    int interprete_K600_Msg(char * msg, 
			    unsigned short * a, 
			    unsigned short * b, 
			    unsigned short * c, 
			    unsigned short * d,
			    char * mychar, 
			    unsigned short * e, 
			    unsigned short * f, 
			    unsigned short * g, 
			    double** k);

    double* _coordinates[MAX_NUM_COORD];
    int _num_leds;
    bool _exit;
    
  };

}; // End namespace

#include "KryptonK600PositionInterfaceThread.inc"

#endif // __K600_POSITION_INTERFACE_HPP__

