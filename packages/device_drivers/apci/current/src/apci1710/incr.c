/**
* Copyright (c) 2001 Jef de Geeter
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

#include "incr.h"
#include "incr_defs.h"
#include "apci1710.h"
#include <asm/io.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/kernel.h> 
//#include <rtl.h>

// !!!!!!!!!!!!!!!!!
// remark : a large part of the documentation is in the header-file
// !!!!!!!!!!!!!!!!!

int init_module( void )
{

    return 0;
}

void cleanup_module( void )
{
}

// **********************
// COUNTER INITIALISATION
// **********************
apci1710_module* apci1710_incr_create_module()
{
    apci1710_module* module;
    module = kmalloc(sizeof(apci1710_module), GFP_KERNEL);
    module->cleanup = 0xc4ea1e;
    return module;
}

int apci1710_incr_init( apci1710_device* dev, unsigned int moduleNb, int counterRange, int firstCounterModus, int firstCounterOption, int secondCounterModus, int secondCounterOption, apci1710_module* module )
{
    // call this function in the following way :
    //
    // apci1710_module module;
    // int returnvalue;
    //
    // returnvalue apci1710_incr_init(. . . . , &module);
    //
    // Attention : we are now presuming that we did not
    // run out of memory!

    // check if dev is an apci1710

    if ( ( dev == NULL ) || ( dev->pci->device != APCI1710_DEVICE_ID ) )
        return -1;

    // moduleNb should be 0 1 2 or 3
    if ( moduleNb > 3 )
        return -2;

    // check if the module is an incremental counter
    if ( ( dev->moduleConf[ moduleNb ] & APCI_INCREMENTAL_COUNTER ) != APCI_INCREMENTAL_COUNTER )
        return -3;

    /* Test the counter range */
    if ( counterRange != APCI1710_16BIT_COUNTER && counterRange != APCI1710_32BIT_COUNTER )
        return -4;

    /* Test the first counter modus */
    if ( firstCounterModus != APCI1710_QUADRUPLE_MODE &&
            firstCounterModus != APCI1710_DOUBLE_MODE &&
            firstCounterModus != APCI1710_SIMPLE_MODE &&
            firstCounterModus != APCI1710_DIRECT_MODE )
        return -5;

    /* Test the first counter option */
    if ( ( firstCounterModus == APCI1710_DIRECT_MODE &&
            ( firstCounterOption != APCI1710_INCREMENT &&
              firstCounterOption != APCI1710_DECREMENT ) ) ||
            ( firstCounterModus != APCI1710_DIRECT_MODE &&
              ( firstCounterOption != APCI1710_HYSTERESIS_ON &&
                firstCounterOption != APCI1710_HYSTERESIS_OFF ) ) )
        return -6;

    // Test second counter modus and option if we have a 16-bit counter

    // IMPORTANT!
    // If you have configured the module for two 16-bit counter, a mixed
    // mode with a counter in quadruple/double/single mode
    // and the other counter in direct mode is not possible!
    if ( counterRange == APCI1710_16BIT_COUNTER )
    {
        if ( ( firstCounterModus != APCI1710_DIRECT_MODE &&
                ( secondCounterModus != APCI1710_QUADRUPLE_MODE &&
                  secondCounterModus != APCI1710_DOUBLE_MODE &&
                  secondCounterModus != APCI1710_SIMPLE_MODE ) ) ||
                ( firstCounterModus == APCI1710_DIRECT_MODE &&
                  secondCounterModus != APCI1710_DIRECT_MODE ) )
            return -7;

        if ( ( secondCounterModus == APCI1710_DIRECT_MODE &&
                ( secondCounterOption != APCI1710_INCREMENT &&
                  secondCounterOption != APCI1710_DECREMENT ) ) ||
                ( secondCounterModus != APCI1710_DIRECT_MODE &&
                  ( secondCounterOption != APCI1710_HYSTERESIS_ON &&
                    secondCounterOption != APCI1710_HYSTERESIS_OFF ) ) )
            return -8;
    }

    // initialize "module" (except for the modregvalues)
    //
    // This module-struct is made to simplify checks
    // in the other functions of incr.o.  It is then
    // possible to check for the right protocol of the
    // module with :
    // if(module->protocol != APCI_INCREMENTAL_COUNTER) return -1;
    // And at once, you are sure that dev and moduleNb
    // are ok.  Another advantage is that we are sure that
    // the module is initialized this way.
    //
    // It also holds a few variables that are used by index-functions

    module->dev = dev;

    module->moduleNb = moduleNb;

    module->protocol = APCI_INCREMENTAL_COUNTER;

    module->counterRange = counterRange;

    module->indexinit = 0;

    // Now we read the configuration and set up the
    // value that shall be written to mode register 1
    // (base+20)

    if ( counterRange == APCI1710_32BIT_COUNTER )
    {
        module->modreg1value = counterRange |  // 32bit
                               firstCounterModus |  // direct               or quadruple/double/simple
                               firstCounterOption; // increment/decrement  or hysteresis
    }

    else // 2 * 16bit mode
    {
        module->modreg1value = counterRange |
                               ( firstCounterModus & 0x5 ) |  // Sets first counter on its modus without touching the second.  Direct mode gets filtered out here, so we correct that later.
                               ( firstCounterOption & 0x20 ) |  // analog ...
                               ( secondCounterModus & 0xa ) |  // analog, directmode does not go in
                               ( secondCounterOption & 0x40 );

        if ( firstCounterModus == APCI1710_DIRECT_MODE )  // correcting for directmode
        {
            module->modreg1value |= APCI1710_DIRECT_MODE;
        }
    }

    // Now write the configuration to modreg1
    outl( module->modreg1value, dev->ioaddr[ 2 ] + 20 + 64 * moduleNb );

    return 0;
}

