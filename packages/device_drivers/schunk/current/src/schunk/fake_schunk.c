/**
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**
 * author  Pascal Deklerck (pascal.deklerck@student.kuleuven.ac.be)
 *   Jef De Geeter (jef@zazof.org)
 * date  July 2001
 *
 * Revision: $Revision: 1.2 $
 * Last edited: $Date: 2002/10/01 17:01:39 $
 * Log:  $Log: fake_schunk.c,v $
 * Log:  Revision 1.2  2002/10/01 17:01:39  pissaris
 * Log:  Major reindent and reformat of sourcecode.
 * Log:
 * Log:  Revision 1.1.1.1  2002/07/12 14:06:13  gdraelan
 * Log:  schunk import
 * Log:  
 * Log:  Revision 1.5  2001/08/07 12:07:16  jef
 * Log:  - added GPL license
 * Log:  - added cvs keywords
 * Log:  
 */

#include "fake_schunk.h"
#include <rtl_time.h>

#define FAKE_SAMPLE_MODE   (unsigned int)0
#define FAKE_CONFIG_MODE   FAKE_SAMPLE_MODE+1

#define ACK "ACK"
#define ACKLEN 3

pthread_t fake_thread;
char fakebuffer[ 1000 ];
int fakebufferlen;
char fakeoutbuf[ 1000 ];
int fakeoutbuflen;

int sample_gen_mode;

void engine_writeout( char*, int );
int engine_data_waiting( void );
int engine_read( char* buf, int len );
void engine_writein( char* command, int len );
void engine_react( void );

rtl_spinlock_t writelock = SPIN_LOCK_UNLOCKED;

int generate_sample( char* sample )
{
    int len;
    strcpy( sample, " 0,    10,     5,    23,    -4,    34,    23\0" );
#ifdef DEBUG

    rtl_printf( "Len = %d\n", len );
#endif

    len = strlen( sample );
    sample[ len ] = 13;
    sample[ len + 1 ] = 0;
#ifdef DEBUG

    rtl_printf( "Added <CR>, len is now %d\n", strlen( sample ) );
#endif

    return strlen( sample );
}


void* generate_data( void* arg )
{

    sensor_t * s;
    char sample[ 30 ];
    int sample_len;

    s = ( sensor_t* ) arg;

    pthread_make_periodic_np( pthread_self(), gethrtime(), 50000000 );

    while ( 1 )
    {

        pthread_wait_np();

        if ( sample_gen_mode == FAKE_SAMPLE_MODE )
        {
            sample_len = generate_sample( sample );
            engine_writeout( sample, sample_len );
        }

        if ( engine_data_waiting() )
        {
            sensor_trigger( s );
        }

    }

    return 0;
}

void fake_schunk_open( sensor_t* s )
{
    pthread_create( &fake_thread, NULL, generate_data, s );
    fakeoutbuflen = 0;
    fakebufferlen = 0;
    sample_gen_mode = FAKE_CONFIG_MODE;
}

void fake_schunk_write( char* command, int len )
{
    engine_writein( command, len );

    if ( strchr( command, 13 ) != NULL )
    {
        engine_react();
        fakebufferlen = 0;
    }
}

int fake_schunk_read( char* buf, int len )
{
    return engine_read( buf, len );
}

void fake_schunk_close( void )
{
    pthread_delete_np( fake_thread );
}

void engine_writeout( char* output, int len )
{
    rtl_spin_lock( &writelock );
    strncpy( fakeoutbuf, ( const char* ) output, len );
    /* fakeoutbuflen += len;
     while (fakeoutbuflen > 1000) fakeoutbuflen -= 1000;
     
    */
    fakeoutbuflen = len;
    // rtl_printf("Fake schunk wrote out <%s>, %d bytes\n", output, len);
    rtl_spin_unlock( &writelock );
}

int engine_data_waiting()
{
    return ( fakeoutbuflen > 0 );
}

int engine_read( char* buf, int len )
{
    int copy_len;
    rtl_spin_lock( &writelock );
    //if (fakeoutbuflen > len) copy_len = len; else copy_len = fakeoutbuflen;
    copy_len = fakeoutbuflen;
    strncpy( buf, ( const char* ) fakeoutbuf, copy_len );
    // niet helemaal juist maar ja... gemakkelijkheid eerst
    fakeoutbuflen = 0;
    // rtl_printf("\nEngine was read, %d bytes copied\n", copy_len);
    rtl_spin_unlock( &writelock );
    return copy_len;
}

void engine_writein( char* command, int len )
{
    strncpy( fakebuffer + fakebufferlen, ( const char* ) command, len );
    fakebufferlen += len;
    rtl_printf( "\nEngine write in, fakebufferlen = %d\n", fakebufferlen );
    rtl_printf( "Engine inbuf is now <%s>\n\n", fakebuffer );
}

void engine_react( void )
{
    int i;
    rtl_printf( "\nEngine reacting" );

    if ( !strncmp( fakebuffer, "CD A", 4 ) )
    {
        rtl_printf( "Engine entering ASCII mode\n" );
        engine_writeout( ACK, ACKLEN );
        engine_writeout( ACK, ACKLEN );
    }

    else
        if ( !strncmp( fakebuffer, "CD B", 4 ) )
        {
            rtl_printf( "Engine entering Binary mode\n" );
            engine_writeout( ACK, ACKLEN );
            engine_writeout( ACK, ACKLEN );
        }

        else
            if ( !strncmp( fakebuffer, "QS", 2 ) )
            {
                rtl_printf( "Engine entering sample mode\n" );
                sample_gen_mode = FAKE_SAMPLE_MODE;
                engine_writeout( ACK, ACKLEN );
                engine_writeout( ACK, ACKLEN );
            }

            else
            {
                rtl_printf( "Engine got this : \n" );
                i = 0;

                while ( i < fakebufferlen )
                {
                    rtl_printf( "Byte %d : %d (%c)\n", i, ( int ) fakebuffer[ i ], fakebuffer[ i ] );
                    i++;
                }

                rtl_printf( "\n" );

                if ( sample_gen_mode == FAKE_SAMPLE_MODE )
                    sample_gen_mode = FAKE_CONFIG_MODE;
            }

    fakebufferlen = 0;
}

