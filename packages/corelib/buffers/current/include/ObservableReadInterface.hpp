/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  ObservableReadInterface.hpp 

                        ObservableReadInterface.hpp -  description
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
 
 
/*

   begin           : Tue Aug 13 2002
   email           : Jan.Veldeman@student.kuleuven.ac.be


***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef OBSERVABLEREADINTERFACE_HPP
#define OBSERVABLEREADINTERFACE_HPP

#include "ReadNonBlockingInterface.hpp"
#include "EventInterfaces.hpp"

namespace ORO_CoreLib
{

    // TODO: split registration in separate interface

    /**
     * This interface describes a non blocking read interface with the
     * possibility to be notified for data-ready events.
     */

    class ObservableReadInterface :
        public ReadNonBlockingInterface,
        public HandlerRegistrationInterface
    {

        public:
            virtual ~ObservableReadInterface()
            {}

    };

}

#endif // OBSERVABLEREADINTERFACE_HPP


