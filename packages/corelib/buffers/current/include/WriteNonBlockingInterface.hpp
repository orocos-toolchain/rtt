/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  WriteNonBlockingInterface.hpp 

                        WriteNonBlockingInterface.hpp -  description
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

   begin           : Tue Aug 6 2002
   email           : Jan.Veldeman@student.kuleuven.ac.be


***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef WRITENONBLOCKINGINTERFACE_HPP
#define WRITENONBLOCKINGINTERFACE_HPP

#include "WriteInterface.hpp"

namespace ORO_CoreLib
{

    /**
     * This interface describes a non blocking write interface for byte data.
     * It 's implementation must guarantee that execution of any member of this
     * class doesn't sleep.
     */

    class WriteNonBlockingInterface : public WriteInterface
    {

        public:
            virtual ~WriteNonBlockingInterface()
            {}

    }

    ;

}

#endif // WRITENONBLOCKINGINTERFACE_HPP


