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

#ifndef APCI1710_INCRMODULE
#define APCI1710_INCRMODULE

/* Incremental counter module
 *
 * Still in development!!!!
 */

#include "apci1710.h"

/** @name INCR module
 *
 * @memo this module contains all INCR specfic stuff.
 * @memo You need both this module and the apci1710 module
 * @memo to do something.
 * @memo To do something with interrupts, you need
 * @memo the apci1710irqhandler module
 *
 * Still in development!!! 
 * 
 * interrupts do not work, but the other base functionality
 * should be quite finished.
 * */ 
//@{

#ifdef __KERNEL__

/**************************
// COUNTER INITIALISATION
***************************/

/*
 * This functions initializes the incr module. This function MUST be called
 * before all the other functions.
 *
 * @param dev    : pointer to an apci1710 device
 * @param moduleNb   : which of the four counter chips on the board (0,1,2,3)
 * @param counterRange   : APCI1710_16BIT_COUNTER or APCI1710_32BIT_COUNTER
 * @param firstCounterModus  : APCI1710_QUADRUPLE_MODE, APCI1710_DOUBLE_MODE, APCI1710_SIMPLE_MODE or AP1710_DIRECT_MODE
 * @param firstCounterOption  : if direct mode then APCI1710_INCREMENT or APCI1710_DECREMENT else APCI1710_HYSTERESIS_ON or APCI1710_HYSTERESIS_OFF
 * @param secondCounterModus  : see firstCounterModus
 * @param secondCounterOption  : see secondCounterOption
 * @param module   : (is supposed empty and will be initialized by this function)
 *
 * @return 0 if no problems
 * @return -1 if dev is not a apci1710 device
 * @return -2 if moduleNb is not 0 1 2 or 3
 * @return -3 if module moduleNb is no incremental counter
 * @return -4 if counterRange is invalid
 * @return -5 if firstCounterModus invalid
 * @return -6 if firstCounterOption invalid
 * @return -7 if secondCounterModus invalid
 * @return -8 if secondCounterOption invalid
 * 
 * @author Filip Vancoillie
 * @author Jef Degeerter
 */
int apci1710_incr_init( apci1710_device* dev, unsigned int moduleNb, int counterRange, int firstCounterModus, int firstCounterOption, int secondCounterModus, int secondCounterOption, apci1710_module* module );

/**
 * Optional function to be called before apci1710_incr_init. 
 * Creates a module struct and returns a pointer to it.
 * You can clean it up again with apci1710_incr_cleanup_module()
 *
 * @return null if the operation failed, a valid struct otherwise.
 */
apci1710_module* apci1710_incr_create_module(void);

/**
 * This one deallocates *module. Only call this if you provided '0' for module with the init call.
 */
int apci1710_incr_cleanup_module(apci1710_module* module);

/*
 * This function tests the component
 * 
 *
 * @author Jef De Geeter 
 * @memo Not yet implemented
 */
int apci1710_incr_counter_auto_test( apci1710_module* module );

/*
 * This function clears the counter value of the selected module.
 *
 * @param module   : see apci1710.h for more info
 * 
 * @return 0 if no problems
 * @return -1 if module is not an incremental counter
 * 
 * @author Filip Vancoillie
 * @author Jef Degeeter
 */
int apci1710_incr_clear_counter_value( apci1710_module* module );

/*
 * Clears all counter values of all the incremental counter modules
 *
 * Not yet implemented
 *
 * @author Filip Vancoillie
 * @author Jef De Geeter
 */
int apci1710_incr_clear_all_counter_value( apci1710_module* module );

/*
 * Disables or enables the software filter in the selected module.
 *
 * @author Jef De Geeter
 * @memo Not yet implemented
 */
int apci1710_incr_set_input_filter( apci1710_module* module, int PCIclock, int filter );

/*********************
// READING THE COUNTER
*********************/ 
/*
 * This function latches the current value of the selected module into the selected latch register
 *
 * @param module                : see apci1710.h for more info
 * @param latchRegister  : 0 for the first latch register, 1 for the second latch register
 *
 * @return 0 if no problems
 * @return -1 if not an incremental counter
 * @return -2 if latchRegister is not 0 or 1
 * 
 * @author Filip Vancoillie 
 * @author Jef De Geeter
 */
int apci1710_incr_latch_counter( apci1710_module* module, unsigned int latchRegister );

