/***************************************************************************
  tag: Peter Soetens  Tue May 4 16:49:14 CEST 2004  ArgumentDescription.hpp 

                        ArgumentDescription.hpp -  description
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
 
 
#ifndef ARGUMENTDESCRIPTION_HPP
#define ARGUMENTDESCRIPTION_HPP

#include <string>

namespace ORO_Execution
{
    /**
     * @brief Description of one Argument of a Command.
     */
    struct ArgumentDescription
    {
        ArgumentDescription (const std::string _name, const std::string _desc)
            : name( _name), description(_desc) {}
        std::string name;
        std::string description;
    };
}

#endif
