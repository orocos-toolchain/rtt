/*****************************************************************************\
*  CANpie                                                                     *
*                                                                             *
*  File        : cpconst.h                                                    *
*  Description : Definitions / Constants for CANpie                           *
*  Author      : Uwe Koppe                                                    *
*  e-mail      : koppe@microcontrol.net                                       *
*                                                                             *
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
*                                                                             *
*   This program is free software; you can redistribute it and/or modify      *
*   it under the terms of the GNU General Public License as published by      *
*   the Free Software Foundation; either version 2 of the License, or         *
*   (at your option) any later version.                                       *
*                                                                             *
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
*                                                                             *
*  History                                                                    *
*  Vers.    Date        Comment                                         Aut.  *
*  -----    ----------  ---------------------------------------------   ----  *
*  0.1      04.12.1998  Initial version                                 UK    *
*  0.2      29.04.1999  Changed structures, new data type definitions   UK    *
*  0.3                  - no changes -                                        *
*  0.4                  - no changes -                                        *
*  0.5                  - no changes -                                        *
*  0.6      15.06.2000  moved defintions from cpmsg.h                   UK    *
*                       set to CANpie release 0.6                             *
*	0.7		06.11.2000	added new error codes									UK		*
*								added missing buffer number									*
*	0.8		15.02.2001	set to CANpie release 0.8								UK		*
*                                                                             *
\*****************************************************************************/


#ifndef  _CANpie_Constants_
#define  _CANpie_Constants_

/*-----------------------------------------------------------------------------
**    Online documentation for Doxygen
*/

/*!   \file    cpconst.h
**    \brief   CANpie constants, structures and enumerations
**
**    This file holds constants and structures used within CANpie.
**
*/

#include "compiler.h"      // compiler definitions
#include "cpconfig.h"      // configuration options


/*----------------------------------------------------------------------------*/
/*!
** \enum    CpErr
** \brief   CANpie Error codes
**
**
*/
enum CpErr {

   /*!   No error (00dec / 00hex)
   */
   CpErr_OK = 0,

   /*!   Error not specified (01dec / 01hex)
   */
   CpErr_GENERIC,

   /*!   Hardware failure (02dec / 02hex)
   */
   CpErr_HARDWARE,

   /*!   Initialisation failure (03dec / 03hex)
   */
   CpErr_INIT_FAIL,

   /*!   Channel is initialized, ready to run (04dec / 04hex)
   */
   CpErr_INIT_READY,

   /*!    CAN channel was not initialized (05dec / 05hex)
   */
   CpErr_INIT_MISSING,

   /*!   Receive buffer is empty (05dec / 05hex)
   */
	CpErr_RCV_EMPTY,

	/*!	Receive buffer overrun (06dec / 06hex)
	*/
	CpErr_OVERRUN,

   /*!   Transmit buffer is full (07dec / 07hex)
   */
	CpErr_TRM_FULL,


	/*!   CAN message has wrong format (10dec / 0Ahex)
	*/
   CpErr_CAN_MESSAGE = 10,

   /*!   CAN identifier not valid (11dec / 0Bhex)
   */
   CpErr_CAN_ID,


   /*!   FIFO is empty (20dec / 14hex)
   */
   CpErr_FIFO_EMPTY = 20,

   /*!   Message is waiting in FIFO (21dec / 15hex)
   */
   CpErr_FIFO_WAIT,

   /*!   FIFO is full (22dec / 16hex)
   */
   CpErr_FIFO_FULL,

   /*!   FIFO size is out of range (23dec / 17hex)
   */
   CpErr_FIFO_SIZE,


   /*!   Controller is in error passive (30dec / 1Ehex)
   */
   CpErr_BUS_PASSIVE = 30,

   /*!   Controller is in bus off (31dec / 1Fhex)
   */
   CpErr_BUS_OFF,

