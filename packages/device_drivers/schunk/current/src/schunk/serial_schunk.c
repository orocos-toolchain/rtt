#include <os/fosi.h>

#include "serial_schunk.h"

extern struct rt_com_struct rt_com_table[ 2 ];
#define rt_com_str (&(rt_com_table[ COMPORT ]))

data_t *read, *write, *temp;

int isDataChanged( void );
unsigned int getValue( int i );

unsigned int isrhandler( unsigned int num, struct pt_regs* r );


unsigned int getLastXForce()
{
    return getValue( 0 );
}

unsigned int getLastYForce()
{
    return getValue( 1 );
}

unsigned int getLastZForce()
{
    return getValue( 2 );
}

unsigned int getLastXTorque()
{
    return getValue( 3 );
}

unsigned int getLastYTorque()
{
    return getValue( 4 );
}

unsigned int getLastZTorque()
{
    return getValue( 5 );
}

unsigned int getValue( int i )
{
    if ( isDataChanged() )
    {
        return write->value[ i ];
    }

    return read->value[ i ];
}

int isDataChanged()
{
    if ( write->data_changed )
    {
        read->data_changed = 0;

        temp = write;
        write = read;
        read = temp;

        return 1;
    }

    return 0;
}


unsigned int isrhandler( unsigned int num, struct pt_regs* r )
{
    //(*(rt_com_str->isr))(num, r);
    ( *( rt_com_str->isr ) ) ();

    //data uitlezen


    write->data_changed = 1;
    return 0;
}


int init_module( void )
{
    int res;

    //set up serial port
    rt_com_setup( COMPORT, 9600, 0, 0, 1, 8, 0 );

    //divert irq to own handler
    res = rt_free_linux_irq( rt_com_str->irq, NULL );

    if ( res < 0 )
    {
        rtos_printf( "Error : rt_free_linux_irq returned %d\n", res );
        return res;
    }

    res = rt_request_linux_irq( rt_com_str->irq, &isrhandler, SERIAL_NAME, NULL );

    if ( res < 0 )
    {
        rtos_printf( "Error : rt_request_linux_irq returned %d\n", res );
        return res;
    }

    //hard_enable_irq ????

    return 0;
}
