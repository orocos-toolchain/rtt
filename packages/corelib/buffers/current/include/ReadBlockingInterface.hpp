/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  ReadBlockingInterface.hpp 

                        ReadBlockingInterface.hpp -  description
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

#ifndef READBLOCKINGINTERFACE_HPP
#define READBLOCKINGINTERFACE_HPP

#include "ReadInterface.hpp"

namespace ORO_CoreLib
{

    /**
     * This interface describes a blocking read interface for byte data.
     */

    class ReadBlockingInterface : public ReadInterface
    {

        public:
            /**
             * Read <length> bytes into <buf>.
             * This call blocks until all the requested bytes are read.
             *
             * @return -1 on failure,
             *         <length> on success
             */
            virtual int read( char* buf, size_t length ) = 0;

            virtual ~ReadBlockingInterface()
            {}

    }

    ;
}


#endif // READBLOCKINGINTERFACE_HPP

