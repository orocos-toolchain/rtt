/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  WriteCout.hpp 

                        WriteCout.hpp -  description
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
 
 
#ifndef WRITECOUT_HPP
#define WRITECOUT_HPP

#include "WriteInterface.hpp"

namespace ORO_CoreLib
{

    class WriteCout :
                public WriteInterface
    {

        public:
            /**
             * Write <length> bytes from <buf> to cout.
             *
             * @param <buf> the buffer where the bytes need to be read from
             * @param <length> the number of bytes to be written
             *
             * @return -1 on failure,
             *         the number of bytes written on success
             */
            virtual int write( const char* buf, size_t length );

            virtual int put( char c );

            /**
             * Construct the WriteCout object.
             */
            WriteCout();

            /**
             * Destroy the WriteCout object.
             */
            virtual ~WriteCout();
    };
}

#endif // WRITECOUT_HPP

