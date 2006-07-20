/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  ComediSubDeviceDIn.hpp 

                        ComediSubDeviceDIn.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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
 
// $Id: ComediSubDeviceDIn.hpp,v 1.6 2003/08/14 09:17:13 kgadeyne Exp $
#ifndef COMEDISUBDEVICEDIN_HPP
#define COMEDISUBDEVICEDIN_HPP

#include "DigitalInInterface.hpp"
#include "ComediDevice.hpp"

namespace RTT
{

  /**
   * This logical device represents one 'Digital Input' subdevice of a Comedi device.
   */
  class ComediSubDeviceDIn
    : public DigitalInInterface
  {

  public:
    /**
     * Create a new ComediSubDeviceDIn with a given ComediDevice, subdevice number and
     * a name for this sub device
     *
     * @param cd The ComediDevice to use for output
     * @param subdevice The subdevice number for this comedi device
     * @param name The name of this instance
     */
    ComediSubDeviceDIn( ComediDevice* cd, const std::string& name, unsigned int subdevice);

    ComediSubDeviceDIn( ComediDevice* cd, unsigned int subdevice );

    void init();

    virtual bool isOn( unsigned int bit = 0) const;

    virtual bool isOff( unsigned int bit = 0) const;

    virtual bool readBit( unsigned int bit = 0) const;

    virtual unsigned int nbOfInputs() const;

    virtual unsigned int readSequence(unsigned int start_bit, unsigned int stop_bit) const;
    
  protected:
      /**
       * The output device to write to
       */
      ComediDevice* myCard;

      /**
       * The subdevice number of this instance in \a myCard
       */
      unsigned int _subDevice;
      int error;
    };

  };

#endif
