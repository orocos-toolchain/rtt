#if 0
/***************************************************************************
  tag: Peter Soetens  Tue May 4 16:49:13 CEST 2004  EndPositionDetector.hpp 

                        EndPositionDetector.hpp -  description
                           -------------------
    begin                : Tue May 04 2004
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
 
#ifndef END_LIMIT_DETECTOR_HPP
#define END_LIMIT_DETECTOR_HPP

#include <device_interface/DigitalInInterface.hpp>
#include <device_interface/SensorInterface.hpp>
#include <limits>

namespace ORO_DeviceDriver
{

    /**
     * @brief This class represents a virtual Digital Input device
     * with two bits denoting software end limit reaches. (e.g. end of travel)
     *
     * The first bit (0) is on when the
     * physical limit is less than the minimal limit, the
     * second bit (1) is on when the physical limit is greater
     * than the maximal limit.
     *
     * The sensor's minMeasurement() and maxMeasurement() values are
     * always taken into account.
     */
    class EndLimitDetector
        : public ORO_DeviceInterface::DigitalInInterface
    {
        ORO_DeviceInterface::SensorInterface<double>* sens;
        double minpos;
        double maxpos;
    public :
        /**
         * @brief Create a End Limit Detector with a sensor.
         *
         * @param _sensor The sensor returning a calibrated value.
         * @param _minpos The minimal limit, if omitted, the _sensor->minMeasurement() is taken.
         * @param _maxpos The maximal limit, if omitted, the _sensor->maxMeasurement() is taken.
         */
        EndLimitDetector( ORO_DeviceInterface::SensorInterface<double>* _sensor,
                          double _minpos = -std::numeric_limits<double>::max(),
                          double _maxpos = std::numeric_limits<double>::max() )
            : sens(_sensor), minpos(_minpos), maxpos(_maxpos)
        {}

        virtual bool isOn( unsigned int bit = 0) const
        {
            double pos =  sens->readSensor();
            if ( bit == 0)
                return pos < minpos || pos < sens->minMeasurement();
            if ( bit == 1)
                return pos > maxpos || pos > sens->maxMeasurement();
            return false;
        }

        virtual bool isOff( unsigned int bit = 0) const
        {
            return !isOn(bit);
        }

        virtual bool readBit( unsigned int bit = 0) const
        {
            return isOn(bit);
        }

        virtual unsigned int readSequence(unsigned int start_bit, unsigned int stop_bit) const
        {
            if (start_bit > 1 || stop_bit > 1 )
                return 0;
            // first form the total sequence, then shift + mask
            return (( isOn(0) + isOn(1)*2 ) >> start_bit ) & (1 << (stop_bit+1)-1);
        }

        virtual unsigned int nbOfInputs() const
        { return 2; }
    };

}

#endif
#endif
