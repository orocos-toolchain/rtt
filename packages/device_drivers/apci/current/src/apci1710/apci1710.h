
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
 **/

#ifndef APCI1710_H
#define APCI1710_H

#ifdef __KERNEL__
#include <linux/pci.h>

/** @name APCI-1710 module
 * @memo This module contains general stuff of the apci 1710 card.
 * */ 
//@{

/** the PCI ID of the vendor of the card */
#define APCI1710_VENDOR_ID 0x10e8 
/** the PCI ID of the device of the card, identifies the card */
#define APCI1710_DEVICE_ID 0x818f

#define APCI1710_33MHZ 0x21 /* 33 */
#define APCI1710_30MHZ 0x1e /* 30 */


struct apci1710_device_t
{

    struct pci_dev* pci;
    unsigned int moduleConf[ 4 ];
    unsigned int ioaddr[ 3 ];
};

typedef struct apci1710_device_t apci1710_device;

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

struct apci1710_module_t
{
    apci1710_device* dev;
    unsigned int moduleNb;
    unsigned int protocol;
    // specific for incremental
    unsigned int counterRange; // (16bit or 32bit)
    unsigned short indexinit;
    u8 modreg1value;
    u8 modreg2value;
    u8 modreg3value;
    unsigned int cleanup; // 1 = true; 0 = false
};

typedef struct apci1710_module_t apci1710_module;

/**
 * this function is executed during the insmod of the module and
 * is responsible for the initialization of the APCI1710 board.
 * check if a PCI bus is available, IO regions are checked and requested
 *
 * @return 0 if no problems: card is found and initialized
 * @return -1 if no PCI bus found
 * @return -2 if no APCI-1710 card found
 * 
 * @author Jef De Geeter
 * @author Filip Vancoillie
 */
int init_module( void );

/**
 * this function is exectued during the rmmod of the module. IO regions
 * are released.
 *
 * @author Jef De Geeter
 */
void cleanup_module( void );


/**
 * information about the board
 * 
 * @return structure containig the device parameters, like irq, base io adresses,
 * the configuration of the modules
 *
 * @author Jef De Geeter
 */
apci1710_device* apci1710_get_device( void );

/*
 * This function sets a flag to indicate if there has been
 * an interrupthandler initialized.  This function is used
 * by functions from incr.c that want to enable interrupts.
 *
 * @param value : 0 if not initialized, 1 if initialized
 *
 * @return 0 if no problem
 * @return -1 if value is not 0 or 1
 *
 *
 * @author Filip Vancoillie
 */

int apci1710_set_irqhandler_init( int value );

/*
 * This function gets the flag that indicates if there has been
 * an interrupthandler initialized.  This function is used
 * by functions from incr.c that want to enable interrupts.
 *
 * @param value : 0 if not initialized, 1 if initialized
 *
 * @return interruptinit
 *
 *
 * @author Filip Vancoillie
 */

int apci1710_get_irqhandler_status( void );

//@}
#else

typedef void apci1710_device;
typedef void apci1710_module;

#endif // __KERNEL__

#endif

