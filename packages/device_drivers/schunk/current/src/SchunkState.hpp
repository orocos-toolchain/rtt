/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:44 CEST 2002  SchunkState.hpp 

                        SchunkState.hpp -  description
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
 
 

#ifndef SCHUNKSTATE_HPP
#define SCHUNKSTATE_HPP

//#include "SchunkDevice.hpp"
#include "SchunkData.hpp"

namespace ORO_DeviceDriver
{

    class SchunkDevice;

    class SchunkState
    {

        private:

        public:
            SchunkState( SchunkDevice *device );
            virtual ~SchunkState()
            {}

            ;

            /**
                * 
                */
            virtual void getLine( SchunkData* data ) = 0;

            /**
             * Make sure the controller is in the state this
             * State represents.
             */
            virtual void imposeState() = 0;

        protected:
            SchunkDevice *dev;
    };
};

#endif // SCHUNKSTATE_HPP

