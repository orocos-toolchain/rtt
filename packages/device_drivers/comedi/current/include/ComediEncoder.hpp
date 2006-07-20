// Copyright (C) 2003 Klaas Gadeyne <klaas.gadeyne@mech.kuleuven.ac.be>
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


#ifndef __COMEDI_ENCODER_INCREMENTAL_HPP
#define __COMEDI_ENCODER_INCREMENTAL_HPP


#include <pkgconf/device_drivers_comedi.h>

#include "SensorInterface.hpp"
#include "EncoderInterface.hpp"
#include "ComediDevice.hpp"
#include <string>

namespace RTT
{
  typedef double UnitType;  
  /**
   * A class for reading an encoder using the comedi hardware
   * abstraction layer.  Based on the comedi API of the home
   * written driver for the NI660X card.
   * @todo Currently this wrapper does not support all functionality.
   * It allways uses X4 encoding (maximum resolution), you cannot choose
   * when to take into account the indexpulse or reset the counter
   * when the index pulse arrives.  Also see the comments of the
   * encodertest.c program 
   * See <http://people.mech.kuleuven.ac.be/~kgadeyne/linux/> for more
   * information about all this stuff
   * @todo subdevice locking
   * @bug upcounting is allways true
   * @bug The current implementation does not consider the _turn
   * parameter, since an overflow of the 32 bit register is unlikely
   * to occur in our case.
   */
  class ComediEncoder :
    public EncoderInterface
  {
  public:
    /**
     * Create a nameserved encoder.
     *
     * @param cd The comedi device your are using
     * @param subd The comedi subdevice where the COUNTER is situated.
     * @param encNr The number of the encoder on the comedi subdevice
     * (aka channel)
     * @param name  The name of the encoder.
     */
    ComediEncoder(ComediDevice * cd, unsigned int subd, 
		  unsigned int encNr, const std::string& name);

    /**
     * Create an encoder.
     *
     * @param cd The comedi device your are using
     * @param subd The comedi subdevice where the COUNTER is situated.
     * @param encNr The number of the encoder on the comedi subdevice
     * (aka channel)
     */
    ComediEncoder(ComediDevice * cd, unsigned int subd, unsigned int encNr);

    virtual ~ComediEncoder();

    // Redefinition of Pure virtuals    
    virtual int positionGet() const;
    virtual int turnGet() const;
    virtual void positionSet( int p);
    virtual void turnSet( int t );
    virtual int resolution() const;
    virtual bool upcounting() const;

  protected:
    void init();

    // Is this enough?
    ComediDevice * _myCard;
    unsigned int _subDevice;
    unsigned int _channel;
    
    int _turn;
    int _resolution;
    bool _upcounting;
  };

};

#endif

