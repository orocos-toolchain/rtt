/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  ForceSensor1D.hpp 

                        ForceSensor1D.hpp -  description
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
 
 

#include <fdi/SensorInterface.hpp>
#include <fdi/AnalogInInterface.hpp>

namespace ORO_DeviceDriver
{
    using namespace ORO_CoreLib;

    /**
     * A ForceSensor measures forces in Newton
     */
class ForceSensor1D
    :public SensorInterface<double>
{
public:
    ForceSensor1D(AnalogInInterface<unsigned int>* input, unsigned int chan, 
                  const Data scaleA = 1, const Data offsetA = 0)
        :dataCard(input), channel(chan), 
         scale(scaleA), offset(offsetA) 
    {}

    virtual int readSensor(Data& data)
    {
        unsigned int d=0;
        dataCard->read(channel, d);
        data = dataCard->lowest() + d/dataCard->resolution();
        data = data * scale + offset;
        std::cout <<"low :"<<dataCard->lowest()<<" res: "<< dataCard->resolution() <<" d:"<<d<<std::endl;
        return 0;
    }

    virtual Data maxMeasurement()
    {
        return dataCard->highest()*scale+offset;
    }

    virtual Data minMeasurement()
    {
        return dataCard->lowest()*scale+offset;
    }
    
    virtual Data zeroMeasurement()    
    {
        return 0;
    }

    virtual void offsetSet( const Data& o)
    {
        offset = o;
    }

    virtual void scaleSet( const Data& s)
    {
        scale = s;
    }
private:
    AnalogInInterface<unsigned int>* dataCard;
    
    unsigned int channel;

    Data scale;
    Data offset;
};

}
