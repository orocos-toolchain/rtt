/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  ForceTorqueInterface.hpp 

                        ForceTorqueInterface.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 

#ifndef FORCETORQUESENSOR_INTERFACE_HPP
#define FORCETORQUESENSOR_INTERFACE_HPP

#include "SensorInterface.hpp"

namespace ORO_DeviceInterface
{

    /**
     * This interface extends the sensor interface to set the frequency to read,
     * amongst others.
     */
    template <class T>
    class ForceTorqueSensorInterface :
                public SensorInterface<T>
    {
        public:

            /**
             * sets the sample period to read the forcesensor
             */
            virtual void setSamplePeriod(float T)            = 0;

    };

};

#endif 

