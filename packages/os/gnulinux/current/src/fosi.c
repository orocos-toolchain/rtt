/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:15 CET 2005  fosi.c 

                        fosi.c -  description
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
 
 
#include <os/fosi.h>
#include <sys/poll.h>
#include <sys/signal.h>
#include <unistd.h>

struct ciovec
{

    struct ciovec * next;

    size_t size;
    char * data;
};

#define MAX_NR_CLIENTSOCKETS 2
struct fifo
{
    int sockethandle;
    int clientsockets[ MAX_NR_CLIENTSOCKETS ];
    fifohandler handler;
    int readable;

    pthread_t thread;

    struct ciovec * head;

    struct ciovec * tail;

    pthread_mutex_t mutex;

    int terminate;
};

#define MAX_NR_FIFOS 64
struct fifo fifos[ MAX_NR_FIFOS ];

void * fifothread( void * arg );

//#define FOSIDEBUG

int rtosf_create( int fnr, size_t bytes )
{

    struct sockaddr_un address;
    char pathname[ UNIX_PATH_MAX ];
    int * intptr;

#ifdef FOSIDEBUG

    printf( "in rtosf_create\n" );
#endif

    // we create a unix domain socket ~/dev/rtfXX
    snprintf( pathname, UNIX_PATH_MAX, "%s/dev/rtf%i", getenv( "HOME" ), fnr );
    // points into environment, we copy it ourselves
    address.sun_family = PF_UNIX;

    strncpy( address.sun_path, pathname, UNIX_PATH_MAX );

    if ( ( fnr < 0 ) || ( fnr >= MAX_NR_FIFOS ) )
        return -1;

    if ( fifos[ fnr ].sockethandle != 0 )
        return -1;

    fifos[ fnr ].sockethandle = socket( PF_UNIX, SOCK_STREAM, 0 ); // protocols don't exist for unix domain sockets

#ifdef FOSIDEBUG

    rtos_printf( "socket reserved for fifo %i : %i\n", fnr, fifos[ fnr ] );

#endif

    bind( fifos[ fnr ].sockethandle, ( struct sockaddr * ) & address, sizeof( address ) );

    listen( fifos[ fnr ].sockethandle, 5 );

    intptr = ( int * ) malloc( sizeof( int ) );

    *intptr = fnr;

    pthread_create( &fifos[ fnr ].thread, NULL, fifothread, intptr );

    return 0;
}

void readinbuffer( struct fifo * f, int i )
{
    char buffer[ 4096 ];
    size_t size;

    size = read( f->clientsockets[ i ], buffer, sizeof( buffer ) );

    f->tail->next = malloc( sizeof( struct ciovec ) );
    f->tail = f->tail->next;
    f->tail->next = NULL;

    f->tail->size = size;
    f->tail->data = malloc( size );

    memcpy( f->tail->data, buffer, size );
}

inline int max( int a, int b )
{
    return ( a < b ? b : a );
};

inline int max3( int a, int b, int c )
{
    return max( max( a, b ), c );
};

void * fifothread( void * arg )
{
    int * argument = ( int * ) arg;
    int fnr = *( ( int * ) arg );
    fd_set inputs;
    int i;

#ifdef FOSIDEBUG

    rtos_printf( "now in thread for fifo : %i, socket #%i\n", fnr, fifos[ fnr ] );
#endif

    pthread_mutex_init( &fifos[ fnr ].mutex, NULL );

    fifos[ fnr ].terminate = 0;

    fifos[ fnr ].head = malloc( sizeof( struct ciovec ) );
    fifos[ fnr ].tail = fifos[ fnr ].head;
    fifos[ fnr ].tail->next = NULL;

    free( argument );

    while ( fifos[ fnr ].terminate == 0 )
    {
        int tus;

        FD_ZERO( &inputs );
#ifdef FOSIDEBUG

        rtos_printf( "socket for fifo %i : %i\n", fnr, fifos[ fnr ].sockethandle );
#endif

        FD_SET( fifos[ fnr ].sockethandle, &inputs );

        if ( ( tus = fifos[ fnr ].clientsockets[ 0 ] ) != 0 )
        {
            FD_SET( tus, &inputs );
#ifdef FOSIDEBUG

            rtos_printf( "including socket #%i\n", fifos[ fnr ].clientsockets[ 0 ] );
#endif

        }

        if ( ( tus = fifos[ fnr ].clientsockets[ 1 ] ) != 0 )
        {
            FD_SET( tus, &inputs );
#ifdef FOSIDEBUG

            rtos_printf( "including socket #%i\n", fifos[ fnr ].clientsockets[ 1 ] );
#endif

        }

        select(
            FD_SETSIZE,
            &inputs, NULL, NULL, NULL );

#ifdef FOSIDEBUG

        rtos_printf( "FIRE\n" );
#endif

        pthread_mutex_lock( &fifos[ fnr ].mutex );       // we will now modify fifos[fnr]

        if ( FD_ISSET( fifos[ fnr ].sockethandle, &inputs ) )
        {
            int i, j;
#ifdef FOSIDEBUG

            rtos_printf( "accepting connection fof fifo #%i on socket #%i!!\n", fnr, fifos[ fnr ] );
#endif

            if ( ( i = accept( fifos[ fnr ].sockethandle, NULL, NULL ) ) < 0 )
            {
                perror( "accept" );
            }

            for ( j = 0; j <= 1; j++ )
                if ( fifos[ fnr ].clientsockets[ j ] == 0 )
                {
                    fifos[ fnr ].clientsockets[ j ] = i;
                    break;
                }

            if ( j == 2 )
                close( i );

#ifdef FOSIDEBUG

            rtos_printf( "accepted connection %i in slot %i!!\n", i, j );

#endif

        }

        for ( i = 0; i < 2; i++ )
        {
            tus = fifos[ fnr ].clientsockets[ i ];

            if ( FD_ISSET( tus, &inputs ) )
            {

                struct pollfd pf;
                pf.fd = fifos[ fnr ].clientsockets[ i ];
                pf.events = POLLIN | POLLHUP;
                poll( &pf, 1, 0 );

                if ( pf.revents && POLLIN )
                {
#ifdef FOSIDEBUG
                    rtos_printf( "calling handler for fifo %i, socket %i : %x\n", fnr, fifos[ fnr ].clientsockets[ i ], fifos[ fnr ].handler );
#endif

                    readinbuffer( &fifos[ fnr ], i );

                    pthread_mutex_unlock( &fifos[ fnr ].mutex );       // we need to unlock this for read to work

                    ( *( fifos[ fnr ].handler ) ) ( fnr, 'r' );

                    pthread_mutex_lock( &fifos[ fnr ].mutex );       // humhum ... and re-lock it for we'll unlock it again
                };

                if ( ( pf.revents && POLLHUP ) && ( ~ ( pf.revents && POLLIN ) ) )
                {
#ifdef FOSIDEBUG
                    rtos_printf( "closed connection %i in clientsocket %i!!\n", fnr, fifos[ fnr ].clientsockets[ i ] );
#endif

                    close( fifos[ fnr ].clientsockets[ i ] );
                    fifos[ fnr ].clientsockets[ i ] = 0;
                };
            };
        };

        pthread_mutex_unlock( &fifos[ fnr ].mutex );       // we will now modify fifos[fnr]
    };

    return NULL;
}

