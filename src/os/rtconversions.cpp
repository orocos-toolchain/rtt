/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  rtconversions.cxx

                        rtconversions.cxx -  description
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

#include "os/rtconversions.hpp"

#include <string>
#include <cstdio>
#include <cstdlib>
#include <stdio.h>

using namespace std;

/**
 * converts a string to an int
 *
 * @param s
 *    string to be converted
 * @return the integer value of \a s
 * @return 0 when faulty \a s found
 */
int string_to_int( const string& s )
{
    bool neg = false;
    int hulp = 0;

    for ( unsigned int i = 0; i < s.size(); i++ )
    {
        if ( ( i == 0 ) && ( s[ 0 ] == '-' ) )
        {
            neg = true;
            continue;
        }
        if ( ( s[ i ] >= '0' ) && ( s[ i ] <= '9' ) )
        {
            hulp *= 10;
            hulp += ( s[ i ] - '0' );
            continue;
        }
        else
        {
            // error detected
            return 0;
        }
    }

    if ( neg )
        hulp *= -1;

    return hulp;
}

// float string_to_float(const string& s)
// {
//  float f;
//  sscanf(s.c_str(),"%f", &f);
//  return f;
// };

char string_to_char( const string& s )
{
    if ( s.length() > 0 )
    {
        return s[ 0 ];
    }

    else
    {
        return '0';
    }

}

unsigned int string_to_unsigned_int( const string& s )
{
    unsigned int hulp = 0;

    for ( unsigned int i = 0; i < s.size(); i++ )
    {
        if ( ( s[ i ] >= '0' ) && ( s[ i ] <= '9' ) )
        {
            hulp *= 10;
            hulp += ( s[ i ] - '0' );
            continue;
        }

        break;
    }

    return hulp;
}

string int_to_string( int i )
{
    string hulp;

    if ( i < 0 )
    {
        hulp += '-';
        i *= -1;
    }

    if ( i == 0 )
    {
        hulp = "0";
        return hulp;
    }

    int t = 1000000000;
    bool start = true;

    for ( int j = 0; t != 0; j++ )
    {
        if ( start )
        {
            if ( ( i / t ) == 0 )
            {
                t /= 10;
                continue;
            }

            else
            {
                start = false;
            }
        }

        hulp += ( '0' + ( i / t ) );
        i %= t;
        t /= 10;
    }

    return hulp;
}

string float_to_string(float f)
{
    char buffer[ 128 ];
    buffer[ 127 ] = '\0';
    char s = ' '; // sign of integer
    int pre = int( f ); // voor de comma
    int post = int( ( f - pre ) * 1000.0 ); // na de comma

    if ( post != 0 )
        post = abs( post );

    if ( pre == 0 && f < 0.0 )
        s = '-';
    else
        s = ' ';

    //snprintf( buffer, 127, "%c%d.%03d", s, pre, post );
	//replacement with MSVC`s snprintf but it is not ALWAYS null terminating
#ifdef _MSC_VER
	_snprintf( buffer, 127, "%c%d.%03d", s, pre, post );
#else
	snprintf( buffer, 127, "%c%d.%03d", s, pre, post );
#endif
    return string(buffer);
}

string unsigned_int_to_string( unsigned int u )
{
    return int_to_string( u );
}
