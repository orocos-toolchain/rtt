/*****************************************************************************\
*  CANpie                                                                     *
*                                                                             *
*  File        : cpconfig.h                                                   *
*  Description : Configuration file for CANpie                                *
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
*  0.1      01.09.1999  Initial version                                 UK    *
*  0.2                  - no changes -                                        *
*  0.3                  - no changes -                                        *
*  0.4                  - no changes -                                        *
*  0.5                  - no changes -                                        *
*  0.6                  - no changes -                                        *
*  0.7                  - no changes -                                        *
*  0.6      26.02.2001  set to CANpie release 0.6                       UK    *
*  0.7                  - no changes -                                      	*
*  0.8                  - no changes -                                      	*
*                                                                             *
\*****************************************************************************/

#ifndef  _CANpie_Configuration_
#define  _CANpie_Configuration_

/*-----------------------------------------------------------------------------
**    Online documentation for Doxygen
*/

/*----------------------------------------------------------------------------*/
/*!
** \file    cpconfig.h
** \brief   CANpie configuration file
**
** Please set the definitions in this file for a specific CANpie behaviour.
** For detailed information please refer to the CANpie documentation
** (canpie.pdf).
**
*/


/*----------------------------------------------------------------------------*/
/*!
** \def     CP_MSG_TIME
** \brief   CpStruct_CAN configuration
**
** This symbol defines if the element v_MsgTime is included in the structure
** CpStruct_CAN. This structure member is only needed for CAN interfaces that
** can handle time stamps.
**
** \li 0 = CAN message does not store time data
** \li 1 = CAN message can store time data
**
*/
#define  CP_MSG_TIME       1


/*----------------------------------------------------------------------------*/
/*!
** \def     CP_USR_DATA
** \brief   CpStruct_CAN configuration
**
** This symbol defines if the element v_UsrData is included in the structure
** CpStruct_CAN. This structure member is only needed for CAN interfaces if
** the application needs to store a 32-bit value for each message.
**
** \li 0 = CAN message does not store user data
** \li 1 = CAN message can store user data
**
*/
#define  CP_USR_DATA       1


/*----------------------------------------------------------------------------*/
/*!
** \def     CP_SW_FILTER
** \brief   Identifier filter for CANpie
**
** \li 0 = CAN driver does not support identifier filter
** \li 1 = CAN driver uses CANpie identifier filter
** \li 2 = CAN driver uses firmware identifier filter (PC board)
**
*/
#define  CP_SW_FILTER      2


/*----------------------------------------------------------------------------*/
/*!
** \def     CP_FULL_CAN
** \brief   Enable FullCAN function support
**
** This symbol enables support for a FullCAN controller (e.g
** <b>CpCoreAllocBuffer()</b> ).
**
** \li 0 = FullCAN controller is not supported
** \li 1 = FullCAN controller is supported
**
*/
#define  CP_FULL_CAN       1


/*----------------------------------------------------------------------------*/
/*!
** \def     CP_INT_HANDLER
** \brief   Interrupt handler support
**
** \li 0 = CAN driver does not support an interrupt handler
** \li 1 = CAN driver supports an interrupt handler
**
*/
#define  CP_INT_HANDLER    1


/*----------------------------------------------------------------------------*/
/*!
** \def     CP_CHANNEL_MAX
** \brief   Number of CAN channels
**
** This symbol defines the highest support CAN channel for CANpie. Counting
** starts from 1, e.g. a value of 2 means the driver supports two CAN channels.
** However, in the functions the parameter ´ubChannelV´ starts from 0 (zero) !
**
*/
#define  CP_CHANNEL_MAX    1


/*----------------------------------------------------------------------------*/
/*!
** \def     CP_CAN_BASE_ADR
** \brief   CAN controller base address
**
** Base address of the CAN controller in the address space.
**
*/
#define  CP_CAN_BASE_ADR   0


/*----------------------------------------------------------------------------*/
/*!
** \def     CP_DEFAULT_BAUDRATE
** \brief   Default baudrate for CpUserAppInit()
**
**	This symbol defines the default baudrate of the CAN interface after
**	calling CpUserAppInit(). The CANpie default value is 20 kBaud.
*/
#define	CP_DEFAULT_BAUDRATE		CP_BAUD_20K


#endif
