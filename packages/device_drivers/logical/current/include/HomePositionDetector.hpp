/***************************************************************************
  tag: Peter Soetens  Tue May 4 16:49:13 CEST 2004  HomePositionDetector.hpp 

                        HomePositionDetector.hpp -  description
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
 
 
#ifndef HOME_POSITION_DETECTOR_HPP
#define HOME_POSITION_DETECTOR_HPP

#include <device_interface/DigitalInInterface.hpp>
#include <device_interface/SensorInterface.hpp>
#include <device_interface/EncoderInterface.hpp>

namespace ORO_DeviceDriver
{

    /**
     * @brief This class represents a virtual Digital Input device
     * with three bits denoting the Homing Status.
     *
     * The first bit (0) is on when the
     * physical position is zero, the next bit (1) is on
     * when the encoder position is zero, the third bit (2)
     * is on when the encoder turn is zero. The encoder is optional.
     */
    class HomePositionDetector
        : public ORO_DeviceInterface::DigitalInInterface
    {
        ORO_DeviceInterface::SensorInterface<double>* sens;
        ORO_DeviceInterface::EncoderInterface* enc;
    public :
        /**
         * Create a Home Position Detector with a sensor and an optional
         * encoder.
         * @param _sensor The sensor returning zero when the physical home position is reached.
         * @param _encoder The encoder measuring the position.
         */
        HomePositionDetector( ORO_DeviceInterface::SensorInterface<double>* _sensor,
                              ORO_DeviceInterface::EncoderInterface* _encoder = 0)
            : sens(_sensor), enc(_encoder) {}

        virtual bool isOn( unsigned int bit = 0) const
        {
            if ( bit == 0)
                return sens->readSensor() == 0.0;
            if ( bit == 1)
                return enc != 0 && enc->positionGet() == 0;
            if ( bit == 2)
                return enc != 0 && enc->turnGet() == 0;
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
            if (start_bit > 2 || stop_bit > 2 )
                return 0;
            // first form the total sequence, then shift + mask
            return (( isOn(0) + isOn(1)*2 + isOn(2)*4 ) >> start_bit ) & (1 << (stop_bit+1)-1) ;
        }
        virtual unsigned int nbOfInputs() const
        { return enc != 0 ? 3 : 1; }
    };

}

#endif
