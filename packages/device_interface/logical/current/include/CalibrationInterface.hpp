/***************************************************************************
  tag: Peter Soetens  Tue May 4 16:49:13 CEST 2004  CalibrationInterface.hpp 

                        CalibrationInterface.hpp -  description
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
 
#ifndef CALIBRATION_INTERFACE_HPP
#define CALIBRATION_INTERFACE_HPP

namespace ORO_DeviceInterface
{
    /**
     * @brief A class which will calibrate a sensor. It is
     * useful for making 'calibration' objects.
     * @see SensorInterface
     */
    class CalibrationInterface
    {
    public:
        virtual ~CalibrationInterface() {}
        /**
         * @brief Start or perform calibration.
         */
      virtual void calibrate() {}

        /** 
         * @brief Inspect if a calibration has been done.
         * 
         * 
         * @return True if so.
         */
      virtual bool isCalibrated() const { return true; }

        /** 
         * @brief Undo any previous calibration
         * ( this function may have no effect ).
         */
      virtual void unCalibrate() {}
    };
}

#endif
