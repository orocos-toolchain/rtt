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
    // Constructors
    JR3Sensor(unsigned int DSP );
    JR3Sensor(JR3Sensor& copy);
    virtual ~JR3Sensor();

    // Read the current FT data
    virtual int read(struct ForceArray& data);

    // Set the samplePeriod (in seconds). In this case, the sample period determines
    // the filter from which the data will be retrieved.
    virtual void setSamplePeriod(float period);

    // Check for error (return 0 on OK, else the error byte/word)
    unsigned int checkForError();

    
protected:
    virtual void step() { refresh(); };
    
private:
    void refresh();
    void switchBuffers();

    JR3Filters       _filterToReadFrom;
    unsigned int     _dsp;
    unsigned int     _counter;
    ForceArray*      _readBuffer;
    ForceArray*      _writeBuffer;
    ForceArray       _buffer1;
    ForceArray       _buffer2;
    ORO_OS::Mutex _readLock;
};

}; // namespace ORO_DeviceDriver

#endif   // JR3SENSOR_H