int apci1710_incr_cleanup_module(apci1710_module* module)
{
    if (module->cleanup == 0xc4ea1e)
        kfree(module);
    return 0;
}

int apci1710_incr_clear_counter_value( apci1710_module* module )
{
    // check if the module is a valid INCR counter

    if ( module->protocol != APCI_INCREMENTAL_COUNTER )
        return -1;

    // base +16
    outb( 1, module->dev->ioaddr[ 2 ] + 16 + 64 * module->moduleNb );

    return 0;
}

// *******************
// READING THE COUNTER
// *******************

int apci1710_incr_latch_counter( apci1710_module* module, unsigned int latchRegister )
{
    // check if the module is a valid INCR counter

    if ( module->protocol != APCI_INCREMENTAL_COUNTER )
        return -1;

    // strobe register (base+0)
    // You need to generate a software strobe to
    // make the counter card write the latest counter value to
    // the latch registers. (p.17)
    // Then, you can read the latch registers with
    // "read_latch_register_value".

    if ( latchRegister > 1 )
        return -2;

    outb( 0x1 << 4 * latchRegister, module->dev->ioaddr[ 2 ] + 64 * module->moduleNb );

    return 0;
}

int apci1710_incr_read_latch_register_status( apci1710_module* module, unsigned int latchRegister, int* latchStatus )
{
    // check if the module is a valid INCR counter

    if ( module->protocol != APCI_INCREMENTAL_COUNTER )
        return -1;

    // check if latchRegister is valid
    if ( latchRegister > 1 )
        return -2;

    // base + 0 (interrupt register)
    // mask with 0x3 to get last 2 bits
    *latchStatus = ( inb( module->dev->ioaddr[ 2 ] + 64 * module->moduleNb ) >> 4 * latchRegister ) & 0x03;

    return 0;
}

