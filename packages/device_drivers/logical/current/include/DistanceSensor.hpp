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

#ifndef __DISTANCE_SENSOR_H__
#define __DISTANCE_SENSOR_H__

#include <device_interface/AnalogInInterface.hpp>
#include <device_interface/SensorInterface.hpp>

namespace ORO_DeviceDriver
{
    using namespace ORO_CoreLib;
    using namespace ORO_DeviceInterface;

  typedef double SensorData;
  /** Sensor that measures (1D-)distance.
      Sensor for measuring distances that can be read via an
      AnalogInInterface device.
      Applicable to any sensor that delivers measurements in the form
      of voltage, current, ...
  */
  class DistanceSensor : public SensorInterface<SensorData>
  {
  public:
    /** Constructor
	@param a_in AnalogInInterface device (eg. ComediSubDeviceAIn)
	@param chan channel on the AnalogInInterface device to which
	sensor output is connected
    */
    DistanceSensor(AnalogInInterface<unsigned int> * a_in, int chan);
    /// Destructor
    virtual ~DistanceSensor();

    // Redefine Pure virtuals
    virtual int readSensor (SensorData & data) const;
    virtual SensorData readSensor () const;
    virtual SensorData maxMeasurement () const;
    virtual SensorData minMeasurement () const;
    virtual SensorData zeroMeasurement () const;
    // The following are deprecated ...
    virtual void offsetSet (const SensorData &);
    virtual void scaleSet (const SensorData &);
  
      virtual void calibrate();
      virtual bool isCalibrated() const;
      virtual void unCalibrate();
  protected:
    AnalogInInterface<unsigned int> * _aIn;
    int _chan;
    SensorData _offset;
    SensorData _scale;
  };

}
#endif

