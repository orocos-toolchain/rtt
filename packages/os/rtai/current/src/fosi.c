
/**
 * Dirty, we do not want the inline functions of string.h here,
 * since here we define the non inline variants (should put
 * this in another file...
 */
//#define _LINUX_STRING_H_

#include "fosi.h"
#include <libio.h>

#define POLL_FREQ 50
#undef current

/**
 * This function waits for a semaphore in a linux kernel module
 * It the semaphore wont be released, your system will hang.
 */
void waitInModule( sem_t* t )
{

    struct rt_task_struct * current = rt_whoami();

    while ( rt_sem_wait_if( t ) < 0 )
    {
        current->state = TASK_INTERRUPTIBLE; // ???
        schedule_timeout( jiffies + ( HZ + POLL_FREQ / 2 - 1 ) / POLL_FREQ );
    }
}

#undef POLL_FREQ 
/**
 * Some functions in libm.a need these when using doubles
 */


int errno_rtai;

typedef struct _IO_FILE FILE;


// stderr is quite null, so be warned
const FILE stderr;


int fputs( const char *s, FILE *stream )
{
    //do nothing for now
    return 0;
}

int __isnan( double d )
{
    return 0;
}

// normally returns errno on a per thread basis
int* __errno_location( void )
{
    return & errno_rtai;
}

/**
 * this is static inline defined in the linux kernel,
 * but when using libm, we need the not inlined version
 *
 * but we dont need libm for rtai since there is rtai_libm, so skip it ?
 */
/*
void * memmove( void * dest, const void * src, size_t n )
{
    int d0, d1, d2;

    if ( dest < src )
        __asm__ __volatile__(
            "rep\n\t"

            "movsb"
    : "=&c" ( d0 ), "=&S" ( d1 ), "=&D" ( d2 )
                    : "0" ( n ), "1" ( src ), "2" ( dest )
                    : "memory" );
    else
        __asm__ __volatile__(
            "std\n\t"
            "rep\n\t"
            "movsb\n\t"

            "cld"
    : "=&c" ( d0 ), "=&S" ( d1 ), "=&D" ( d2 )
                    : "0" ( n ),
                    "1" ( n - 1 + ( const char * ) src ),
                    "2" ( n - 1 + ( char * ) dest )
                    : "memory" );

    return dest;
}
*/
/**
 * Returns the amount of ticks in one second
 */
RTIME ticksPerSec( void )
{
    return nano2count( 1000 * 1000 * 1000 );
}

/**
 * Returns the realtime system time
 */
RTIME systemTimeGet( void )
{
    return rt_get_time();
}

// searching for better implementation
// hrt is in nanoseconds
#define hrt2ts(hrt) ((const TIME_SPEC *) ({ TIME_SPEC timevl; \
            __count2timespec(nano2count(hrt),&timevl);\
            &timevl; }))

NANO_TIME rtos_get_time_ns( void )
{
    return rt_get_time();
}

int rtos_nanosleep( const TIME_SPEC *rqtp, TIME_SPEC *rmtp )
{
    // RTAI has no posix implementation of nanosleep
    rt_sleep( rqtp->tv_nsec + 1000*1000*1000*rqtp->tv_sec );
    //    nanosleep( rqtp, rmtp ); // rtai 24.1.9
    return 0;
}

int sem_post( sem_t *s )
{
    return rt_sem_signal( s );
}

int sem_wait( sem_t *s )
{
    return rt_sem_wait( s );
}

void sem_init( sem_t *s, int dummy, int value )
{
    rt_sem_init( s, value );
}

int sem_destroy( sem_t *s )
{
    return rt_sem_delete( s );
}

// printf
//#define rtos_printf(x) rt_printk(x)
int rtos_printf( const char *fmt, ... )
{
    va_list list;
    char printkbuf [ 2000 ];
    printkbuf[ 0 ] = '\0';
    va_start ( list, fmt );
    vsnprintf( printkbuf, 1999, fmt, list );
    va_end ( list );
    return rt_printk( printkbuf );
}

// will become redundant ?
void rtos_enable_fpu( pthread_attr_t *pa )
{
    return ;
}

// RT FIFO
// niet meer nodig, nu gedefined in configure.ac
//#define HAVE_FIFO_RT

int rtosf_create( int fnr, size_t bytes )
{
    return rtf_create( fnr, bytes );
};

int rtosf_destroy( int fnr )
{
    return rtf_destroy( fnr );
}

int rtosf_put( int fnr, const char * text, size_t bytes )
{
    // TODO: file bug report with rtai
    return rtf_put( fnr, ( void* ) text, bytes );
}

int rtosf_get( int fnr, char * text, size_t bytes )
{
    return rtf_get( fnr, text, bytes );
}

int rtosf_resize( int fnr, size_t bytes )
{
    return rtf_resize( fnr, bytes );
}

void rtosf_remove_all_handlers( int fnr )
{
    // XXX retrieve bufsize of fnr
    rtf_destroy( fnr );
    rtf_create( fnr, 4096 ); // FIXME
}

int rtosf_set_handler( int fnr, int ( *handler ) ( unsigned int, int ) )
{
    return rtf_create_handler( fnr, X_FIFO_HANDLER( handler ) );
}

#if 0
typedef int DItype __attribute__ ( ( mode ( DI ) ) );
typedef int SItype __attribute__ ( ( mode ( SI ) ) );

// XXX THIS IS ONLY FOR LITTLE ENDIAN

struct DIstruct
{
    SItype low, high;
};

// XXX THIS IS ONLY FOR LITTLE ENDIAN

struct DIstruct
{
    SItype low, high;
};

typedef union
{

    struct DIstruct s;
    DItype ll;
} DIunion;


DItype
__muldi3 ( DItype u, DItype v )
{
    DIunion w;
    DIunion uu, vv;

    uu.ll = u,
            vv.ll = v;

    w.ll = __umulsidi3 ( uu.s.low, vv.s.low );
    w.s.high += ( ( USItype ) uu.s.low * ( USItype ) vv.s.high
                  + ( USItype ) uu.s.high * ( USItype ) vv.s.low );

    return w.ll;
}

DItype
__divdi3 ( DItype u, DItype v )
{
    word_type c = 0;
    DIunion uu, vv;
    DItype w;

    uu.ll = u;
    vv.ll = v;

    if ( uu.s.high < 0 )
        c = ~c,
            uu.ll = __negdi2 ( uu.ll );

    if ( vv.s.high < 0 )
        c = ~c,
            vv.ll = __negdi2 ( vv.ll );

    w = __udivmoddi4 ( uu.ll, vv.ll, ( UDItype * ) 0 );

    if ( c )
        w = __negdi2 ( w );

    return w;
}

#endif
