/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:44 CEST 2002  SchunkState.hpp 

                        SchunkState.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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

