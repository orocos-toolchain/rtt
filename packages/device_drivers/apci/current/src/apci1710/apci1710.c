/**
*
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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <asm/io.h>
#include <linux/ioport.h>

#include "apci1710.h"

#ifndef CONFIG_PCI
#   error "This driver needs PCI support to be available"
#endif


// !!!!!!!!!!!!!!!!
// IMPORTANT : see header-file for more documentation
// !!!!!!!!!!!!!!!!

static int interruptinit;
static apci1710_device apci1710;

int init_module( void )
{

    int i;

    // checks if there's a PCI bus


    //see if we can find the device
    apci1710.pci = pci_find_device( APCI1710_VENDOR_ID, APCI1710_DEVICE_ID, apci1710.pci );

    if ( apci1710.pci == NULL )
    {
        printk( "No APCI-1710 card found!\n" );
        return -2;
    }

    for ( i = 0; i < 3; i++ )
    {
        apci1710.ioaddr[ i ] = pci_resource_start( apci1710.pci, i ) & PCI_BASE_ADDRESS_IO_MASK;
    }

    printk( "Device APCI-1710 found at IO 0x%04x and IRQ %d\n", apci1710.ioaddr[ 2 ], apci1710.pci->irq );

    for ( i = 0; i < 4; i++ )
    {
        apci1710.moduleConf[ i ] = inl( apci1710.ioaddr[ 2 ] + 60 + 64 * i );
        printk( "Module %d is configured with %x function.\n", i, apci1710.moduleConf[ i ] );
    }

    // checks and request the IO regions
    if ( check_region( apci1710.ioaddr[ 0 ], 64 ) || check_region( apci1710.ioaddr[ 1 ], 8 ) || check_region( apci1710.ioaddr[ 2 ], 256 ) )
        return -3;

    request_region( apci1710.ioaddr[ 0 ], 64, "APCI-1710" );

    request_region( apci1710.ioaddr[ 1 ], 8, "APCI-1710" );

    request_region( apci1710.ioaddr[ 2 ], 256, "APCI-1710" );

    // Sets initerruptinit to 0
    interruptinit = 0;

    return 0;
}

void cleanup_module( void )
{

    // release IO region
    release_region( apci1710.ioaddr[ 0 ], 64 );
    release_region( apci1710.ioaddr[ 1 ], 8 );
    release_region( apci1710.ioaddr[ 2 ], 256 );
}

apci1710_device* apci1710_get_device( void )
{

    return & apci1710;
}

int apci1710_set_irqhandler_init( value )
{
    // Check for valid input.

    if ( ( value != 1 ) && ( value != 0 ) )
        return -1;

    // assign
    interruptinit = value;

    // rtl_printf("De waarde van interruptinit is : %u\n",interruptinit);
    return interruptinit;
}

int apci1710_get_irqhandler_status( void )
{
    return interruptinit;
}

EXPORT_SYMBOL(apci1710_get_device);
EXPORT_SYMBOL(apci1710_get_irqhandler_status);
