/**
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


#ifndef APCI1710_SSIMODULE
#define APCI1710_SSIMODULE

#include "apci1710.h"

/** @name SSI module
 *
 * @memo this module contains all SSI specfic stuff and needs the apci1710 module. 
 * */ 
//@{


/** if the bits must be interpreted in gray code */
#define COUNTINGMODE_GRAY  0 
/** if the bits needs conversion to binary code */
#define COUNTINGMODE_BINARY 1

/** SSI Outputclock determines the output clock and divides the PCI bus frequency
 *  
 *  Input frequency = PCI bus frequency (between 0 and 33 MHz. You will find the frequency in the manual of your PC)
 *  E.g. : If the SSI_OUTPUTCLOCK = 100 the frequency is divided by 200.
 *  The maximum clock frequency is 1 Mhz. Formula for calculation can be
 *  found in the manual.
 *
 * */
#define SSI_OUTPUTCLOCK_50 50
#define SSI_OUTPUTCLOCK_100 100
#define SSI_OUTPUTCLOCK_1000 1000

#ifdef __KERNEL__

/** identification of a SSI module */
#define APCI_SSI_COUNTER 0x53490000

typedef struct
{
    int ssiProfile[ 4 ];
    int countingMode[ 4 ];
    int positionBitLength[ 4 ];
    int turnBitLength[ 4 ];
    unsigned int timervalue;
}

apci1710_ssi;



/**
 * Executed during insmod of this module
 * 
 * @return always 0
 */
int init_module( void );

/**
 * Executed during rmmod of this module
 */
void cleanup_module( void );

#endif // __KERNEL__

/**
 * This functions initializes the ssi module. This function MUST be called
 * before all the other functions.
 * 
 * @param dev contains information of the board (e.g. IO addresses), apci1710_get_device
 * from the apci1710 module returns the dev! 
 * @param moduleNb to initialize
 * @param ssiProfile the bit length of the SSI profile
 * @param positionBitLength number of bits used for the position
 * @param turnBitLength number of bits used for the turn
 * @param countingMode if conversion needed from gray to binary
 * @param ssiOutputClock determines the output clock and divides the PCI bus frequency
 *
 * @precondition 0 <= moduleNb <= 3
 * @precondition 2 <= ssiProfile <= 32
 * 
 * @return 0 if no problems
 * @return -1 if module moduleNb is NO SSI counter
 * @return -2 if the countingMode is incorrect 
 * 
 * @author Jef De Geeter
 */
int apci1710_ssi_init( apci1710_device* dev, int moduleNb, int ssiProfile, int positionBitLength, int turnBitLength, int countingMode, unsigned int ssiOutputClock );

/**
 * This function reads 1 value from 1 counter on a specific SSI module.
 *
 * @param dev contains information of the board (e.g. IO addresses)
 * @param moduleNb to read from
 * @param selectedSSI selects 1 counter, each SSI module contains 3 counters
 * @param result contains the uninterpreted data
 * 
 * @precondition 0 <= moduleNb <= 3
 * @precondition 0 <= selectedSSI <= 2
 * 
 * @return 0 if no problems
 * @return -1 if module moduleNb is NO SSI counter 
 *
 * @author Jef De Geeter
 */
int apci1710_ssi_read( apci1710_device* dev, int moduleNb, int selectedSSI, unsigned int* result );

/**
 * This functions reads all 3 values from 1 counter on a specific SSI module
 *
 * @param dev contains information of the board (e.g. IO addresses)
 * @param moduleNb to read from
 * @param result should be an array of at least length 3, first value is the result from the first counter...
 * 
 * @precondition 0 <= moduleNb <= 3
 * 
 * @return 0 if no problems
 * @return -1 if module moduleNb is NO SSI counter
 * 
 * @author Jef De Geeter
 */
int apci1710_ssi_read_all( apci1710_device* dev, int moduleNb, unsigned int* result );

/**
 * Instructs the card to refresh the modules position registers of all encoders.
 */
int apci1710_ssi_refresh(apci1710_device* dev, int moduleNb );


/**
 * Reads the value in the encoder position register.
 */
int apci1710_ssi_read_latest(apci1710_device* dev, int moduleNb, int selectedSSI, unsigned int* result );

/**
 * Reads the values of all the encoder position registers.
 */
int apci1710_ssi_read_latest_all( apci1710_device* dev, int moduleNb, unsigned int* result );


//@}

#endif