int apci1710_incr_read_latch_register_value( apci1710_module* module, unsigned int latchRegister, unsigned int* result )
{
    // TODO : the type of result needs to be 4 bytes for
    // 32bit and 2 bytes for 16bit (unsigned)

    // check if the module is a valid INCR counter

    if ( module->protocol != APCI_INCREMENTAL_COUNTER )
        return -1;

    // check if latchRegister is valid
    if ( latchRegister > 1 )
        return -2;

    // read first lactch register on base + 4, second on base + 8
    // Now we are actually reading the latch-registers
    *result = inl( module->dev->ioaddr[ 2 ] + 4 * ( latchRegister + 1 ) + 64 * module->moduleNb );

    return 0;
}

int apci1710_incr_enable_latch_interrupt( apci1710_module* module )
{
    u32 help1;
    u32 help2;
    u32 help3;

    // Test if the module is a valid incremental counter

    if ( module->protocol != APCI_INCREMENTAL_COUNTER )
        return -1;

    // Test if interruptroutine initialized
    if ( apci1710_get_irqhandler_status() != 1 )
        return -2;

    // adjust modreg2value
    module->modreg2value = ( module->modreg2value | APCI1710_ENABLE_LATCH_INT );

    // write configuration to mode registers
    help1 = ( u32 ) module->modreg1value;

    help2 = ( u32 ) module->modreg2value;

    help3 = ( u32 ) module->modreg3value;

    outw( ( help3 << 16 ) | ( help2 << 8 ) | help1, module->dev->ioaddr[ 2 ] + 20 + 64 * module->moduleNb );

    // ATTENTION, this does not work!!
    // Maybe my interrupthandler (apci1710irahandler)
    // is not ok.  See that file to know what
    // exactly is going wrong.
    return 0;
}

int apci1710_incr_disable_latch_interrupt( apci1710_module* module )
{
    u32 help1;
    u32 help2;
    u32 help3;

    // Test if the module is a valid incremental counter

    if ( module->protocol != APCI_INCREMENTAL_COUNTER )
        return -1;

    // Test if interruptroutine initialized
    if ( apci1710_get_irqhandler_status() != 1 )
        return -2;

    // adjust modreg2value
    module->modreg2value = ( module->modreg2value & APCI1710_DISABLE_LATCH_INT );

    // write configuration to mode registers
    help1 = ( u32 ) module->modreg1value;

    help2 = ( u32 ) module->modreg2value;

    help3 = ( u32 ) module->modreg3value;

    outw( ( help3 << 16 ) | ( help2 << 8 ) | help1, module->dev->ioaddr[ 2 ] + 20 + 64 * module->moduleNb );

    // ATTENTION, this does not work!!
    // Maybe my interrupthandler (apci1710irahandler)
    // is not ok.  See that file to know what
    // exactly is going wrong.
    return 0;
}

int apci1710_incr_read_16bit_counter_value( apci1710_module* module, unsigned int latchRegister, u32* result )
{
    // check if the module is a valid INCR counter

    if ( module->protocol != APCI_INCREMENTAL_COUNTER )
        return -1;

    // check if selectedCounter is valid
    if ( latchRegister > 1 )
        return -2;

    // Check if module is 16bit.
    if ( module->counterRange != APCI1710_16BIT_COUNTER )
        return -3;

    // Latch the counter
    outb( 0x1 << 4 * latchRegister, module->dev->ioaddr[ 2 ] + 64 * module->moduleNb );

    // read first lactch register on base + 4, second on base + 8
    // Now we are actually reading the latch-registers
    *result = inl( module->dev->ioaddr[ 2 ] + 4 * ( latchRegister + 1 ) + 64 * module->moduleNb );

    return 0;
}

