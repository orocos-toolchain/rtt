/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:58 CEST 2004  AnalogSensor.hpp 

                        AnalogSensor.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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
 
#ifndef ANALOGSENSOR_HPP
#define ANALOGSENSOR_HPP

#include "AnalogInput.hpp"
#include "SensorInterface.hpp"

namespace RTT
{

    

    /**
     * A SensorInterface which reads its data from an analog channel.
     */
    class AnalogSensor
        : public SensorInterface<double>
    {
        AnalogInput<unsigned int>* ain;
        double min_meas;
        double max_meas;
        double scale;
        double offset;
        public:
        /**
         * Create an AnalogSensor, connected to an AnalogInput, with a minimal and maximal
         * measurement value. A scale and offset are used to convert the analog signal
         * to the measurement.
         */
        AnalogSensor(AnalogInput<unsigned int>* _ain, double _min_meas, double _max_meas, double _scale=1.0, double _offset=0.0)
            : ain( _ain ),
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
