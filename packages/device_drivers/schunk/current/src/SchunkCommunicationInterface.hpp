/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:43 CEST 2002  SchunkCommunicationInterface.hpp 

                        SchunkCommunicationInterface.hpp -  description
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
 
 

#ifndef SCHUNKCOMMUNICATIONINTERFACE_HPP
#define SCHUNKCOMMUNICATIONINTERFACE_HPP

namespace ORO_DeviceDriver
{
    //using namespace ORO_CoreLib;

    class SchunkCommunicationInterface
    {

        public:
            /**
             * Sends <count> bytes from <buf> to the output stream.
             * Always flush the inputbuffer before you send. That
             * way you are sure of the position of the answer of the
             * controller in the buffer.
             *
             * @return the number of bytes actually send
             */
            virtual int send( char* buf, int count ) = 0;

            /**
             * Flushes the input buffer.
             */
            virtual int flushInput() = 0;

            /**
             * Reads <count> bytes from the input buffer.
             * This must be a blocking read that returns all requested 
             * bytes.
             * @return the number of bytes actually read
             */
            virtual int read( char* buf, int count ) = 0;

            virtual ~SchunkCommunicationInterface()
            {}

            ;
    };

};


#endif // SCHUNKCOMMUNICATIONINTERFACE_HPP