   /*!   Controller is in warning status (32dec / 20hex)
   */
   CpErr_BUS_WARNING,


   /*!   Channel out of range (40dec / 28hex)
   */
   CpErr_CHANNEL = 40,

   /*!   Register address out of range (41dec / 29hex)
   */
   CpErr_REGISTER,

   /*!   Baudrate out of range (42dec / 2Ahex)
   */
   CpErr_BAUDRATE,


   /*!   Function is not supported (50dec / 32hex)
   */
   CpErr_NOT_SUPPORTED = 50
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CP_CC
** \brief   CAN controller identification numbers
**
*/
enum CP_CC {

   /*!   Philips 82C200
   */
   CP_CC_82C200 = 0,

   /*!   Philips SJA1000
   */
   CP_CC_SJA1000,

   /*!   Philips 80C591
   */
   CP_CC_80C591,

   /*!   Philips 80C592
   */
   CP_CC_80C592,


   /*!   Infineon C505
   */
   CP_CC_C505 = 20,

   /*!   Infineon C515
   */
   CP_CC_C515,

   /*!   Infineon C161
   */
   CP_CC_C161,

   /*!   Infineon C164
   */
   CP_CC_C164,

   /*!   Infineon C167
   */
   CP_CC_C167,

   /*!   Infineon 81C90
   */
   CP_CC_81C90,

   /*!   Infineon 81C91
   */
   CP_CC_81C91,

   /*!   Intel AN82527
   */
   CP_CC_AN82527 = 40,

   /*!   Intel AN87C196CA
   */
   CP_CC_AN87C196CA,

   /*!   Intel AN87C196CB
   */
   CP_CC_AN87C196CB,

   /*!   Motorola 68HC05
   */
   CP_CC_68HC05 = 60,

   /*!   Motorola 68HC08
   */
   CP_CC_68HC08,

   /*!   Motorola 68HC912
   */
   CP_CC_68HC912,

   /*!   Motorola 68376
   */
   CP_CC_MC68376,

   /*!   Motorola MPC555
   */
   CP_CC_MPC555,

   /*!   Microchip MCP2510
   */
   CP_CC_MCP2510 = 80
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CP_FIFO
** \brief   FIFO Buffer numbers
*/
enum CP_FIFO {
   CP_FIFO_RCV = 0,
   CP_FIFO_TRM
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CP_CALLBACK
** \brief   Callback Return Codes
**
** These return values are used by the callback functions that can be
** installed by the function CpUserIntFunctions(). <p>
** \b Example
** \code
** _U08 MyCallback(CpStruct_CAN * pCanMsgV)
** {
**    // Do something with IDs < 100
**    if( CpMacGetStdId(pCanMsgV) < 100)
**    {
**       //.....
**       return(CP_CALLBACK_PROCESSED)
**    }
**
**    // Put all other messages into the FIFO
**    return (CP_CALLBACK_PUSH_FIFO);
** }
** \endcode
** <br>
*/
enum CP_CALLBACK {

   /*!
   ** Message was processed by callback and is not inserted in the FIFO
   */
   CP_CALLBACK_PROCESSED = 0,

   /*!
   ** Message was processed by callback and is inserted in the FIFO
   */
   CP_CALLBACK_PUSH_FIFO
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CP_BAUD
** \brief   Fixed baudrates
**
** The values of the enumeration CP_BAUD are used as parameter for the
** function CpUserBaudrate().
*/
enum CP_BAUD {
   /*!
   ** Baudrate 10 kBit/sec
   */
   CP_BAUD_10K = 0,

   /*!
   ** Baudrate 20 kBit/sec
   */
   CP_BAUD_20K,

   /*!
   ** Baudrate 50 kBit/sec
   */
   CP_BAUD_50K,

   /*!
   ** Baudrate 100 kBit/sec
   */
   CP_BAUD_100K,

   /*!
   ** Baudrate 125 kBit/sec
   */
   CP_BAUD_125K,

