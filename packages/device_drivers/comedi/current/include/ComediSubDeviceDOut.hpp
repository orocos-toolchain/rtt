// $Id: ComediSubDeviceDOut.hpp,v 1.4 2003/08/14 09:17:13 kgadeyne Exp $
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

/* Based completely on ComediSubDeviceDin.hpp,  basically I didn't do
   anything but M-x query-replace [Enter] in [Enter] out :-)
   And changed the api to that of DigitalOutInterface offcourse.
   And the implementation: M-x query-replace [Enter] read [Enter] write :-)
   And some other things...
   And ...
*/

#ifndef COMEDISUBDEVICEDOUT_HPP
#define COMEDISUBDEVICEDOUT_HPP

#include <device_interface/DigitalOutInterface.hpp>
#include "ComediDevice.hpp"

namespace ORO_DeviceDriver
{

  /**
   * This logical device represents one subdevice of a Comedi device.
   */
  class ComediSubDeviceDOut
    : public ORO_DeviceInterface::DigitalOutInterface
  {

  public:
    /**
     * Create a new ComediSubDeviceDOut with a given ComediDevice, subdevice number and
     * a name for this sub device
     *
     * @param cd The ComediDevice to use for output
     * @param subdevice The subdevice number for this comedi device
     * @param name The name of this instance
     */
    ComediSubDeviceDOut( ComediDevice* cd, const std::string& name, unsigned int subdevice);

    ComediSubDeviceDOut( ComediDevice* cd, unsigned int subdevice );

    void init();

    virtual void switchOn( unsigned int bit);

    virtual void switchOff( unsigned int bit);

    virtual void setBit( unsigned int bit, bool value);

    virtual void setSequence(unsigned int start_bit, unsigned int stop_bit, unsigned int value);

    virtual bool checkBit( unsigned int bit) const;

    virtual unsigned int checkSequence (unsigned int start_bit, unsigned int stop_bit) const;

    virtual unsigned int nbOfOutputs() const;

  protected:
    /**
     * The output device to write to
     */
    ComediDevice* myCard;

    /**
     * The subdevice number of this instance in \a myCard
     */
    unsigned int subDevice;
    unsigned int channels;
  };

}

#endif
