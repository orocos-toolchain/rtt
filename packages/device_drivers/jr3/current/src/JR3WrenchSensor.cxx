/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:32 CEST 2004  JR3WrenchSensor.cxx 

                        JR3WrenchSensor.cxx -  description
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

#include "jr3dsp_lxrt.h"
#include "jr3/JR3WrenchSensor.hpp"

#include <os/rtstreams.hpp>


#define CUTOFF_FREQUENCY_FILTER1 500.0
#define CUTOFF_FREQUENCY_FILTER2 125.0
#define CUTOFF_FREQUENCY_FILTER3  31.25
#define CUTOFF_FREQUENCY_FILTER4   7.813
#define CUTOFF_FREQUENCY_FILTER5   1.953
#define CUTOFF_FREQUENCY_FILTER6   0.4883

namespace ORO_DeviceDriver
{

JR3WrenchSensor::JR3WrenchSensor(unsigned int DSP, float samplePeriod, std::string type, ORO_CoreLib::Event<void(void)>& maximumload)

  : TaskNonPreemptible( samplePeriod ), _filterToReadFrom(Filter6), _dsp(DSP)
{
  //  &_maximumload_event = &maximumload;

  if (type == "200N20")
    _type = _200N20;
  else if (type == "100N5")
    _type = _100N5;
  else
    // wrong type
    assert(0);

  chooseFilter( this->periodGet() );
  _readBuffer  = &_buffer1;
  _writeBuffer = &_buffer2;

  JR3DSP_setUnits_N_dNm_mmX10(_dsp);

  this->checkSensorAndDSP();

  this->start();
  rtos_printf("(WrenchSensorJR3)  task started\n");
}
  

JR3WrenchSensor::~JR3WrenchSensor( )
{
  this->stop();
  rtos_printf("(WrenchSensorJR3)  task stoped\n");
}


unsigned int JR3WrenchSensor::checkForError()
{
    return JR3DSP_get_error_word(_dsp);
}


void JR3WrenchSensor::checkSensorAndDSP()
{
    JR3DSP_check_sensor_and_DSP( _dsp );
}


void JR3WrenchSensor::offsetSet(const ForceArray& newOffset)
{
  rtos_printf("(WrenchSensorJR3)  OffsetSet\n");
  // Copy
  _currentOffset = newOffset;
  // And adjust
  _currentOffset.Fy = -_currentOffset.Fy;
  _currentOffset.Ty = -_currentOffset.Ty;
  // All the torques are in dNm (Nm*10), so scale:
  _currentOffset.Tx *= 10.0;
  _currentOffset.Ty *= 10.0;
  _currentOffset.Tz *= 10.0;
  
  JR3DSP_set_offsets(&_currentOffset, _dsp);
}



void JR3WrenchSensor::offsetAdd(const ForceArray& extraOffset)
{
  rtos_printf("(WrenchSensorJR3)  OffsettAdd\n");
  // First calculate the new offset (see above)
  _currentOffset.Fx += extraOffset.Fx;
  _currentOffset.Fy -= extraOffset.Fy;
  _currentOffset.Fz += extraOffset.Fz;
  _currentOffset.Tx += extraOffset.Tx*10.0;
  _currentOffset.Ty -= extraOffset.Ty*10.0;
  _currentOffset.Tz += extraOffset.Tz*10.0;
  
  JR3DSP_set_offsets(&_currentOffset, _dsp);
}


int JR3WrenchSensor::readSensor(ORO_Geometry::Wrench& data) const
{
    ORO_OS::MutexLock locker(_readLock);
    data = *_readBuffer;
    return 0;
}



ORO_Geometry::Wrench JR3WrenchSensor::readSensor() const
{
    ORO_OS::MutexLock locker(_readLock);
    return *_readBuffer;
}
    


ORO_Geometry::Wrench JR3WrenchSensor::maxMeasurement() const
{
    switch(_type)
    {
        case _200N20:
            return ORO_Geometry::Wrench(ORO_Geometry::Vector(200,200,400), ORO_Geometry::Vector(20,20,20));
        case _100N5:
            return ORO_Geometry::Wrench(ORO_Geometry::Vector(100,100,200), ORO_Geometry::Vector(5,5,5));
    }

    rtos_printf("JR3WrenchSensor::maxMeasuremnet() should not get here !\n");
    return ORO_Geometry::Wrench(ORO_Geometry::Vector(0,0,0), ORO_Geometry::Vector(0,0,0));
}


ORO_Geometry::Wrench JR3WrenchSensor::minMeasurement() const
{
    switch(_type)
    {
        case _200N20:
            return ORO_Geometry::Wrench(ORO_Geometry::Vector(-200,-200,-400), ORO_Geometry::Vector(-20,-20,-20));
        case _100N5:
            return ORO_Geometry::Wrench(ORO_Geometry::Vector(-100,-100,-200), ORO_Geometry::Vector(-5,-5,-5));
    }
    
    rtos_printf("JR3WrenchSensor::minMeasuremnet() should not get here !\n");
    return ORO_Geometry::Wrench(ORO_Geometry::Vector(0,0,0), ORO_Geometry::Vector(0,0,0));
}



ORO_Geometry::Wrench JR3WrenchSensor::zeroMeasurement() const  
{
    return ORO_Geometry::Wrench(ORO_Geometry::Vector(0,0,0), ORO_Geometry::Vector(0,0,0));
}



void JR3WrenchSensor::refresh()
{

    switch (_filterToReadFrom)
    {
        case Filter1:
            JR3DSP_getDataFromFilter1(&_writeArray, _dsp);
            break;
        case Filter2:
            JR3DSP_getDataFromFilter2(&_writeArray, _dsp);
            break;
        case Filter3:
            JR3DSP_getDataFromFilter3(&_writeArray, _dsp);
            break;
        case Filter4:
            JR3DSP_getDataFromFilter4(&_writeArray, _dsp);
            break;
        case Filter5:
            JR3DSP_getDataFromFilter5(&_writeArray, _dsp);
            break;
        case Filter6:
            JR3DSP_getDataFromFilter6(&_writeArray, _dsp);
            break;
        default:
            rtos_printf("JR3WrenchSensor::read() from non existing filter !\n");
            break;
    }


    // Copy to _writeBuffer (a Wrench)
    // Measurements are in a right turning coordinate system (force exerted BY the sensor),
    // so switch Fy and Ty sign to get left turning measurement.
    (*_writeBuffer)(0) =   _writeArray.Fx;
    (*_writeBuffer)(1) = - _writeArray.Fy; 
    (*_writeBuffer)(2) =   _writeArray.Fz;
    // All the torques are in dNm (Nm*10), so scale:
    (*_writeBuffer)(3) =   _writeArray.Tx/10.0;
    (*_writeBuffer)(4) = - _writeArray.Ty/10.0;
    (*_writeBuffer)(5) =   _writeArray.Tz/10.0;

    
    switch (_type)
    {
        case _200N20:
	  if (    ((*_writeBuffer)(0) > 200 ) || ((*_writeBuffer)(0) < -200) ||
		  ((*_writeBuffer)(1) > 200 ) || ((*_writeBuffer)(1) < -200) ||
                  ((*_writeBuffer)(2) > 400 ) || ((*_writeBuffer)(2) < -400) ||
                  ((*_writeBuffer)(3) >  20 ) || ((*_writeBuffer)(3) <  -20) ||
                  ((*_writeBuffer)(4) >  20 ) || ((*_writeBuffer)(4) <  -20) ||
		  ((*_writeBuffer)(5) >  20 ) || ((*_writeBuffer)(5) <  -20))
	    _maximumload_event.fire();
	  break;

        case _100N5:
	  if (    ((*_writeBuffer)(0) > 100 ) || ((*_writeBuffer)(0) < -100) ||
                  ((*_writeBuffer)(1) > 100 ) || ((*_writeBuffer)(1) < -100) ||
                  ((*_writeBuffer)(2) > 200 ) || ((*_writeBuffer)(2) < -200) ||
                  ((*_writeBuffer)(3) >   5 ) || ((*_writeBuffer)(3) <   -5) ||
                  ((*_writeBuffer)(4) >   5 ) || ((*_writeBuffer)(4) <   -5) ||
                  ((*_writeBuffer)(5) >   5 ) || ((*_writeBuffer)(5) <   -5))
	    _maximumload_event.fire();
	  break;
    }

    switchBuffers();
};


void JR3WrenchSensor::switchBuffers()
{
    ORO_OS::MutexTryLock locker( _readLock );
    if ( locker.isSuccessful() )
        if ( _readBuffer == &_buffer1 )
        {
            _readBuffer = &_buffer2;
            _writeBuffer = &_buffer1;
        }
        else
        {
            _readBuffer = &_buffer1;
            _writeBuffer = &_buffer2;
        }
}


void JR3WrenchSensor::chooseFilter(float period) 
{ 
    // Calculate the best filter to read from, based on the value 'T'
    if      ( period < 1.0/(2*CUTOFF_FREQUENCY_FILTER1)) _filterToReadFrom = Filter1;
    else if ( period < 1.0/(2*CUTOFF_FREQUENCY_FILTER2)) _filterToReadFrom = Filter2;
    else if ( period < 1.0/(2*CUTOFF_FREQUENCY_FILTER3)) _filterToReadFrom = Filter3;
    else if ( period < 1.0/(2*CUTOFF_FREQUENCY_FILTER4)) _filterToReadFrom = Filter4;
    else if ( period < 1.0/(2*CUTOFF_FREQUENCY_FILTER5)) _filterToReadFrom = Filter5;
    else if ( period < 1.0/(2*CUTOFF_FREQUENCY_FILTER6)) _filterToReadFrom = Filter6;
    else
    {
        rtos_printf("(WrenchSensorJR3)  Sample to low to garantee no aliasing!\n");
        _filterToReadFrom = Filter6;
    }
    
    rt_std::cout << "(WrenchSensorJR3)  JR3WrenchSensor chooseFilter: " << _filterToReadFrom << rt_std::endl;
};

}; //namespace ORO_DeviceDriver