/*
 * Reads the latch register status of the selected module and selected latch register
 *
 * @param module  : see apci1710.h for more info
 * @param latchRegister  : 0 for the first latch register, 1 for the second latch register
 * 
 * @return latchStatus : 0 if no latch occurred, 1 if a software latch has occurred, 2 if a hardware latch has occurred, 3 if a software and hardware latch have occurred.
 *
 * @return 0 if no problems
 * @return -1 if not an incremental counter
 * @return -2 if latchRegister is not 0 or 1
 * 
 * @author Filip Vancoillie
 * @author Jef De Geeter
 */
int apci1710_incr_read_latch_register_status( apci1710_module* module, unsigned int latchRegister, int* latchStatus );

/*
 * Reads the latch register value of the selected module and the selected latch register
 *
 * @param module contains information on device, modulenumber and protocol (ssi, incremental, ...)
 * @param latchRegister 0 for the first latch register, 1 for the second latch register
 *  
 * @return result contains the latch register value
 * 
 * @return 0 if no problems
 * @return -1 if not an incremental counter
 * @return -2 if latchRegister is not 0 or 1
 * 
 * @author Filip Vancoillie
 * @author Jan Veldeman
 * @author Jef De Geeter
 */
int apci1710_incr_read_latch_register_value( apci1710_module* module, unsigned int latchRegister, unsigned int* result );

/*
 * Latches the module, and reads the 16bit counter 
 * value from the selected latchRegister.
 * It then puts the read value in "result".
 * This function combines apci1710_latch_counter
 * and apci1710_read_latch_register_value.
 *
 * @param module contains information on device, modulenumber and protocol (ssi, incremental, ...)
 * @param latchRegister 0 for the first latch register
 *    1 for the second latch register
 *
 * @return result contains the latch register value
 *
 * @return 0 if no problems
 * @return -1 if module is not an incremental counter
 * @return -2 if latchRegister is not 0 or 1
 * @return -3 if module is not a 16bit counter
 *
 * @author Filip Vancoillie 
*/

int apci1710_incr_read_16bit_counter_value( apci1710_module* module, unsigned int latchRegister, unsigned int* result );

/*
 * This function enables the latch interrupt for "module".
 * For each software or hardware latch, there occurs an interrupt.
 * 
 * @param module : this struct has been initialized by apci1710_incr_init
 *                              see apci1710.h for details
 * 
 * @return 0 if no problem
 * 
 * @author Filip Vancoillie
 */

int apci1710_incr_enable_latch_interrupt( apci1710_module* module );

/*
 * This function disables latch interrupt for "module".
 *
 * @param module : this struct has been initialized by apci1710_incr_init
 *                              see apci1710.h for details
 *   
 * @author Filip Vancoillie
 */

int apci1710_incr_disable_latch_interrupt( apci1710_module* module );

/*
 * Latches the module, and reads the 32bit counter value from the right register.
 * It then puts the read value in "result".
 * This function combines apci1710_latch_counter
 * and apci1710_read_latch_register_value.
 *
 * @param module contains information on device, modulenumber and protocol (ssi, incremental, ...)
 *
 * @return result contains the latch register value
 *
 * @return 0 if no problems
 * @return -1 if module is not an incremental counter
 * @return -2 if module is not a 32bit counter
 *
 * @author Filip Vancoillie
*/

int apci1710_incr_read_32bit_counter_value( apci1710_module* module, unsigned int* result );

/************************
// WRITING IN THE COUNTER
************************/

/*
 * Writes a 16-bit value into the selcted 16-bit counter of the selected module
 *
 * @param module   : this struct has been initialized by apci1710_incr_init
 *                               see apci1710.h for details
 * @param selectedCounter  : 0 for first of two 16bit counters, 1 is the second
 * @param writeValue  : 16bit value that shall be written to the counter
 *
 * @return 0 if no problems
 * @return -1 if module is not an incremental counter
 * @return -2 if selectedCounter is not 0 or 1
 * @return -3 if module is not a 16bit counter
 * 
 * @author Filip Vancoillie
 * @author Jef De Geeter
 */
int apci1710_incr_write_16bit_counter_value( apci1710_module* module, unsigned int selectedCounter, unsigned int writeValue );

