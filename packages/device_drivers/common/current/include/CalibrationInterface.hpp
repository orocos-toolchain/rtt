/***************************************************************************
  tag: Peter Soetens  Tue May 4 16:49:13 CEST 2004  CalibrationInterface.hpp 

                        CalibrationInterface.hpp -  description
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
 
 
#ifndef CALIBRATION_INTERFACE_HPP
#define CALIBRATION_INTERFACE_HPP

namespace ORO_DeviceDriver
{
    /**
     * @brief A class which will calibrate a sensor. It is
     * useful for making 'calibration' objects.
     */
    class CalibrationInterface
    {
    public:
        virtual ~CalibrationInterface() {}
        virtual void calibrate() = 0;
    };
}

#endif
