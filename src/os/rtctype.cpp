/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  rtctype.cxx 

                        rtctype.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
const int alpha=0x01;
const int upper=0x02;
const int lower=0x04;
const int digit=0x08;
const int space=0x10;
const int cntrl=0x20;
const int punct=0x40;
const int print=0x80;
const int xdigit=0x100;

int values[ 256 ];

class values_init
{

    public:
        values_init()
        {
            for ( int i = 'A'; i <= 'Z'; i++ )
                values[ i ] = alpha + upper + print;

            for ( int i = 'a'; i <= 'z'; i++ )
                values[ i ] = alpha + lower + print;

            for ( int i = '0'; i <= '9'; i++ )
                values[ i ] = digit + print;

            values[ int(',') ] = punct + print;

            values[ int(';') ] = punct + print;

            values[ int('.') ] = punct + print;

            values[ int(':') ] = punct + print;

            values[ int('!') ] = punct + print;

            values[ int('/') ] = punct + print;

            values[ int('?') ] = punct + print;

            values[ int('\'') ] = punct + print;

            values[ int('"') ] = punct + print;

            values[ int('-') ] = punct + print;

            values[ int('+') ] = punct + print;

            values[ int('(') ] = punct + print;

            values[ int(')') ] = punct + print;
        }
};

values_init values_init_instance; // call boem's constructor on init

inline int isalpha( int c )
{
    return values[ c ] & alpha;
}

inline int isupper( int c )
{
    return values[ c ] & upper;
}

inline int islower( int c )
{
    return values[ c ] & lower;
}

inline int isdigit( int c )
{
    return values[ c ] & digit;
}

#undef isxdigit
inline int isxdigit( int c )
{
    return values[ c ] & digit;
} // FIXME

inline int isspace ( int c )
{
    return values[ c ] & space;
}

inline int iscntrl ( int c )
{
    return values[ c ] & cntrl;
}

inline int ispunct ( int c )
{
    return values[ c ] & punct;
}

inline int isalnum ( int c )
{
    return isalpha( c ) || isdigit( c );
}

inline int isprint ( int c )
{
    return values[ c ] & print;
}

inline int isgraph ( int c )
{
    return true;
} // FIXME

int toupper( int c )
{
    if ( isalpha( c ) && islower( c ) )
        return ( c + 'A' - 'a' );
    else
        return c;
}

int tolower( int c )
{
    if ( isalpha( c ) && isupper( c ) )
        return ( c - ( 'A' - 'a' ) );
    else
        return c;
}
