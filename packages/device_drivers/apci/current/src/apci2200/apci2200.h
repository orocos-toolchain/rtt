/**
*
* Copyright (c) 2001 Jef de Geeter
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
 * Revision:  $Revision: 1.4 $
 * Last edited: $Date: 2002/10/01 17:01:39 $    
 * Log:  $Log: apci2200.h,v $
 * Log:  Revision 1.4  2002/10/01 17:01:39  pissaris
 * Log:  Major reindent and reformat of sourcecode.
 * Log:
 * Log:  Revision 1.3  2002/09/19 12:45:15  psoetens
 * Log:  drivers headers are now independent of linux-implementation
 * Log:  headers (which are no longer installed)
 * Log:  
 * Log:  Revision 1.2  2002/07/19 15:47:56  psoetens
 * Log:  merge Oelewapperke with HEAD
 * Log:  
 * Log:  Revision 1.1.1.1.6.1  2002/07/17 16:30:28  kmuylken
 * Log:  added some comments
 * Log:  added getnumberofdigitaloutputs -> not implemented yet
 * Log:  
 * Log:  Revision 1.1.1.1  2002/06/03 14:05:01  psoetens
 * Log:  Initial import
 * Log:  
 * Log:  Revision 1.1  2002/06/03 13:24:17  psoetens
 * Log:  Added the old drivers in a new directory structure
 * Log:  
 * Log:  Revision 1.10  2001/11/15 15:28:42  psoetens
 * Log:  Added copyright information
 * Log:  
 * Log:  Revision 1.9  2001/08/07 09:56:53  jef
 * Log:  documentation commect added
 * Log:  
 * Log:  Revision 1.8  2001/08/07 09:55:27  jef
 * Log:  added cvs keyword
 * Log:   
 */

// The documentation can be extracted with Doc++.

#ifndef APCI2200
#define APCI2200

#include <linux/pci.h>

/** @name APCI-2200 module
 * @memo This module contains the driver for digital IO relay card: apci 2200 card.
 * */ 
//@{

/** the PCI ID of the vendor of the card */
#define VENDOR_ID 0x15b8 
/** the PCI ID of the device of the card, identifies the card */
#define DEVICE_ID 0x1005

/**
 * ioaddr[1] is the base address 
 */ 
/** offset for the watchdog register */
#define      APCI2200_WATCHDOG_REGISTER       0x08 
/** offset for the digital output register */
#define      APCI2200_DIGITAL_OUTPUT_REGISTER 0x04 
/** offset for the digital input register */
#define      APCI2200_DIGITAL_INPUT_REGISTER  0x00

struct apci2200_device_t
{

    struct pci_dev* pci;
    unsigned int ioaddr[ 2 ];
};


typedef struct apci2200_device_t apci2200_device;

/**
 * this function is executed during the insmod of the module and
 * is responsible for the initialization of the APCI2200 board.
 * check if a PCI bus is available, IO regions are checked and requested
 *
 * @return 0 if no problems: card is found and initialized
 * @return -1 if no PCI bus found
 * @return -2 if no APCI-2200 card found
 * 
 * @author Jef De Geeter
 */
int init_module( void );

/**
 * this function is executed during the rmmod of the module. IO regions
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
apci2200_device* apci2200_get_device( void );

/**
 * gets the status of all 8 digital input signals
 *
 * @return bits 0 -> 7 contain the status
 *
 * @author Jef De Geeter
 */
unsigned int apci2200_get_input_status( apci2200_device* dev );

/**
 * gets the status of all 16 digital output signals
 *
 * @return bits 0 -> 15 contain the status
 *
 * @author Jef De Geeter
 */
unsigned int apci2200_get_output_status( apci2200_device* dev );

/**
 * sets some of the 16 digital output signals "on"
 *
 * @param on specifies which signals to set "on", e.g. 0x0022 (0010 0010) sets the second and sixth signal "on" 
 * 
 * @author Jef De Geeter
 */
void apci2200_set_output_on( apci2200_device* dev, unsigned int on );

/**
 * sets some of the 16 digital output signals "off"
 *
 * @param off specifies which signals to set "off", e.g. 0x0001 (0000 0001) sets the first signal "off" 
 *
 * @author Jef De Geeter
 */
void apci2200_set_output_off( apci2200_device* dev, unsigned int off );

/**
 * initialize the watchdog
 *
 * @param access_mode ???
 * @param time_unit: 0 = nanosec; 1 = microsec; 2 = millisec; 3 = sec
 * @param reload_value ???
 * 
 * @author Jef De Geeter
 */
void apci2200_watchdog_init( apci2200_device* dev, short access_mode, short time_unit, int reload_value );

/**
 * start the watchdog
 *
 * @author Jef De Geeter
 */
void apci2200_watchdog_start( void );

/**
 * stop the watchdog
 *
 * @author Jef De Geeter
 */
void apci2200_watchdog_stop( void );

/**
 * gets the status of the watchdog
 *
 * @author Jef De Geeter
 */
void apci2200_watchdog_get_status( void );

/**
 * trigger the watchdog
 *
 * @author Jef De Geeter
 */
void apci2200_watchdog_trigger( void );

unsigned int apci2200_get_number_off_digital_outputs( void );

//@}
#endif

