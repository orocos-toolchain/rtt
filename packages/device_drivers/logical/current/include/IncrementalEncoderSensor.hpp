/***************************************************************************

                        IncrementalEncoderSensor.hpp -  description
                           -------------------
    begin                : Tue Oct 26 2004
    copyright            : (C) 2004 Johan Rutgeerts
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
 
#ifndef INCREMENTAL_ENCODER_SENSOR_HPP
#define INCREMENTAL_ENCODER_SENSOR_HPP

#include <device_interface/EncoderInterface.hpp>
#include <device_interface/SensorInterface.hpp>
#include <device_interface/CalibrationInterface.hpp>

#include <limits>
#include <iostream>

namespace ORO_DeviceDriver
{
    /**
     * @brief A sensor reading a single Incremental Encoder and converting
     * the counter to a physical unit, with support for calibration.
     */
    class IncrementalEncoderSensor
        : public ORO_DeviceInterface::SensorInterface<double>
    {
        ORO_DeviceInterface::EncoderInterface* enc;
        double unit_to_inc;
        double min;
        double max;
        double posOffset;
        bool calibrated;
        double calPos;
        int resolution;
      

    public:
        /** 
         * @brief Create a new EncoderInterface to SensorInterface Object.
         * 
         * @param _enc   The Encoder to use
         * @param _unit_to_inc Conversion of physical units to increments (e.g. increments / rad )
         * @param _calPos Position (in physical units, e.g. [rad]) where calibrate() will be called 
         * @param _minpos The minimal, physical position, after calibration
         * @param _maxpos The maximal, physical position, after calibration
         * 
         */
        IncrementalEncoderSensor(ORO_DeviceInterface::EncoderInterface* _enc, double _unit_to_inc, double _calPos, double _minpos, double _maxpos, int _resolution)
	  : enc(_enc), unit_to_inc(_unit_to_inc), min(_minpos), max(_maxpos), posOffset(0), calibrated(false), calPos(_calPos), resolution(_resolution)
        {}

        virtual int readSensor( double& p ) const
        {
            p = readSensor();
            return 0;
        }

        /**
         * Set the minimal and maximal position.
         */
        void limit(double _min, double _max) 
        {
            min = _min;
            max = _max;
        }

        virtual void calibrate() 
        {
            if( enc->upcounting() )
            {
                posOffset = calPos - ( enc->turnGet() * resolution + enc->positionGet() ) / unit_to_inc;
            }
            else
            {
                posOffset = calPos - ( enc->turnGet() * resolution + ( resolution - enc->positionGet() ) ) / unit_to_inc;
            }

            this->calibrated = true;
        }

        virtual void unCalibrate()
        {
            this->calibrated = false;
        }

        virtual bool isCalibrated() const
        {
            return this->calibrated;
        }

        virtual double readSensor() const
        {
            if ( enc->upcounting() )
	      return ( enc->turnGet() * resolution + enc->positionGet() ) / unit_to_inc - posOffset;
            else
	      return ( enc->turnGet() * resolution + ( resolution - enc->positionGet() ) ) / unit_to_inc - posOffset;
	}
        
        virtual void writeSensor(double q) const
        {
	  enc->positionSet((int)(unit_to_inc * q));
	}
      
        virtual double maxMeasurement() const
        {
            return calibrated ? max : std::numeric_limits<double>::max();
        }

        virtual double minMeasurement() const
        {
            // ::min() is the <smallest> number ( cfr 0 )
            return calibrated ? min : -std::numeric_limits<double>::max();
        }

        virtual double zeroMeasurement() const
        {
            return 0.0;
        }
    };
}

#endif
