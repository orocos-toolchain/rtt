/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  SensorInterface.hpp 

                        SensorInterface.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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
 

#ifndef SENSORINTERFACE_HPP
#define SENSORINTERFACE_HPP

#include "CalibrationInterface.hpp"

#pragma interface

namespace ORO_DeviceInterface
{

    /**
     * @brief An interface that describes the general senor
     * You can read a value/structure
     *
     * The OutputData is the actual SI unit of the measured data
     * (e.g. force, velocity,...) and is in case of a 1D sensor a double.
     *
     * If the sensor is not callibrated, the \a minMeasurement() and
     * \a maxMeasurement() functions should return std::numeric_limits<SensorData>::min()
     * and std::numeric_limits<SensorData>::max() respectively.
     * @see CalibrationInterface
     */
    template <class SensorData>
    class SensorInterface
      : public CalibrationInterface
    {
        public:
        virtual ~SensorInterface()
        {}

        typedef SensorData DataType;

        /**
         * Read the Data, the structure that this sensor 'exports'
         * @return 0 on success
         * @return -1 on failure
         */
        virtual int readSensor( DataType& p ) const = 0;

        /**
         * Return the last measurement.
         */
        virtual DataType readSensor() const = 0;

        /**
         * Returns the maximum value this sensor can read.
         */
        virtual DataType maxMeasurement() const = 0;

        /**
         * Returns the minimum value this sensor can read.
         */
        virtual DataType minMeasurement() const = 0;

        /**
         * Returns the value which would be given with the sensor in ideal
         * rest.
         */
        virtual DataType zeroMeasurement() const = 0;

    };

}

#endif // SENSORINTERFACE_HPP

