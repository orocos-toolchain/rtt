
#include "os/rtconversions.hpp"

#include <string>

using namespace std;

/**
 * converts a string to an int
 * 
 * @param s
 *    string to be converted
 * @return the integer value of <s>
 * @return 0 when faulty <s> found
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

    snprintf( buffer, 127, "%c%d.%03d", s, pre, post );

    return string(buffer);
}

string unsigned_int_to_string( unsigned int u )
{
    return int_to_string( u );
}
