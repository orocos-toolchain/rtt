/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  WriteInterface.hpp 

                        WriteInterface.hpp -  description
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

#ifndef ORO_CORELIB_WRITEINTERFACE_HPP
#define ORO_CORELIB_WRITEINTERFACE_HPP

namespace ORO_CoreLib
{

    /**
     * This interface describes a write interface for byte data.
     */
    class WriteInterface
    {
        typedef unsigned int size_t;

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

