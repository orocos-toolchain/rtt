/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:43 CEST 2002  SchunkData.hpp 

                        SchunkData.hpp -  description
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
 
// Jan Veldeman
// 2-8-2002

#ifndef SCHUNKDATA_HPP
#define SCHUNKDATA_HPP

#include "SchunkDataReadInterface.hpp"
#include "SchunkDataSetInterface.hpp"

namespace ORO_DeviceDriver
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
