/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:15 CET 2005  utils.h 

                        utils.h -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 
#ifndef UTILS_H
#define UTILS_H

/*
 * need this for rtti and so on, you can remove it if you find
 * a better one or in another header file an implementation 
 * PS 
 */ 
/*
inline int strcmp(const char* a, const char* b)
{
    while (*a)
        if (*a++ != *b++) return 0;
    return 1;
}*/

inline unsigned int powertwo( unsigned int power )
{
    return ( unsigned int ) 1 << power;
}

inline char * bin( unsigned int i )
{
#define UTILS_BIN_RESULT_STRING_SIZE 41 /* 32 bits + 8 spaces (every 4 bits) + \0 = 41 */
    static char result[ UTILS_BIN_RESULT_STRING_SIZE ];     
    int bit, write_pos;
    write_pos = 0;
    result[ UTILS_BIN_RESULT_STRING_SIZE - 1 ] = '\0';

    for ( bit = 31; bit >= 0; bit-- )
    {
        result[ write_pos++ ] = ( i & powertwo( bit ) ? '1' : '0' );

        if ( ( bit % 4 ) == 0 )
            result[ write_pos++ ] = ' ';
    }

    return result;
}

/* returns true if bit "bitnr" is equal in value1 and value2 */
inline int bitIsEqual( unsigned long value1, unsigned long value2, unsigned int bitnr )
{
    unsigned int bitmask = powertwo( bitnr );
    return ( value1 & bitmask ) == ( value2 & bitmask );
}

#endif /* UTILS_H */
