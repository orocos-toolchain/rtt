/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  JR3Sensor.hpp 

                        JR3Sensor.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 

#ifndef JR3SENSOR_H
#define JR3SENSOR_H

#include <os/Mutex.hpp>
#include <os/MutexLock.hpp>
#include <corelib/TaskNonPreemptible.hpp>
#include "jr3dsp.h"

namespace ORO_DeviceDriver
{

class JR3Sensor : public ORO_CoreLib::TaskNonPreemptible
{
    enum JR3Filters {Undefined, Filter1, Filter2, Filter3, Filter4, Filter5, Filter6};
    
public:
    // Constructor
    JR3Sensor(unsigned int DSP, float samplePeriod );
    virtual ~JR3Sensor();

    // Read the current FT data
    virtual int read(struct ForceArray& data);

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
    JR3Sensor(JR3Sensor& copy) : TaskNonPreemptible(0.01), _filterToReadFrom(Filter6) {};
    void refresh();
    void switchBuffers();

    JR3Filters       _filterToReadFrom;
    unsigned int     _dsp;
    unsigned int     _counter;
    ForceArray*      _readBuffer;
    ForceArray*      _writeBuffer;
    ForceArray       _rotatedBuffer;
    ForceArray       _buffer1;
    ForceArray       _buffer2;
    ORO_OS::Mutex    _readLock;
    
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
