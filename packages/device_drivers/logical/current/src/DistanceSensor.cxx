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

#include "device_drivers/DistanceSensor.hpp"

using namespace ORO_CoreLib;
using namespace CBDeviceDriver;


DistanceSensor::DistanceSensor(AnalogInInterface<unsigned int> * a_in, 
			       int chan)
  : _aIn(a_in), _chan(chan), _offset(0.0), _scale(1.0)
{};

DistanceSensor::~DistanceSensor(){};

int DistanceSensor::readSensor (SensorData & data)
{
  unsigned int d;
  _aIn->read(_chan,d);
  /* First, take into account resolution and offset of
     AnalogInInterface
     Conversion from bits -> Volts (or Ampere or ...)
  */
  data = (double (d - _aIn->binaryLowest()) / _aIn->resolution(_chan)) + _aIn->lowest(_chan);
  /* Then apply conversion from Volts -> Distance Unit */
  data += _offset;
  data *= _scale;
  // Cannot check if _aIn->read() worked (no return value, so this is
  // always true :-(
  return 1;
}

SensorData DistanceSensor::maxMeasurement()
{
  return (SensorData) _aIn->highest(_chan);
}

SensorData DistanceSensor::minMeasurement()
{
   return (SensorData) _aIn->lowest(_chan);
}

SensorData DistanceSensor::zeroMeasurement()
{
  // Don't know what Peter means with this function
  /* I quote
     "Returns the value which would be given with the sensor in ideal
     rest."
  */
  return (SensorData) 0.0;
}

void DistanceSensor::offsetSet (const SensorData & offset)
{
  rtos_printf( "DistanceSensor::offsetSet() = DEPRECATED?\n" );
  _offset = offset;
}

void DistanceSensor::scaleSet (const SensorData & scale)
{
  rtos_printf( "DistanceSensor::scaleSet() = DEPRECATED?\n" );
  _scale = scale;
}

  
