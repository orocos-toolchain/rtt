/**
*
* Copyright (c) 2001 Filip Vancoillie 
*
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

#include <rtl.h>
#include <rtl_sched.h>
#include "apci1710.h"

#define IRQ 18 
// This module must see where the interrupt comes from
// (from which counter). Even more, the
// kind of interrupt (latch, index, compare) determines
// which interrupt-routine is used. (according to the dos code)
//

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Apparently, this DOES NOT work.  Either I
// get no interrupts at all, either I get
// 100000's of them (when the first bit of
// moderegister3 is high) when there should be 0.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// I do not know why this is but it has something
// to do with this strange modregister3.  (see incr.c)

static int test = 0;

unsigned int apci1710irqhandler( unsigned int irq, struct pt_regs *regs )
{
    //rtl_printf("!!!!!!!!!!!!!\n");
    // Apparently, this rtl_printf crashes the whole thing.

    // This irq-handler does only count the number
    // of interrupts that occured.
    test++;
    rtl_hard_enable_irq( irq ); // This might be not necessary
    return 0;
}



int init_module( void )
{
    int help;

    rtl_irqstate_t f;
    rtl_no_interrupts( f );
    // request irq
    help = rtl_request_irq( IRQ, apci1710irqhandler );

    // If succesful, an internal flag of apci1710
    // is set high with apci1710_set_irqhandler_init(1).
    //
    // This internal flag will be checked by
    // functions of incr.c that enable interrupts.
    // Interrupts will only be enabled if there
    // is an interrupt-handler loaded.
    rtl_hard_enable_irq( IRQ );
    rtl_restore_interrupts( f );

    if ( help == 0 )
        apci1710_set_irqhandler_init( 1 );

    rtl_printf( "request returned %d\n", help );

    return help;

    //rtl_hard_enable_irq(IRQ);
}

void cleanup_module( void )
{
    // Number of interrupts that occured
    rtl_printf( "aantal opgetreden interrupts, %u\n", test );
    rtl_free_irq( IRQ );

    // Put internal flag of apci1710 low.
    // See above for explanation.
    apci1710_set_irqhandler_init( 0 );
}

// @author Filip Vancoillie
