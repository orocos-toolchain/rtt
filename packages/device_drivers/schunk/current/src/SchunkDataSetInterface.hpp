/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:44 CEST 2002  SchunkDataSetInterface.hpp 

                        SchunkDataSetInterface.hpp -  description
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
 
 

#ifndef SCHUNKDATASETINTERFACE_HPP
#define SCHUNKDATASETINTERFACE_HPP

#include <os/cpp.hpp>

namespace ORO_DeviceDriver
{

    class SchunkDataSetInterface
    {

        public:
            virtual void setOverloaded( bool overload ) = 0;
            virtual void setValue( int idx, int value ) = 0;
            virtual ~SchunkDataSetInterface()
            {}

            ;
    };

};

#endif // SCHUNKDATASETINTERFACE_HPP

