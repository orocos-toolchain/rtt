/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:17 CET 2005  serial_schunk.c 

                        serial_schunk.c -  description
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
