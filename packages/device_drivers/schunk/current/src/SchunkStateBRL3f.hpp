/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:44 CEST 2002  SchunkStateBRL3f.hpp 

                        SchunkStateBRL3f.hpp -  description
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
 
 

#ifndef SCHUNKSTATEBRL3F_HPP
#define SCHUNKSTATEBRL3F_HPP

#include "SchunkState.hpp"
#include "SchunkDevice.hpp"

namespace CBDeviceDriver
{

    /**
     * Schunk state for binary data format, all force/torque values.
     */

    class SchunkStateBRL3f : public SchunkState
    {

        private:
            void sendCommand();
            void processResult( SchunkData* data );

        public:
            SchunkStateBRL3f( SchunkDevice *device );
            void getLine( SchunkData* data );
            void imposeState();
    };
};


#endif // SCHUNKSTATEBRL3F_HPP

