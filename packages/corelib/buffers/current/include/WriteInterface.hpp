/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  WriteInterface.hpp 

                        WriteInterface.hpp -  description
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


#ifndef WRITEINTERFACE_HPP
#define WRITEINTERFACE_HPP

typedef unsigned int size_t;

namespace ORO_CoreLib
{

    /**
     * This interface describes a write interface for byte data.
     */
    class WriteInterface
    {

        public:
            /**
             * Write <length> bytes from <buf>.
             *
             * @param <buf> the buffer containing the bytes to be written 
             * @param <length> the number of bytes to be written
             *
             * @return -1 on failure,
             *         the number of bytes written on success
             */
            virtual int write( const char* buf, size_t length ) = 0;

            /**
             * Write a byte.
             *
             * @param c The byte to be written.
             * @return -1 on failure, zero on success
             */
            virtual int put( char c ) = 0;

            virtual ~WriteInterface()
            {}

    };
}

#endif // WRITEINTERFACE_HPP

