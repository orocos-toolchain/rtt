/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  ReadInterface.hpp 

                        ReadInterface.hpp -  description
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
 

#ifndef ORO_CORELIB_READINTERFACE_HPP
#define ORO_CORELIB_READINTERFACE_HPP

namespace ORO_CoreLib
{

    /**
     * This interface describes a read interface for byte data.
     */
    class ReadInterface
    {
    protected:
        typedef unsigned int size_t;
    public:
        /**
         * Read <length> bytes into <buf>.
         *
         * @param <buf> the buffer where the bytes needs to be written.
         *              This must garantee to have at least a capacity of
         *              <length> bytes.
         * @param <length> the number of bytes to be read
         *
         * @return -1 on failure,
         *         the number of bytes read on success
         */
        virtual int read( char* buf, size_t length ) = 0;

        /**
         * Read one byte.
         *
         * @param c the result of the read.
         * @return -1 if reading failed, zero otherwise.
         */
        virtual int get( char& c ) = 0;
            
        virtual ~ReadInterface()
        {}

    };
}

#endif // READINTERFACE_HPP

