/***************************************************************************
  tag: Peter Soetens  Tue May 4 16:49:13 CEST 2004  EncoderPositionSensor.hpp 

                        EncoderPositionSensor.hpp -  description
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
 
#ifndef ENCODER_POSITION_SENSOR_HPP
#define ENCODER_POSITION_SENSOR_HPP

#include <device_interface/SensorInterface.hpp>
#include <device_interface/CalibrationInterface.hpp>

#include <limits>

namespace ORO_DeviceDriver
{
    using ORO_DeviceInterface::SensorInterface;
    /**
     * @brief A sensor reading a single Encoder and converting
     * the counter to a physical unit, with support for calibration.
     */
    class EncoderPositionSensor
        : public SensorInterface<double>,
          public CalibrationInterface
    {
        EncoderInterface* enc;
        double unit_to_inc;
        double min;
        double max;
        double posOffset;
        bool calibrated;
        double cal_pos;
        double cal_dir;
    public:
        /** 
         * @brief Create a new EncoderInterface to SensorInterface Object.
         * 
         * @param _enc   The Encoder to use
         * @param _unit_to_inc Conversion of physical units to increments (eg increments / mm )
         * @param _minpos The minimal, physical position, after calibration
         * @param _maxpos The maximal, physical position, after calibration
         * 
         */
        EncoderPositionSensor(EncoderInterface* _enc, double _unit_to_inc, double _minpos, double _maxpos)
            : enc(_enc), unit_to_inc(_unit_to_inc), min(_minpos), max(_maxpos), posOffset(0), calibrated(false),
              cal_pos(0), cal_dir(-1)
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
        /** 
         * @brief Set the calibration position of this encoder
         * 
         * @see \a calibrate()
         * @param pos The value the encoder will read out on a
         * calibrated position.
         */
        void calibrationPosition( double pos )
        {
            cal_pos = pos;
        }
        /** 
         * @brief Set the direction in which the calibration
         * will be done.
         *
         * @see \a calibrate()
         * @param dir A positive or negative value.
         */
        void calibrationDirection( double dir )
        {
            cal_dir = dir;
        }

        virtual void calibrate() 
        {
            this->calibrate( cal_pos, cal_dir );
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
            return double( enc->turnGet()*enc->resolution() +  enc->positionGet() ) / unit_to_inc + posOffset;
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

        /**
         * @brief Calibrate the Axis with a given calibration position
         * and direction of the movement.
         *
         * A calibration position is
         * defined by a \a positionGet() == 0 (thus, the encoder count is zero),
         * for any turn \a turnGet() == k. You may
         * call this method only when the encoder is no more than a turn
         * distance away from the intended calibration point. After calibration,
         * \a readSensor() == \a cal_pos and \a isCalibrated() == \a true .
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
