/***************************************************************************
  tag: Peter Soetens  Tue May 4 16:49:13 CEST 2004  EncoderPositionSensor.hpp 

                        EncoderPositionSensor.hpp -  description
                           -------------------
    begin                : Tue May 04 2004
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
 
 
#ifndef ENCODER_POSITION_SENSOR_HPP
#define ENCODER_POSITION_SENSOR_HPP

#include <device_interface/SensorInterface.hpp>

#include <limits>

namespace ORO_DeviceDriver
{
    using ORO_DeviceInterface::SensorInterface;
    /**
     * @brief A sensor reading a single Encoder and converting
     * the counter to a physical unit.
     */
    class EncoderPositionSensor
        : public SensorInterface<double>
    {
        EncoderInterface* enc;
        double unit_to_inc;
        double min;
        double max;
        double posOffset;
        bool calibrated;
    public:
        EncoderPositionSensor(EncoderInterface* _enc, double _unit_to_inc, double _minpos, double _maxpos)
            : enc(_enc), unit_to_inc(_unit_to_inc), min(_minpos), max(_maxpos), posOffset(0), calibrated(false) {}

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

        virtual double readSensor() const
        {
            return double( enc->turnGet()*enc->resolution() +  enc->positionGet() ) / unit_to_inc + posOffset;
        }

        virtual double maxMeasurement() const
        {
            return calibrated ? max : std::numeric_limits<double>::max();
        }

        virtual double minMeasurement() const
        {
            return calibrated ? min : std::numeric_limits<double>::min();
        }

        virtual double zeroMeasurement() const
        {
            return 0.0;
        }

        /**
         * @brief Calibrate the Axis with a given calibration position
         * and direction of the movement.
         *
         * A calibration position is
         * defined by a positionGet() == 0. You may
         * call this method only when the encoder is no more than a turn
         * distance away from the calibration point.
         * @verbatim
         * ...|..........|..........|..........| : | = turn increment, . = position increment/decrement
         *    ^turn==N-1 ^ turn==N  ^ turn==N+1
         *               ^ calpos = x mm
         *     |-------------------|  : valid range physical position when calling this method.
         *        
         *            ---->           : passed calpos during positive movement, sign = +1
         *            <----           : passed calpos during negative movement, sign = -1
         * @endverbatim
         * @param calpos The physical position of the calibrated
         *               zero position counter.
         * @param sign   Is the calpos left or right from the current position.
         *        (meaning : write +1 if you
         *        were moving with a positive velocity, write -1 if you were
         *        moving with a negative velocity.)
         * @return The change in position caused by this calibration.
         */
        double calibrate( double calpos, double sign )
        {
            double currentPos;
            if ( sign < 0 )
                {
                    if ( unit_to_inc > 0)
                        currentPos = calpos - ( enc->resolution() - enc->positionGet() ) / unit_to_inc;
                    else
                        currentPos = calpos - enc->positionGet() / unit_to_inc;
                }
            else
                {
                    if ( unit_to_inc > 0)
                        currentPos =  calpos + enc->positionGet() / unit_to_inc;
                    else
                        currentPos = calpos + (enc->resolution() - enc->positionGet() ) / unit_to_inc;
                }
            double oldposition = readSensor();
            posOffset = currentPos -  (readSensor() - posOffset); //calculate the offset
            calibrated = true;
            return readSensor() - oldposition;
        }
    };

}

#endif