   /*!
   ** Baudrate 250 kBit/sec
   */
   CP_BAUD_250K,

   /*!
   ** Baudrate 500 kBit/sec
   */
   CP_BAUD_500K,

   /*!
   ** Baudrate 800 kBit/sec
   */
   CP_BAUD_800K,

   /*!
   ** Baudrate 1 MBit/sec
   */
   CP_BAUD_1M
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CP_CHANNEL
** \brief   Channel definition
*/
enum CP_CHANNEL {
   CP_CHANNEL_1 = 0,
   CP_CHANNEL_2,
   CP_CHANNEL_3,
   CP_CHANNEL_4,
   CP_CHANNEL_5,
   CP_CHANNEL_6,
   CP_CHANNEL_7,
   CP_CHANNEL_8
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CP_MODE
** \brief   Mode of CAN controller
**
** These values are used as parameter for the function CpCoreCANMode() in
** order to change the state of the CAN controller.
*/
enum CP_MODE {
   /*!   Set controller in Stop mode (no reception / transmission possible)
   */
   CP_MODE_STOP = 0,

   /*!   Set controller into normal operation
   */
   CP_MODE_START,

   /*!	Start Autobaud Detection
   */
   CP_MODE_AUTO_BAUD,

   /*!	Set controller into Sleep mode
   */
   CP_MODE_SLEEP
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CP_STATE
** \brief   State of CAN controller
**
** These values are used as return value for the function CpCoreCANState().
*/
enum CP_STATE {
	/*!
	**	CAN controller is active, no errors
	*/
	CP_STATE_ACTIVE    = 0,

	/*!
	**	CAN controller is in stopped mode
	*/
	CP_STATE_STOPPED,

	/*!
	**	CAN controller is in Sleep mode
	*/
	CP_STATE_SLEEPING,

	/*!
	**	CAN controller is active, warning level is reached
	*/
	CP_STATE_BUS_WARN  = 6,

	/*!
	**	CAN controller went into Bus Off
	*/
	CP_STATE_BUS_OFF,

	/*!
	**	General failure of physical layer detected (if supported by hardware)
	*/
	CP_STATE_PHY_FAULT = 10,

	/*!
	**	Fault on CAN-H detected (Low Speed CAN)
	*/
	CP_STATE_PHY_H,

	/*!
	**	Fault on CAN-L detected (Low Speed CAN)
	*/
	CP_STATE_PHY_L,

	CP_STATE_ERR_BIT   = 0x10,
	CP_STATE_ERR_STUFF = 0x20,
	CP_STATE_ERR_FORM	 = 0x30,
	CP_STATE_ERR_CRC   = 0x40,
	CP_STATE_ERR_ACK   = 0x50
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CP_BUFFER
** \brief   Buffer definition
**
** The enumeration CP_BUFFER is used to define a message buffer inside a
** FullCAN controller. The index for the first buffer starts at 1.
*/
enum CP_BUFFER {
   CP_BUFFER_1 = 1,
   CP_BUFFER_2,
   CP_BUFFER_3,
   CP_BUFFER_4,
   CP_BUFFER_5,
   CP_BUFFER_6,
   CP_BUFFER_7,
   CP_BUFFER_8,
   CP_BUFFER_9,
   CP_BUFFER_10,
   CP_BUFFER_11,
   CP_BUFFER_12,
   CP_BUFFER_13,
   CP_BUFFER_14,
   CP_BUFFER_15
};


/*----------------------------------------------------------------------------*/
/*!
** \enum    CP_BUFFER_DIR
** \brief   Buffer direction definition
*/
enum CP_BUFFER_DIR {
   /*!
   **    Buffer direction is receive
   */
   CP_BUFFER_DIR_RX = 0,

