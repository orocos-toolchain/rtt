/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:32 CEST 2004  JR3Sensor.cxx 

                        JR3Sensor.cxx -  description
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
#include "jr3/JR3Sensor.hpp"

#include <os/rtstreams.hpp>


#define CUTOFF_FREQUENCY_FILTER1 500.0
#define CUTOFF_FREQUENCY_FILTER2 125.0
#define CUTOFF_FREQUENCY_FILTER3  31.25
#define CUTOFF_FREQUENCY_FILTER4   7.813
#define CUTOFF_FREQUENCY_FILTER5   1.953
#define CUTOFF_FREQUENCY_FILTER6   0.4883

namespace ORO_DeviceDriver
{

JR3Sensor::JR3Sensor(unsigned int DSP, float samplePeriod ) : TaskNonPreemptible( samplePeriod ), _filterToReadFrom(Filter6), _dsp(DSP)
{
    chooseFilter( this->getPeriod() );
    _readBuffer  = &_buffer1;
    _writeBuffer = &_buffer2;


  JR3DSP_setUnits_N_dNm_mmX10(_dsp);
}


JR3Sensor::~JR3Sensor( )
{
    this->stop();
    rtos_printf("JR3Sensor destructor\n");
}


unsigned int JR3Sensor::checkForError()
{
  return JR3DSP_get_error_word(_dsp);
}


void JR3Sensor::checkSensorAndDSP()
{
  JR3DSP_check_sensor_and_DSP( _dsp );
}


void JR3Sensor::offsetSet(const ForceArray& newOffset)
{
    rtos_printf("OffsetSet\n");
  // Copy
//  std::cout << " old offset: " << _currentOffset << std::endl;
//  std::cout << " new offset: " << newOffset << std::endl;
  _currentOffset = newOffset;
  // And adjust
  _currentOffset.Fy = -_currentOffset.Fy;
  _currentOffset.Ty = -_currentOffset.Ty;
  // All the torques are in dNm (Nm*10), so scale:
  _currentOffset.Tx *= 10.0;
  _currentOffset.Ty *= 10.0;
  _currentOffset.Tz *= 10.0;
//  std::cout << " old offset: " << _currentOffset << std::endl;
//  printf( "------------------------\n");
//  printf( "offset change 1!!!!!!!!!\n");
//  printf( "------------------------\n");
                                                                                                                                          
  JR3DSP_set_offsets(&_currentOffset, _dsp);
}
                                                                                                                                          
void JR3Sensor::offsetAdd(const ForceArray& extraOffset)
{
    rtos_printf("OffsettAdd\n");
//  std::cout << " old offset: " << _currentOffset << std::endl;
//  std::cout << " xra offset: " << extraOffset << std::endl;
  // First calculate the new offset (see above)
  _currentOffset.Fx += extraOffset.Fx;
  _currentOffset.Fy -= extraOffset.Fy;
  _currentOffset.Fz += extraOffset.Fz;
  _currentOffset.Tx += extraOffset.Tx*10.0;
  _currentOffset.Ty -= extraOffset.Ty*10.0;
  _currentOffset.Tz += extraOffset.Tz*10.0;
//  std::cout << " old offset: " << _currentOffset << std::endl;
  // Then set it
//  printf( "------------------------\n");
//  printf( "offset change 2!!!!!!!!!\n");
//  printf( "------------------------\n");
                                                                                                                                          
  JR3DSP_set_offsets(&_currentOffset, _dsp);
}



int JR3Sensor::read(struct ForceArray& data)
{
    ORO_OS::MutexLock locker(_readLock);
    data = *_readBuffer;
    return 0;
}


void JR3Sensor::refresh()
{

    switch (_filterToReadFrom)
    {
        case Undefined:
            rtos_printf("JR3Sensor::read() from undefined filter !\n");
            break;
        case Filter1:
            JR3DSP_getDataFromFilter1(_writeBuffer, _dsp);
            break;
        case Filter2:
            JR3DSP_getDataFromFilter2(_writeBuffer, _dsp);
            break;
        case Filter3:
            JR3DSP_getDataFromFilter3(_writeBuffer, _dsp);
            break;
        case Filter4:
            JR3DSP_getDataFromFilter4(_writeBuffer, _dsp);
            break;
        case Filter5:
            JR3DSP_getDataFromFilter5(_writeBuffer, _dsp);
            break;
        case Filter6:
            JR3DSP_getDataFromFilter6(_writeBuffer, _dsp);
            break;
        default:
            rtos_printf("JR3Sensor::read() from non existing filter !\n");
            break;
    }
    // Now, these measurements are in a right turning coordinate system
    // (the fs measures the force exerted BY the sensor), so switch
    // coordinate systems:
    _writeBuffer->Fy = -_writeBuffer->Fy;
    _writeBuffer->Ty = -_writeBuffer->Ty;
    // All the torques are in dNm (Nm*10), so scale:
    _writeBuffer->Tx = _writeBuffer->Tx/10.0;
    _writeBuffer->Ty = _writeBuffer->Ty/10.0;
    _writeBuffer->Tz = _writeBuffer->Tz/10.0;

#ifdef DEBUG
    _counter++;
    if (_counter == 100)
    {
        _counter = 0;
        rtos_printf("JR3Sensor refresh: Fx: %i, Fy: %i, Fz: %i, Tx: %i, Ty: %i, Tz: %i\n",
                    (int)_writeBuffer->Fx, (int)_writeBuffer->Fy, (int)_writeBuffer->Fz,
                    (int)_writeBuffer->Tx, (int)_writeBuffer->Ty, (int)_writeBuffer->Tz       );
    }
#endif
    switchBuffers();
};


void JR3Sensor::switchBuffers()
{
    ORO_OS::MutexTryLock locker( _readLock );
    if ( locker.isSuccessful() ){
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
}


void JR3Sensor::chooseFilter(float period) 
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
        rtos_printf("Sample to low to garantee no aliasing!\n");
        _filterToReadFrom = Filter6;
    }
    
    rt_std::cout << "JR3Sensor chooseFilter: " << _filterToReadFrom << rt_std::endl;
};

}; //namespace ORO_DeviceDriver
