/**
* Copyright (c) 2001 Jef de Geeter
*
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

/**
 * author Jef De Geeter (jef@zazof.org)
 * date   July 2001  
 *
 * Revision: $Revision: 1.4 $
 * Last edited: $Date: 2002/10/01 17:01:38 $
 * Log:  $Log: apci2200.c,v $
 * Log:  Revision 1.4  2002/10/01 17:01:38  pissaris
 * Log:  Major reindent and reformat of sourcecode.
 * Log:
 * Log:  Revision 1.3  2002/07/19 15:47:56  psoetens
 * Log:  merge Oelewapperke with HEAD
 * Log:  
 * Log:  Revision 1.2.6.2  2002/07/17 16:30:28  kmuylken
 * Log:  added some comments
 * Log:  added getnumberofdigitaloutputs -> not implemented yet
 * Log:  
 * Log:  Revision 1.2.6.1  2002/07/09 15:54:08  psoetens
 * Log:  iobase fix
 * Log:  
 * Log:  Revision 1.2  2002/06/10 11:52:57  psoetens
 * Log:  fixed comedi warnings
 * Log:  fixed makefiles & subdirs
 * Log:  fixed apci2200 kernel 2.4 calls
 * Log:  
 * Log:  Revision 1.1.1.1  2002/06/03 14:05:01  psoetens
 * Log:  Initial import
 * Log:  
 * Log:  Revision 1.1  2002/06/03 13:24:17  psoetens
 * Log:  Added the old drivers in a new directory structure
 * Log:  
 * Log:  Revision 1.7  2001/11/15 15:28:42  psoetens
 * Log:  Added copyright information
 * Log:  
 * Log:  Revision 1.6  2001/08/07 09:59:19  jef
 * Log:  changed cvs keyword
 * Log:  
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <asm/io.h>
#include <linux/ioport.h>

#ifndef CONFIG_PCI
#   error "This driver needs PCI support to be available"
#endif


#include "apci2200.h"

apci2200_device apci2200;

int init_module( void )
{
    int i;


    //see if we can find the device
    apci2200.pci = pci_find_device( VENDOR_ID, DEVICE_ID, apci2200.pci );

    if ( apci2200.pci == NULL )
    {
        printk( "No APCI-2200 card found!\n" );
        return -2;
    }

    for ( i = 0; i < 2; i++ )
    {
        apci2200.ioaddr[ i ] = pci_resource_start( apci2200.pci, i ) & PCI_BASE_ADDRESS_IO_MASK;
        printk( "Device APCI-2200 found at IO 0x%04x\n", apci2200.ioaddr[ i ] );
    }

    if (!request_region( apci2200.ioaddr[ 0 ], 4, "APCI-2200" ))
	{
		printk(KERN_ERR "apci-2200: I/O port %d is not free.\n", apci2200.ioaddr[ 0 ]);
		return -EIO;
	}

    if (!request_region( apci2200.ioaddr[ 1 ], 64, "APCI-2200" ))
	{
		printk(KERN_ERR "apci-2200: I/O port %d is not free.\n", apci2200.ioaddr[ 1 ]);
		release_region(apci2200.ioaddr[ 0 ], 4);
		return -EIO;
	}

    return 0;
}

void cleanup_module( void )
{

    // release IO region
    release_region( apci2200.ioaddr[ 0 ], 4 );
    release_region( apci2200.ioaddr[ 1 ], 64 );
}

apci2200_device* apci2200_get_device( void )
{
    return & apci2200;
}

unsigned int apci2200_get_input_status( apci2200_device* dev )
{
    return inl( dev->ioaddr[ 1 ] + APCI2200_DIGITAL_INPUT_REGISTER );
}

unsigned int apci2200_get_output_status( apci2200_device* dev )
{
    return inl( dev->ioaddr[ 1 ] + APCI2200_DIGITAL_OUTPUT_REGISTER );
}

void apci2200_set_output_on( apci2200_device* dev, unsigned int on )
{
    unsigned int result, status;
    // status contains the output status, output is bitwise or-ed with status
    status = apci2200_get_output_status( dev );
    result = on | status;
    outl( result, dev->ioaddr[ 1 ] + APCI2200_DIGITAL_OUTPUT_REGISTER );
}

void apci2200_set_output_off( apci2200_device* dev, unsigned int off )
{
    unsigned int result, status;
    // status contains the output status
    // result = status . (neg output) (in C: ^ is used for exor, and ~ for neg)
    status = apci2200_get_output_status( dev );
    result = status & ( ~ off );
    outl( result, dev->ioaddr[ 1 ] + APCI2200_DIGITAL_OUTPUT_REGISTER );
}

unsigned int apci2200_get_number_off_digital_outputs( void )
{
    //not implemented yet
    return 16;
};

void apci2200_watchdog_init( apci2200_device* dev, short access_mode, short time_unit, int reload_value )
{
    unsigned char command1;
    unsigned char command2;
    int command;

    // test the access

    if ( ( access_mode == 8 ) || ( access_mode == 24 ) || ( access_mode == 40 ) || ( access_mode == 56 ) )
    {
        // get the watchdog command
        command1 = inb( dev->ioaddr[ 1 ] + APCI2200_WATCHDOG_REGISTER + 12 );
        command2 = inb( dev->ioaddr[ 1 ] + APCI2200_WATCHDOG_REGISTER + 13 );
        // disable the timer enable bit
        command1 &= 0xec;
        command2 &= 0xf9;
        // set the command
        outb( command1, dev->ioaddr[ 1 ] + APCI2200_WATCHDOG_REGISTER + 12 );
        outb( command2, dev->ioaddr[ 1 ] + APCI2200_WATCHDOG_REGISTER + 13 );
        // set the watchdog reload value
        outl( reload_value, dev->ioaddr[ 1 ] + APCI2200_WATCHDOG_REGISTER + 4 );
        // set the watchdog time unit
        outl( time_unit & 0xff, dev->ioaddr[ 1 ] + APCI2200_WATCHDOG_REGISTER + 8 );
    }

    else
    {
        if ( ( access_mode == 16 ) || ( access_mode == 24 ) || ( access_mode > 40 ) )
        {
            // get the watchdog command
            command = inw( dev->ioaddr[ 1 ] + APCI2200_WATCHDOG_REGISTER + 12 );
            // disable the timer enable bit
            command &= 0xf9ec;
            // set the command
            outw( command, dev->ioaddr[ 1 ] + APCI2200_WATCHDOG_REGISTER + 12 );
            // set the watchdog reload value
            outl( reload_value, dev->ioaddr[ 1 ] + APCI2200_WATCHDOG_REGISTER + 4 );
            // set the watchdog time unit
            outl( time_unit & 0xffff, dev->ioaddr[ 1 ] + APCI2200_WATCHDOG_REGISTER + 8 );
        }

        else
        {
            if ( access_mode >= 32 )
            {
                // get the watchdog command
                command = inw( dev->ioaddr[ 1 ] + APCI2200_WATCHDOG_REGISTER + 12 );
                // disable the timer enable bit
                command &= 0xfffff9ec;
                // set the command
                outl( command, dev->ioaddr[ 1 ] + APCI2200_WATCHDOG_REGISTER + 12 );
                // set the watchdog reload value
                outl( reload_value, dev->ioaddr[ 1 ] + APCI2200_WATCHDOG_REGISTER + 4 );
                // set the watchdog time unit
                outl( time_unit, dev->ioaddr[ 1 ] + APCI2200_WATCHDOG_REGISTER + 8 );
            }
        }
    }
}

EXPORT_SYMBOL(apci2200_get_device);
EXPORT_SYMBOL(apci2200_get_input_status);
EXPORT_SYMBOL(apci2200_set_output_on);
EXPORT_SYMBOL(apci2200_get_output_status);
EXPORT_SYMBOL(apci2200_get_number_off_digital_outputs);
EXPORT_SYMBOL(apci2200_set_output_off);
