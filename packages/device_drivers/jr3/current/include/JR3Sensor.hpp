
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
