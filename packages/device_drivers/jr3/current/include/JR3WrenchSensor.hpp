/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  JR3WrenchSensor.hpp 

                        JR3WrenchSensor.hpp -  description
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
 

#ifndef _JR3_WRENCH_SENSOR_H
#define _JR3_WRENCH_SENSOR_H

#include <os/Mutex.hpp>
#include <os/MutexLock.hpp>
#include <corelib/TaskNonPreemptible.hpp>
#include <device_interface/SensorInterface.hpp>
#include <geometry/frames.h>
#include "jr3dsp.h"

namespace ORO_DeviceDriver
{

enum JR3Sensors {Undefined, _200N20, _100N5};
    
class JR3WrenchSensor : public ORO_CoreLib::TaskNonPreemptible,
                        public ORO_DeviceInterface::SensorInterface<ORO_Geometry::Wrench>
{
    enum JR3Filters {Undefined, Filter1, Filter2, Filter3, Filter4, Filter5, Filter6};
    
    
public:
    // Constructor
    JR3WrenchSensor(unsigned int DSP, float samplePeriod, JR3Sensors type );
    virtual ~JR3WrenchSensor();

    
    /**
     * Read the Data, the structure that this sensor 'exports'
     * @return 0 on success
     * @return -1 on failure
     */
    virtual int readSensor( ORO_Geometry::Wrench& p ) const;

    /**
     * Return the last measurement.
     */
    virtual ORO_Geometry::Wrench readSensor() const;

    /**
     * Returns the maximum value this sensor can read.
     */
    virtual ORO_Geometry::Wrench maxMeasurement() const;

    /**
     * Returns the minimum value this sensor can read.
     */
    virtual ORO_Geometry::Wrench minMeasurement() const;

    /**
     * Returns the value which would be given with the sensor in ideal
     * rest.
     */
    virtual ORO_Geometry::Wrench zeroMeasurement() const;
     
    
    // Chooses the correct lowpass filter for the given sample period.
    virtual void chooseFilter(float period);

    // Check for error (return 0 on OK, else the error byte/word)
    unsigned int checkForError();

    // Prints out some checks in kernel log
    void checkSensorAndDSP();

    // Set/Add offset
    void offsetSet( const ForceArray& offsets );
    void offsetAdd( const ForceArray& offsets );

    
protected:
    virtual void step() { refresh(); };
    
private:
    // Copy constructor not implemented
    JR3WrenchSensor(JR3WrenchSensor& copy) : TaskNonPreemptible(0.01), _filterToReadFrom(Filter6) {};
    void refresh();
    void switchBuffers();

    JR3Sensors                 _type;
    JR3Filters                 _filterToReadFrom;
    unsigned int               _dsp;
    unsigned int               _counter;
    
    ORO_Geometry::Wrench*      _readBuffer;
    ORO_Geometry::Wrench*      _writeBuffer;
    ForceArray                 _writeArray;
    ORO_Geometry::Wrench       _buffer1;
    ORO_Geometry::Wrench       _buffer2;
    mutable ORO_OS::Mutex              _readLock;
    
    // The offsets currently in use
    ForceArray       _currentOffset;
    ForceArray       _rotatedOffset;
};



inline std::ostream& operator << (std::ostream& s, ForceArray ft)
{
  s << "[" << ft.Fx << "\t" << ft.Fy << "\t" << ft.Fz << "\t"
           << ft.Tx << "\t" << ft.Ty << "\t" << ft.Tz << "]";
  return s;
}
                                                                                                                                          
}; // namespace ORO_DeviceDriver

#endif   // JR3SENSOR_H
