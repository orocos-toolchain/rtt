/***************************************************************************
$Id: $


                        PositionInterface.hpp -  description
                           -------------------
    begin                : <2004-03-09 13:46:54 klaas>
    copyright            : (C) 2004 klaas gadeyne
    email                : klaas.gadeyne@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef POSITIONINTERFACE_HPP
#define POSITIONINTERFACE_HPP

#include <vector>

namespace ORO_DeviceInterface
{

    /**
     * An interface that describes the general positionsenor
     * You can read a value/structure
     *
     * The OutputData is the actual SI unit of the measured data
     * (e.g. force, velocity,...)
     */
    template <class PositionData>
    class PositionInterface
    {
        public:
        typedef PositionData Data;
      
      virtual ~PositionInterface(){};

        /**
         * Read T, the structure that this sensor 'exports'
         * @return 0 on success
         * @return -1 on failure
         */
        virtual int PositionGet(std::vector<Data> & p ) = 0;

        /**
         * Returns the maximum value this sensor can read
         */
      //  virtual Data maxMeasurement() = 0;

        /**
         * Returns the minimum value this sensor can read
         */
        // virtual Data minMeasurement() = 0;

        /**
         * Returns the value which would be given with the sensor in ideal
         * rest.
         */
        // virtual Data zeroMeasurement() = 0;

    };

}

#endif // POSITIONINTERFACE_HPP

