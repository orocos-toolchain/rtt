/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  iostream.hpp 

                        iostream.hpp -  description
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
 
/*
* Project: rtai_cpp - RTAI C++ Framework 
*
* File: $Id: iostream.hpp,v 1.6 2002/10/10 14:18:36 psoetens Exp $
*
* Copyright: (C) 2001,2002 Erwin Rol <erwin@muffin.org>
*
* Licence:
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

#ifndef __IOSTREAM_H__
#define __IOSTREAM_H__

namespace std
{

    class ostream;
    typedef ostream& ( *__omanip ) ( ostream& );

    /**
     * printk Helper Class
     */

    class ostream
    {

        public:
            ostream();
            virtual ~ostream();

            ostream& set_output_on( bool f );

            ostream& put( char c );
            ostream& put( unsigned char c )
            {
                return put( ( char ) c );
            }

            ostream& put( signed char c )
            {
                return put( ( char ) c );
            }

            ostream& write( const char *s );
            ostream& write( char s );
            ostream& write( char const* s,int );
            ostream& write( const unsigned char *s )
            {
                return write( ( const char* ) s );
            }

            ostream& write( const signed char *s )
            {
                return write( ( const char* ) s );
            }

            ostream& operator<<( const char* s );
            ostream& operator<<( char s );
            ostream& operator<<( const void* p );

            ostream& operator<<( unsigned short n );
            ostream& operator<<( unsigned int n );
            ostream& operator<<( unsigned long n );
            ostream& operator<<( unsigned long long n );

            ostream& operator<<( short n );
            ostream& operator<<( int n );
            ostream& operator<<( long n );
            ostream& operator<<( long long n );

            ostream& operator<<( float f );
            ostream& operator<<( double f );
            ostream& operator<<( long double f );

            ostream& operator<<( __omanip func )
            {
                return ( *func ) ( *this );
            }

        protected:
            bool m_OutputOn;
    };

    extern ostream& endl( ostream& s );
    extern ostream& output_off( ostream& s );
    extern ostream& output_on( ostream& s );

    extern ostream cerr;
    extern ostream cout;

}

; // namespace std

#endif
