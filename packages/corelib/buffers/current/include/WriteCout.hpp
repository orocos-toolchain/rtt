/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  WriteCout.hpp 

                        WriteCout.hpp -  description
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
 
#ifndef WRITECOUT_HPP
#define WRITECOUT_HPP

namespace ORO_CoreLib
{

    class WriteCout
    {

        public:
        typedef unsigned int size_t;

            /**
             * Write \a length bytes from \a buf to cout.
             *
             * @param \a buf the buffer where the bytes need to be read from
             * @param \a length the number of bytes to be written
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