   /*!
   **    Buffer direction is transmit
   */
   CP_BUFFER_DIR_TX
};


/*----------------------------------------------------------------------------*/
/*!
** \struct  CpStruct_CAN   cpconst.h
** \brief   CAN message structure
**
** For transmission and reception of CAN messages a structure which holds
** all necessary informations is used. The structure has the following
** data fields:
*/
typedef struct {
   /*!   The identifier field may have 11 bits for standard frames
   **    (CAN specification 2.0A) or 29 bits for extended frames
   **    (CAN specification 2.0B). The three most significant bits
   **    are reserved for special functionality (the LSB is Bit 0,
   **    the MSB is Bit 31 ).<p>
   **    <ul>
   **    <li>Bit 31: Bit value 1 marks the identifier as an
   **                extended frame. Bit value 0 marks the
   **                identifier as a standard frame.
   **    <li>Bit 30: Bit value 1 marks the identifier
   **                as an remote transmission (RTR).
   **    <li>Bit 29: Reserved for future use
   **    </ul>
   */
   _U32  v_MsgId;

   /*!   The message flags field contains the data length code
   **    (DLC) of the CAN message and the buffer number when
   **    using a FullCAN controller.<p>
   **    The data length code (<b>Bit 0 - Bit 3</b>) contains the
   **    number of data bytes which are transmitted by a message.
   **    The possible value range for the data length code is
   **    from 0 to 8 (bytes).<p>
   **    A FullCAN controller (e.g. AN82527) has more than only
   **    one transmit and one receive buffer and offers more
   **    sophisticated message filtering. The field message
   **    buffer (<b>Bit 4 - Bit 7</b>) specifies the buffer for
   **    message transmission or reception.<p>
   **    The high word (<b>Bit 16 - Bit 31</b>)
   **    is reserved for user defined data.
   */
   _U32  v_MsgFlags;

   /*!   The data fields contain up to eight bytes for a CAN
   **    message. If the data length code is less than 8, the
   **    value of the unused data bytes will be undefined.
   */
   _U08  v_MsgData[8];

#if CP_MSG_TIME == 1
   /*!   The time stamp field defines the time when a CAN message
   **    was received by the CAN controller. The time stamp is a
   **    relative value, which is created by a free running timer.
   **    The time base is one microsecond (1 us). This means a
   **    maximum time span of 4294,96 seconds (1 hour 11 minutes)
   **    between two messages can be measured. This is an optional
   **    field (available if #CP_MSG_TIME is set to 1).
   */
   _U32  v_MsgTime;
#endif

#if CP_USR_DATA == 1
   /*!   The field user data can hold a 32 bit value, which is
   **    defined by the user. This is an optional field
   **    (available if #CP_USR_DATA is set to 1).
   */
   _U32  v_UsrData;
#endif

} CpStruct_CAN;


/*----------------------------------------------------------------------------*/
/*!
** \struct  CpStruct_HDI   cpconst.h
** \brief   Hardware description interface
**
** The Hardware Description Interface provides a method to gather
** information about the CAN hardware and the functionality of the driver.
** All items in the structure CpStruct_HDI are constant and must be
** supplied by the designer of the CAN driver. The hardware description
** structure is available for every physical CAN channel.
*/

typedef struct {
   /*!   Bit coded value that decribes the features of the CAN driver.
   **    <ul>
   **    <li>Bit 0/1: 0 = Standard Frame, 1 = Extended Frame passive,
   **        2 = Extended Frame active
   **    <li>Bit 2: 0 = BasicCAN, 1 = FullCAN
   **    <li>Bit 3: 0 = No IRQ Handler, 1 = Has IRQ Handler
   **    <li>Bit 4: 0 = No identifier filter, 1 = software identifier filter
   **    <li>Bit 5: 0 = No timestamp, 1 = has timestamp
   **    <li>Bit 6: 0 = No user data field, 1 = has user data field
   **    <li>Bit 7: reserved
   **    </ul>
   */
   _U08  v_SupportFlags;

   /*!   Constant value that identifies the used CAN controller
   **    chip. Possible values for this member are listed
   **    in the header file cpconst.h
   */
   _U08  v_ControllerType;

   /*!   Defines the number of the interrupt which is used.
   **    If the flag IRQHandler is set to 0, the value of
   **    IRQNumber will be undefined.
   */
   _U08  v_IRQNumber;

   /*!   Holds the major version number of the CANpie driver
   **    release (current value = 0).
   */
   _U08  v_VersionMajor;

   /*!   Holds the minor version number of the CANpie
   **    driver release (current value = 4).
   */
   _U08  v_VersionMinor;

   /*!   A null terminated string which holds the name of the
   **    specific CAN driver. The string can be used to
   **    distinguish between different vendors.
   */
   char* v_DriverName;

   /*!   A null terminated string which holds the version of
   **    the specific CAN driver (e.g. DLL version, firmware
   **    version).
   */
   char* v_VersionNumber;
} CpStruct_HDI;


/*-------------------------------------------------------------------------
** CpStruct_BitTimingValue
** Bit timing structure
*/
typedef struct {
   _U08     btr0;
   _U08     btr1;
   _U08     sjw;
} CpStruct_BitTimingValue;


/*----------------------------------------------------------------------------*\
** Definitions                                                                **
**                                                                            **
\*----------------------------------------------------------------------------*/

#define  CP_VERSION_MAJOR           0
#define  CP_VERSION_MINOR           8

#define  CP_NUMBER_OF_BAUDRATES     9

#define  CP_BIT_TIMING_VALUE_SIZE   sizeof(CpStruct_BitTimingValue)
#define  CP_BIT_TIMING_TABLE_SIZE   (CP_BIT_TIMING_VALUE_SIZE * CP_NUMBER_OF_BAUDRATES)

/*-------------------------------------------------------------------------
** For dynamic memory allocation we need the size of a CAN message
** structure. For 32 Bit operating systems (WinNT/Linux) an alignment
** is necessary. For little micros we can save memory if only the required
** memory space is assigned by malloc(). Make sure to select the right
** option here in case you have problems with the FIFOs.
**
*/
#if   CP_SMALL_CODE == 1
#define  CP_CAN_MESSAGE_SIZE     sizeof(CpStruct_CAN)
#else
#define  CP_CAN_MESSAGE_SIZE     32
#endif



/*-----------------------------------------------------------------------------
** CP_MASK_XXX
** mask for CAN message (RTR, Extended CAN)
*/


/*-------------------------------------------------------------------*/
/*!
** \def  CP_MASK_STD_FRAME
**
** Mask for standard frame (11 bits)
*/
#define  CP_MASK_STD_FRAME 0x000007FF


/*-------------------------------------------------------------------*/
/*!
** \def  CP_MASK_EXT_FRAME
**
** Mask for extended frame (29 bits)
*/
#define  CP_MASK_EXT_FRAME 0x1FFFFFFF


/*-------------------------------------------------------------------*/
/*!
** \def  CP_MASK_EXT_BIT
**
** Set the EXT bit in the v_MsgId field
*/
#define  CP_MASK_EXT_BIT   0x80000000


/*-------------------------------------------------------------------*/
/*!
** \def  CP_MASK_RTR_BIT
**
** Set the RTR bit in the v_MsgId field
*/
#define  CP_MASK_RTR_BIT   0x40000000


/*-------------------------------------------------------------------*/
/*!
** \def  CP_MASK_DLC_BITS
**
** Mask for DLC field in v_MsgFlags
*/
#define  CP_MASK_DLC_BITS  0x0000000F


/*-------------------------------------------------------------------*/
/*!
** \def  CP_MASK_BUF_BITS
**
** Mask for buffer field in v_MsgFlags
*/
#define  CP_MASK_BUF_BITS  0x000000F0


#endif   /* _CANpie_Constants_   */
