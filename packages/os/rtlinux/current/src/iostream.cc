/*
* Project: rtai_cpp - RTAI C++ Framework 
*
* File: $Id: iostream.cc,v 1.7 2002/10/08 09:17:40 rwaarsin Exp $
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

#include "cpp.hpp"
#include "iostream.hpp"

namespace std
{

    ostream cerr;
    ostream cout;

    ostream& endl( ostream& s )
    {
        return s.put( '\n' );
    }

    ostream& output_on( ostream& s )
    {
        return s.set_output_on( true );
    }

    ostream& output_off( ostream& s )
    {
        return s.set_output_on( false );
    }

    ostream::ostream()
            : m_OutputOn( true )
    {}

    ostream::~ostream()
    {}

    ostream& ostream::set_output_on( bool f )
    {
        m_OutputOn = f;

        return *this;
    }

    ostream& ostream::put( char c )
    {
        if ( m_OutputOn )
            rtl_printf( "%c" , c );

        return *this;
    }

    ostream& ostream::write( const char *s )
    {
        if ( m_OutputOn )
            rtl_printf( "%s", s );

        return *this;
    }

    ostream& ostream::write( char const* s,int )
    {
        if ( m_OutputOn )
            rtl_printf( "%s", s );

        return *this;
    }

    ostream& ostream::write( char s )
    {
        if ( m_OutputOn )
            rtl_printf( "%c", s );

        return *this;
    }
    ostream& ostream::operator<<( const char* s )
    {
        return write( s );
        ;
    }

    ostream& ostream::operator<<( char s )
    {
        return write( s );
        ;
    }

    ostream& ostream::operator<<( const void* p )
    {
        if ( m_OutputOn )
            rtl_printf( "%p", p );

        return *this;
    }

    ostream& ostream::operator<<( unsigned short n )
    {
        if ( m_OutputOn )
            rtl_printf( "%d", n );

        return *this;
    }

    ostream& ostream::operator<<( unsigned int n )
    {
        if ( m_OutputOn )
            rtl_printf( "%d", n );

        return *this;
    }

    ostream& ostream::operator<<( unsigned long n )
    {
        if ( m_OutputOn )
            rtl_printf( "%d", n );

        return *this;
    }

    ostream& ostream::operator<<( unsigned long long n )
    {
        if ( m_OutputOn )
            rtl_printf( "<unsigned long long not implemented>" );

        return *this;
    }

    ostream& ostream::operator<<( short n )
    {
        if ( m_OutputOn )
            rtl_printf( "%d", n );

        return *this;
    }

    ostream& ostream::operator<<( int n )
    {
        if ( m_OutputOn )
            rtl_printf( "%d", n );

        return *this;
    }

    ostream& ostream::operator<<( long n )
    {
        if ( m_OutputOn )
            rtl_printf( "%d", n );

        return *this;
    }

    ostream& ostream::operator<<( long long n )
    {
        if ( m_OutputOn )
            rtl_printf( "<long long not implemented>" );

        return *this;
    }

    ostream& ostream::operator<<( float f )
    {
        if ( m_OutputOn )
            rtl_printf( "<float not implemented>" );

        return *this;
    }

    ostream& ostream::operator<<( double f )
    {
        if ( m_OutputOn )
            rtl_printf( "<double not implemented>" );

        return *this;
    }

    ostream& ostream::operator<<( long double f )
    {
        if ( m_OutputOn )
            rtl_printf( "<long double not implemented>" );

        return *this;
    }

} // namespace std

