/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:43 CEST 2002  SchunkCommunicationFile.hpp 

                        SchunkCommunicationFile.hpp -  description
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
 
 

// Jan.Veldeman@student.kuleuven.ac.be
// 7-8-2002

/*
 * schunk communication through unix files (/dev/ttyS0)
 */

#ifndef SCHUNKCOMMUNICATIONFILE_HPP
#define SCHUNKCOMMUNICATIONFILE_HPP

#include <corelib/SchunkCommunicationInterface.hpp>

extern "C"
{
#include <stdio.h>
}

namespace CBDeviceDriver
{

    class SchunkCommunicationFile : public SchunkCommunicationInterface
    {

        public:
            SchunkCommunicationFile();
            ~SchunkCommunicationFile();
            int send( char* buf, int count );
            int flushInput();
            int read( char* buf, int count );

        private:
            FILE* _file;
    };
}

#endif // SCHUNKCOMMUNICATIONFILE_HPP