void rtosf_destroy( int fnr )
{
    void * returnvalue;
    char pathname[ UNIX_PATH_MAX ];

#ifdef FOSIDEBUG

    rtos_printf( "rtosf_destroy(%i)\n", fnr );
#endif

    close( fifos[ fnr ].sockethandle );
    fifos[ fnr ].terminate = 1;  // this is the termination condition for the thread
    // NEED to make the thread wake up from select() here
    pthread_kill( fifos[ fnr ].thread, SIGUSR1 );
    pthread_join( fifos[ fnr ].thread, &returnvalue );
    // wait for the threads termination
    // pthread_join
    snprintf( pathname, UNIX_PATH_MAX, "%s/dev/rtf%i", getenv( "HOME" ), fnr );
    unlink( pathname );
}

int rtosf_get( int fnr, char * msg, size_t bytes )
{
#ifdef FOSIDEBUG
    rtos_printf( "rtosf_get(%i,%x,%u)\n", fnr, msg, bytes );
#endif

    if ( fifos[ fnr ].head == fifos[ fnr ].tail )
    {
#ifdef FOSIDEBUG
        rtos_printf( "nothing to read\n" );
#endif

        return 0; // nothing to read
    }

    if ( bytes == 0 )
        return 0; // likewise

    pthread_mutex_lock( &fifos[ fnr ].mutex );       // we will now modify fifos[fnr]

    // either the msg is large enough, and we can copy the entire ciovec
    if ( bytes >= fifos[ fnr ].head->next->size )
    {

        struct ciovec * tus;
        size_t size;

        size = fifos[ fnr ].head->next->size;
        memcpy( msg, fifos[ fnr ].head->next->data, size );

        tus = fifos[ fnr ].head->next->next;

        if ( tus == 0 )
            tus = fifos[ fnr ].head;       // lijst is leeg

        free( fifos[ fnr ].head->next->data );

        free( fifos[ fnr ].head->next );

        fifos[ fnr ].head->next = tus;

        pthread_mutex_unlock( &fifos[ fnr ].mutex );  // unlock the mutex

        return size;   // return number of bytes "read"
    }

    else
    {
        // or it is not, and we need to return the stuff in pieces
        // we will need an additional ciovec to contain the remainder of the data

        struct ciovec * tus;

        tus = malloc( sizeof( struct ciovec ) );
        tus->next = fifos[ fnr ].head->next->next;
        tus->size = fifos[ fnr ].head->next->size - bytes;
        tus->data = malloc( tus->size );
        memcpy( msg, fifos[ fnr ].head->next->data, bytes );
        memcpy( tus->data, fifos[ fnr ].head->next->data + bytes, tus->size );

        free( fifos[ fnr ].head->data );
        free( fifos[ fnr ].head );

        fifos[ fnr ].head = tus;

        pthread_mutex_unlock( &fifos[ fnr ].mutex );  // unlock the mutex
        return bytes;   // return number of bytes "read"
    };

#ifdef FOSIDEBUG

    rtos_printf( "finito rtosf_get" );

#endif

    pthread_mutex_unlock( &fifos[ fnr ].mutex );       // done
}

int rtosf_resize( int fnr, size_t size )
{
    return 0;
}

int rtosf_put( int fnr, const char * text, size_t bytes )
{
    char * rtosf_put_text = ( char * ) malloc( bytes + 1 );
    memcpy( rtosf_put_text, text, bytes );
    // TODO: check if terminator '\0' is needed.
    rtosf_put_text[ bytes ] = '\0';
    write( fifos[ fnr ].clientsockets[ 0 ], text, bytes );
    write( fifos[ fnr ].clientsockets[ 1 ], text, bytes );
    free( rtosf_put_text );
    return 0; // no error checking yet
}

int rtosf_set_handler( int fnr, int ( *h ) ( unsigned int, int ) )
{
#ifdef FOSIDEBUG
    rtos_printf( "Setting handler : %x for fifo %i\n", h, fnr );
#endif

    fifos[ fnr ].handler = h;
    return 0; // no error checking, previous handlers are overwritten
}

void rtosf_remove_all_handlers( int fnr )
{
    fifos[ fnr ].handler = 0;
}
