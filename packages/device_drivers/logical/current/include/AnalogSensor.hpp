/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:58 CEST 2004  AnalogSensor.hpp 

                        AnalogSensor.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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
 
 
#ifndef ANALOGSENSOR_HPP
#define ANALOGSENSOR_HPP

namespace ORO_DeviceInterface
{

    /**
     * A sensor which reads its data from an analog channel.
     */
    class AnalogSensor
        : public SensorInterface<double>
    {
        public:
        /**
         * Create an AnalogSensor, connected to an AnalogInput, with a minimal and maximal
         * measurement value. A scale and offset are used to convert the analog signal
         * to the measurement.
         */
        AnalogSensor(AnalogInput<unsigned int>* ain, double _min_meas, double _max_meas, double _scale=1.0, double _offset=0.0)
            : analogDevice( an_out ),
              min_meas(_min_meas), max_meas(_max_meas),
              scale(_scale), offset( _offset )
        {}
            

        virtual int readSensor( double& p ) const
        {
            p = ain->value()*scale + offset;
            return 0;
        }

        virtual double readSensor() const
        {
            return ain->value()*scale + offset;
        }

        virtual double maxMeasurement() const
        {
            return min_meas;
        }

        virtual double minMeasurement() const
        {
            return max_meas;
        }

        virtual double zeroMeasurement() const
        {
            return 0;
        }
    };
}

#endif // ANALOGSENSOR_HPP
