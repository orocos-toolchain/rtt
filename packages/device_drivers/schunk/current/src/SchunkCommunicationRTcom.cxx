#include "corelib/SchunkCommunicationRTcom.hpp"
#include <os/cpp.hpp>
#include <os/fosi.h> 
//#include <rtai.h>
//#include <rtai_sched.h>

namespace ORO_DeviceDriver

            public:
            SchunkCommunicationRTcom::SchunkCommunicationRTcom( Event* event )
{
    int res;

    pthread_cond_init( &ring_buffer.cond, NULL );
    pthread_mutex_init( &ring_buffer.mutex, NULL );

    // set up serial port
    res = rt_com_set_param( comport, 0, 0 );

    if ( res )
    {
        rtos_printf( "Error : rt_com_set_param returned %d\n", res );
        return res;
    }

    res = rt_com_setup( comport, 9600, 0, 1, 8 );

    if ( res )
    {
        rtos_printf( "Error :  rt_com_setup returned %d\n", res );
        return res;
    }

    res = rt_com_set_mode( comport, RT_COM_NO_HAND_SHAKE );

    if ( res )
    {
        rtos_printf( "Error : rt_com_set_mode returned %d\n", res );
        return res;
    }

    data_available = event;
    data_available->addHandler( this, this );

    // set callback method; this method is called whenever data is
    // recieved from the given comport
    rt_com_set_callback( comport, &handler );

    rtos_printf( "\nSchunkCommunicationRTcom opened\n" );
}

SchunkCommunicationRTcom::~SchunkCommunicationRTcom()
{
    rt_com_set_callback( comport, 0 );
    data_available->removeHandler( this );

    rt_com_setup( comport, -1, 0, 0, 0 );
    rt_com_set_param( comport, -1, 0 );

    pthread_mutex_destroy( &ring_buffer.mutex );
    pthread_cond_destroy( &ring_buffer.cond );

    rtos_printf( "\nSchunkCommunicationRTcom closed\n" );
}

virtual int SchunkCommunicationRTcom::send( char* buf, int count )
{
    int s = rt_com_write( comport, buf, count );
    return s;
}

virtual int SchunkCommunicationRTcom::flushInput()
{
    ring_buffer.pRead = ring_buffer.pWrite;
}

virtual int SchunkCommunicationRTcom::read( char* buf, int count )
{
    int s;
    pthread_mutex_lock( &ring_buffer.mutex );

    while ( count > ring_buffer_getMaxReadLength() )
    {
        pthread_cond_wait( &ring_buffer.cond, &ring_buffer.mutex );
    }

    s = ring_buffer_read( buf, count );
    pthread_mutex_unlock();
    return s;
}

virtual void handleEvent( EventInterface *ei )
{
    ei->complete( this );
}

virtual void completeEvent( EventInterface *eli )
{
    int s;
    char buf[ 32 ];

    s = rt_com_read( comport, buf, 32 );
    ring_buffer_write( buf, s );
}

private:

void handler( unsigned int i )
{
    data_available->fire();
}

void SchunkCommunicationRTcom::mask( int* a )
{
    *a &= maskNb;
}

int SchunkCommunicationRTcom::ring_buffer_write( char* buf, int count )
{
    int i, j, maxLength;
    pthread_mutex_lock( &ring_buffer.mutex );
    j = count;
    maxLength = ring_buffer_getMaxWriteLength()

                if ( count > maxLength )
                {
                    j = maxLength;
                }

                for ( i = 0; i < j; i++ )
                {
                    ring_buffer.buf[ ring_buffer.pWrite++ ] = buf[ i ];
                    mask( &ring_buffer.pWrite );
                }

                pthread_mutex_unlock( &ring_buffer.mutex );
    pthread_cond_signal( &ring_buffer.cond );
    return i;
}

int SchunkCommunicationRTcom::ring_buffer_read( char* buf, int count )
{
    int i, j, maxLength;
    j = count;
    maxLength = ring_buffer_getMaxReadLength();

    if ( count > maxLength )
    {
        j = maxLength;
    }

    for ( i = 0; i < j; i++ )
    {
        buf[ i ] = ring_buffer.buf[ ring_buffer.pRead++ ];
        mask( &ring_buffer.pRead );
    }

    return i;
}

int SchunkCommunicationRTcom::ring_buffer_getMaxReadLength()
{
    int i = ring_buffer.pWrite - ring_buffer.pRead;
    mask( i );
    return i;
}

int SchunkCommunicationRTcom::ring_buffer_getMaxWriteLength()
{
    return ( maskNb - ring_buffer_getReadLength() );
}
}