int apci1710_incr_read_32bit_counter_value( apci1710_module* module, u32* result )
{
    // check if the module is a valid INCR counter

    if ( module->protocol != APCI_INCREMENTAL_COUNTER )
        return -1;

    // Check if module is 32bit.
    if ( module->counterRange != APCI1710_32BIT_COUNTER )
        return -2;

    // Latch the counter
    outb( 0x1 , module->dev->ioaddr[ 2 ] + 64 * module->moduleNb );

    // read first lactch register on base + 4, second on base + 8
    // Now we are actually reading the latch-registers
    *result = inl( module->dev->ioaddr[ 2 ] + 4 + 64 * module->moduleNb );

    return 0;
}

// **********************
// WRITING IN THE COUNTER
// **********************

int apci1710_incr_write_16bit_counter_value( apci1710_module* module, unsigned int selectedCounter, u32 writeValue )
{
    // check if the module is a valid INCR counter

    if ( module->protocol != APCI_INCREMENTAL_COUNTER )
        return -1;

    // check if selectedCounter is valid
    if ( selectedCounter > 1 )
        return -2;

    // Check if module is 16bit.
    if ( module->counterRange != APCI1710_16BIT_COUNTER )
        return -3;

    // first counter: base + 8; second counter: base + 14
    outw( writeValue, module->dev->ioaddr[ 2 ] + 8 + 6 * selectedCounter + 64 * module->moduleNb );

    return 0;
}

int apci1710_incr_write_32bit_counter_value( apci1710_module* module, u32 writeValue )
{
    // check if the module is a valid INCR counter

    if ( module->protocol != APCI_INCREMENTAL_COUNTER )
        return -1;

    // Check if module is 32bit.
    if ( module->counterRange != APCI1710_32BIT_COUNTER )
        return -2;

    // base + 4
    outl( writeValue, module->dev->ioaddr[ 2 ] + 4 + 64 * module->moduleNb );

    return 0;
}

// ***************
// INDEX FUNCTIONS
// ***************

int apci1710_incr_init_index( apci1710_module* module, int referenceAction, int indexOperation, int autoMode, int interruptMode )
{
    // check if the module is a valid INCR counter

    if ( module->protocol != APCI_INCREMENTAL_COUNTER )
        return -1;

    // only a 32bit counter can have an index-signal (XXX I'm not 100% sure about that XXX)
    if ( module->counterRange != APCI1710_32BIT_COUNTER )
        return -2;

    // check if referenceAction is valid
    if ( referenceAction != APCI1710_ENABLE_INDEX_ACTION && referenceAction != APCI1710_DISABLE_INDEX_ACTION )
        return -3;

    // check if indexOperation is valid
    if ( indexOperation != APCI1710_INDEX_LATCH_COUNTER && indexOperation != APCI1710_INDEX_CLEAR_COUNTER )
        return -4;

    // check if autoMode is valid
    if ( autoMode != APCI1710_ENABLE_INDEX_AUTO_MODE && autoMode != APCI1710_DISABLE_INDEX_AUTO_MODE )
        return -5;

    // check if interruptMode is valid
    if ( interruptMode != APCI1710_ENABLE_INDEX_INT && interruptMode != APCI1710_DISABLE_INDEX_INT )
        return -6;

    // check if interrupt function is initialized when APCI1710_ENABLE_INDEX_INT is chosen
    // ATTENTION : interrupts are not working properly
    if ( ( interruptMode == APCI1710_ENABLE_INDEX_INT ) && ( apci1710_get_irqhandler_status() != 1 ) )
        return -7;

    // set the configuration
    if ( referenceAction == APCI1710_ENABLE_INDEX_ACTION )
    {
        module->modreg2value = APCI1710_ENABLE_INDEX_ACTION;
    }

    else
    {
        module->modreg2value = ( 0x0 & APCI1710_DISABLE_INDEX_ACTION );
    }

    if ( indexOperation == APCI1710_INDEX_LATCH_COUNTER )
    {
        module->modreg2value = ( module->modreg2value | APCI1710_INDEX_LATCH_COUNTER );
    }

    else
    {
        module->modreg2value = ( module->modreg2value & APCI1710_INDEX_CLEAR_COUNTER );
    }

    // XXX Things seem to be more complex than this.  See manual of the card.
    // XXX It concerns high-edge/low-edge settings for indexOperation.

    if ( autoMode == APCI1710_ENABLE_INDEX_AUTO_MODE )
    {
        module->modreg2value = ( module->modreg2value | APCI1710_ENABLE_INDEX_AUTO_MODE );
    }

    else
    {
        module->modreg2value = ( module->modreg2value & APCI1710_DISABLE_INDEX_AUTO_MODE );
    }

    // This mode-register 3 thing is strange.
    // But it is the way they use it in the doscode.

    if ( interruptMode == APCI1710_ENABLE_INDEX_INT )
    {
        module->modreg3value = APCI1710_ENABLE_INDEX_INT;
    }

    else
    {
        module->modreg3value = ( 0x0 & APCI1710_DISABLE_INDEX_INT );
    }

    module->indexinit = 1;

    // I do not actually write the config until you call
    // apci1710_incr_enable_index

    return 0;
}