/*
 * Writes a 32-bit value into the 32-bit counter of the selected module
 *
 * @param module   : this struct has been initialized by apci1710_incr_init
 *                               see apci1710.h for details
 * @param writeValue            : 32bit value that shall be written to the counter
 *
 * @return 0 if no problems
 * @return -1 if module is not an incremental counter
 * @return -2 if module is not a 32bit counter
 *
 * @author Filip Vancoillie
 * @author Jef De Geeter
 */
int apci1710_incr_write_32bit_counter_value( apci1710_module* module, unsigned int writeValue );

/*****************
// INDEX FUNCTIONS
*****************/

/*
 * Initializes the module (which must be 32bit (I think) )
 * to handle index signals without actually enabling the index.
 *
 * This index-signal is meant to keep track of where you
 * are when several overflows of the counter occured.
 *
 * 
 * @param module : this struct has been initialized by apci1710_incr_init
 *     see apci1710.h for details
 * @param referenceAtion : APCI1710_ENABLE_INDEX_ACTION or APCI1710_DISABLE_INDEX_ACTION
 * @param indexOperation : APCI1710_INDEX_LATCH_COUNTER or APCI1710_INDEX_CLEAR_COUNTER
 *     this determines what will happen when an index
 *     signal occurs
 * @param autoMode : APCI1710_ENABLE_INDEX_AUTO_MODE or APCI1710_DISABLE_INDEX_AUTO_MODE
 *     this determines if the index bit in the state register
 *     will be reset autmatically, or if it will be reset when
 *     read
 * @param interruptMode : APCI1710_ENABLE_INDEX_INT or APCI1710_DISABLE_INDEX_INT
 *     this enables, or disables interrupts
 *
 *
 *    ***************************************************
 *    ATTENTION : INTERRUPTS ARE NOT PROPERLY IMPLEMENTED
 *    ***************************************************
 *
 * 
 * @return 0 if no problem
 * @return -1 if module is not an incremental counter
 * @return -2 if module is not a 32bit counter
 * @return -3 if referenceAction is invalid
 * @return -4 if indexOperation is invalid
 * @return -5 if autoMode is invalid
 * @return -6 if interruptMode is invalid
 * @return -7 if index-interrupts are enabled while there is
 *   no interrupt handler installed.
 *
 * @author Filip Vancoillie
 */

int apci1710_incr_init_index( apci1710_module* module, int referenceAction, int indexOperation, int autoMode, int interruptMode );

/*
 * This function enables handling index signals for "module"
 *
 * @param module : this struct has been initialized by apci1710_incr_init
 *    see apci1710.h for details
 *
 * @return 0 if no problem
 * @return -1 if module is not an incremental counter
 * @return -2 if module has not been initialized for handling index
 *     signals with "apci1710_incr_init_index"
 *
 * @author Filip Vancoillie
 */
int apci1710_incr_enable_index( apci1710_module* module );

/* This function disables handling index signals for "module"
 *
 * @param module : this struct has been initialized by apci1710_incr_init
 *                              see apci1710.h for details
 *
 * @return 0 if no problem
 * @return -1 if module is not an incremental counter
 * @return -2 if module has not been initialized for handling index
 *                              signals with "apci1710_incr_init_index"
 *
 * @author Filip Vancoillie
 */

int apci1710_incr_disable_index( apci1710_module* module );

/*
 * This function gets the index status from the state register.
 * 
 * @param module : this struct has been initialized by apci1710_incr_init
 *                              see apci1710.h for details
 * 
 * @return result : 0 if no index signal occured
 *    1 if an index signal occurred
 *    3 if ???  I must look up this one XXX
 * @return 0 if no problem
 * @return -1 if module is not an incremental counter
 * @return -2 if module has not been initialized to handle index
 *     signals with "apci1710_incr_init_index"
 *
 * @author Filip Vancoillie
 */

int apci1710_incr_get_index_status( apci1710_module* module, int* result );

// REFERENCE

// UAS, CB, U/D#, EXTERNAL STROBE

// COMPARE LOGIC

// FREQUENCY MEASUREMENT

// DIGITAL OUTPUT

int apci1710_incr_set_digital_channel_on( apci1710_module* module );
int apci1710_incr_set_digital_channel_off( apci1710_module* module );

/**
 * Executed during insmod of this module
 * 
 * @return always 0
 *
 * @author Jan Veldeman
 */
int init_module( void );

/**
 * Executed during rmmod of this module
 *
 * @author Jan Veldeman
 */
void cleanup_module( void );

//@}

#endif /*__KERNEL__*/
#endif


