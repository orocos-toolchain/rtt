
#include "jr3/JR3Sensor.hpp"

#include "jr3dsp_lxrt.h"
#include <os/rtstreams.hpp>


#define CUTOFF_FREQUENCY_FILTER1 500.0
#define CUTOFF_FREQUENCY_FILTER2 125.0
#define CUTOFF_FREQUENCY_FILTER3  31.25
#define CUTOFF_FREQUENCY_FILTER4   7.813
#define CUTOFF_FREQUENCY_FILTER5   1.953
#define CUTOFF_FREQUENCY_FILTER6   0.4883

namespace CBDeviceDriver
{

JR3Sensor::JR3Sensor(unsigned int DSP ) : TaskNonPreemptible(0.01), _filterToReadFrom(Filter6), _dsp(DSP)
{
    setSamplePeriod( this->periodGet() );
    _readBuffer  = &_buffer1;
    _writeBuffer = &_buffer2;
/*
  SixAxisArray temp;
  temp.axis_1 = 0;
  temp.axis_2 = 0;
  temp.axis_3 = 0;
  temp.axis_4 = 0;
  temp.axis_5 = 0;
  temp.axis_6 = 0;

  JR3DSP_set_offsets_scaled(&temp);
*/
/*
  ForceArray offsets;
  offsets.Fx =   -0.1;
  offsets.Fy =   41.0;
  offsets.Fz = -115.1;
  offsets.Tx =   28.4;
  offsets.Ty =  -16.4;
  offsets.Tz =    4.1;
*/
/*
  ForceArray offsets;
  offsets.Fx =   -3.6;
  offsets.Fy =   37.6;
  offsets.Fz = -114.3;
  offsets.Tx =   25.2;
  offsets.Ty =  -13.2;
  offsets.Tz =    4.1;

  JR3DSP_set_offsets(&offsets);
  JR3DSP_transformCoordinateSystem(ANGLE_OFFSET, CENTER_GRIPPER_OFFSET);
*/  
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


void JR3Sensor::setSamplePeriod(float period) 
{ 
    // Calculate the best filter to read from, based on the value 'T'
    if      ( period > 1.0/(2*CUTOFF_FREQUENCY_FILTER1)) _filterToReadFrom = Filter1;
    else if ( period > 1.0/(2*CUTOFF_FREQUENCY_FILTER2)) _filterToReadFrom = Filter2;
    else if ( period > 1.0/(2*CUTOFF_FREQUENCY_FILTER3)) _filterToReadFrom = Filter3;
    else if ( period > 1.0/(2*CUTOFF_FREQUENCY_FILTER4)) _filterToReadFrom = Filter4;
    else if ( period > 1.0/(2*CUTOFF_FREQUENCY_FILTER5)) _filterToReadFrom = Filter5;
    else if ( period > 1.0/(2*CUTOFF_FREQUENCY_FILTER6)) _filterToReadFrom = Filter6;
    else
    {
        rtos_printf("Sample to low to garantee no aliasing!\n");
        _filterToReadFrom = Filter6;
    }
    
    rt_std::cout << "JR3Sensor setSamplePeriod:" << _filterToReadFrom << rt_std::endl;
};

}; //namespace CBDeviceDriver