int apci1710_incr_enable_index( apci1710_module* module )
{
    u32 help1;
    u32 help2;
    u32 help3;

    // check if the module is a valid INCR counter

    if ( module->protocol != APCI_INCREMENTAL_COUNTER )
        return -1;

    // check if index has been initialized
    if ( module->indexinit != 1 )
        return -2;

    // adjust modreg2value
    module->modreg2value = ( module->modreg2value | APCI1710_ENABLE_INDEX );

    // The following read-operation is present
    // in the doscode.  I do not find documentation
    // about it, so I don't know the reason.
    // For me, it does work without it.
    // inl(module->dev->ioaddr[2] + 24 + 64* module->moduleNb);
    //
    // Remark : maybe that this affects the behaviour of
    // interrupts.  I don't know.

    // write configuration to mode registers
    help1 = ( u32 ) module->modreg1value;

    help2 = ( u32 ) module->modreg2value;

    help3 = ( u32 ) module->modreg3value;

    outl( ( help3 << 16 ) | ( help2 << 8 ) | help1, module->dev->ioaddr[ 2 ] + 20 + 64 * module->moduleNb );

    //rtl_printf("modregvalue 2 en 3 : %u en %u\n", help2, help3);

    return 0;
}

int apci1710_incr_disable_index( apci1710_module* module )
{
    u32 help1;
    u32 help2;
    u32 help3;

    // check if the module is a valid INCR counter

    if ( module->protocol != APCI_INCREMENTAL_COUNTER )
        return -1;

    // check if index has been initialized
    if ( module->indexinit != 1 )
        return -2;

    // adjust modreg2value
    module->modreg2value = ( module->modreg2value & APCI1710_DISABLE_INDEX );

    // write configuration to mode registers
    help1 = ( u32 ) module->modreg1value;

    help2 = ( u32 ) module->modreg2value;

    help3 = ( u32 ) module->modreg3value;

    outw( ( help3 << 16 ) | ( help2 << 8 ) | help1, module->dev->ioaddr[ 2 ] + 20 + 64 * module->moduleNb );

    return 0;
}

int apci1710_incr_get_index_status( apci1710_module* module, int* result )
{
    // check if the module is a valid INCR counter

    if ( module->protocol != APCI_INCREMENTAL_COUNTER )
        return -1;

    // check if index has been initialized
    if ( module->indexinit != 1 )
        return -2;

    *result = inb( module->dev->ioaddr[ 2 ] + 12 + 64 * module->moduleNb );

    return 0;
}

int apci1710_incr_set_input_filter( apci1710_module* module, int PCIclock, int filter )
{ return -1;}
int apci1710_incr_counter_auto_test( apci1710_module* module )
{ return -1;}

int apci1710_incr_set_digital_channel_on( apci1710_module* module )
{ return -1;}
int apci1710_incr_set_digital_channel_off( apci1710_module* module )
{ return -1;}
