/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:43 CEST 2002  SchunkData.hpp 

                        SchunkData.hpp -  description
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
 
 
// Jan Veldeman
// 2-8-2002

#ifndef SCHUNKDATA_HPP
#define SCHUNKDATA_HPP

#include "SchunkDataReadInterface.hpp"
#include "SchunkDataSetInterface.hpp"

namespace CBDeviceDriver
{

    class SchunkData :
                public SchunkDataReadInterface,
                public SchunkDataSetInterface
    {

        private:
            bool _overload;
            int data[ 6 ];

        public:
            bool getOverloaded();
            int getXForce();
            int getYForce();
            int getZForce();
            int getXTorque();
            int getYTorque();
            int getZTorque();
            void setOverloaded( bool overload );
            void setValue( int idx, int value );
            ~SchunkData();

    };


}

; // namespace


#endif // SCHUNKDATA_HPP
