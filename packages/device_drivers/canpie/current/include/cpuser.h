/*****************************************************************************\
*  CANpie                                                                     *
*                                                                             *
*  File        : cpuser.h                                                     *
*  Description : User functions                                               *
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
*  0.1      11.02.1999  Initial version                                 UK    *
*  0.2      29.04.1999  Changed structures, new data type definitions   UK    *
*  0.3      01.09.1999  Adaption to version 0.3 of the CANpie Doc.      UK    *
*  0.4      18.01.2000  added function calls in CpUserAppInit(), i.e.   UK    *
*                       CpUserBaudrate() and CpFilterAll(),                   *
*                       changed place of CpVar_InitFlag                       *
*  0.5                  - no changes -                                        *
*  0.6      15.06.2000  set to CANpie release 0.6                       UK    *
*  0.7                  - no changes -                                      	*
*  0.8                  - no changes -                                      	*
*                                                                             *
\*****************************************************************************/


#ifndef  _CANpie_User_
#define  _CANpie_User_


#include "cpconst.h"



/*!   \file    cpuser.h
**    \brief   CANpie user functions
**
**    The user functions always call the core functions, they never access the
**    hardware directly. That means the user functions do not have to be
**    modified when implementing the CANpie on an existing hardware. A typical
**    user function is the writing of a CAN message to the FIFO buffer
**    (e.g. CpUserMsgWrite() ).<p>
**    The CAN driver is initialized with the function CpUserAppInit().
**    This routine will call several core functions for the correct setup of
**    the CAN controller. The core functions called are:
**    <ul>
**    <li>CpFifoSetup()
**    <li>CpCoreInitDriver()
**    <li>CpUserBaudrate()
**    <li>CpUserFilterAll()
**    </ul>
**    The initial baudrate is set to 20kBit/s by default.
**    <b>The function CpUserAppInit() must be called before any other CANpie
**    function.</b>
**
*/


/*----------------------------------------------------------------------------*\
** Definitions																						**
**																										**
\*----------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
** A driver with only one channel and small memory resources does not need
** the 'channel' parameter.
** The definition CP_SMALL_CODE is checked for the value '1' and the
** function prototypes are converted then. The function call in the
** application stays the same (with 'channel' parameter).
**
*/
#if   CP_SMALL_CODE == 1

#define  channel  0

#define  CpUserAppInit(CH, A, B, C)          CpUserAppInit(A, B, C)
#define  CpUserAppDeInit(CH)                 CpUserAppDeInit()
#define  CpUserBaudrate(CH, A)               CpUserBaudrate(A)
#define  CpUserFifoClear(CH, A)              CpUserFifoClear(A)
#define  CpUserFilterAll(CH, A)              CpUserFilterAll(A)
#define  CpUserFilterMsg(CH, A, B)           CpUserFilterMsg(A, B)
#define  CpUserIntFunctions(CH, A, B, C)     CpUserIntFunctions(A, B, C)
#define  CpUserMsgRead(CH, A)                CpUserMsgRead(A)
#define  CpUserMsgWrite(CH, A)               CpUserMsgWrite(A)

#endif

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------------*\
** Function prototypes																			**
**																										**
\*----------------------------------------------------------------------------*/

/*!
** \brief   Initialize the CAN driver
** \param   channel        CAN channel of the hardware
** \param   rcvFifoSize    Size of the reception FIFO
**                         (number of messages)
** \param   trmFifoSize    Size of the transmit FIFO (number of messages)
** \param   timeout        Timeout value for transmission of messages in
**                         milliseconds
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** This function must be called by the application program prior to
** all other functions. It is responsible for initializing the CAN
** controller interface.
**
*/
Cp_EXPORT _U32 Cp_PREFIX CpUserAppInit(_U32 channel, _U32 rcvFifoSize, _U32 trmFifoSize, _U32 timeout);


/*!
** \brief   De-Initialize the CAN driver
** \param   channel        CAN channel of the hardware
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** This function must be called when the application program quits.
**
*/
Cp_EXPORT _U32 Cp_PREFIX CpUserAppDeInit(_U32 channel);


/*!
** \brief   Setup baudrate
** \param   channel        CAN channel of the hardware
** \param   baudrate       constant for the baudrate, taken from
**                         CP_BAUD enumeration
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** This function sets the baudrate of the selected CAN channel.
** Possible values for the baudrate are:
** <ul>
** <li>CP_BAUD_10K  -  10 kBit/s
** <li>CP_BAUD_20K  -  20 kBit/s
** <li>CP_BAUD_50K  -  50 kBit/s
** <li>CP_BAUD_100K - 100 kBit/s
** <li>CP_BAUD_125K - 125 kBit/s
** <li>CP_BAUD_250K - 250 kBit/s
** <li>CP_BAUD_500K - 500 kBit/s
** <li>CP_BAUD_800K - 800 kBit/s
** <li>CP_BAUD_1M   - 1 MBit/s
** </ul>
** It simply calls the CpCoreBaudrate() function with bit timing
** values that are defined in the table "CpStruct_BitTiming". The
** values of this table depend on the used CAN controller and the
** controller clock frequency.
**
*/
Cp_EXPORT _U32 Cp_PREFIX CpUserBaudrate(_U32 channel, _U32 baud);


/*!
** \brief   Clear FIFO buffer
** \param   channel        CAN channel of the hardware
** \param   buffer         CP_FIFO_RCV for the Receive FIFO or
**                         CP_FIFO_TRM for the Transmit FIFO
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** Deletes all messages of the specific FIFO buffer.
**
*/
Cp_EXPORT _U32 Cp_PREFIX CpUserFifoClear(_U32 channel, _U32 buffer);


/*!
** \brief   Set global filter
** \param   channel        CAN channel of the hardware
** \param   enable         TRUE for accepting all identifiers,
**                         FALSE for rejecting all identifiers
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** This function enables or disables the reception of CAN messages.
** If the flag <b>enable</b> is set to TRUE, all incoming CAN messages
** are accepted. If the flag <b>enable</b> is set to FALSE, all CAN
** messages are rejected. Please notice that this function only
** works for standard messages (2.0A, 11 bit identifier).
** This function is only available, if the member <b>SupportSWFilter</b>
** of the HDI structure (CpStruct_HDI) is set to TRUE.
**
*/
Cp_EXPORT _U32 Cp_PREFIX CpUserFilterAll(_U32 channel, _U32 enable);


/*!


** \brief   Set filter for single message
** \param   channel        CAN channel of the hardware
** \param   id             value of the identifier
** \param   enable         TRUE for accepting an identifier,
**                         FALSE for rejecting an identifier
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** This function sets a filter for CAN messages. If the flag <b>enable</b>
** is set to TRUE, the CAN identifier <b>id</b> is accepted. If the
** flag <b>enable</b> is set to FALSE, the reception of messages with the
** identifier <b>id</b> is not possible. Please notice that this
** function only works for standard messages (2.0A, 11 bit identifier).
** This function is only available, if the member <b>SupportSWFilter</b>
** of the HDI structure (CpStruct_HDI) is set to TRUE.
**
*/
Cp_EXPORT _U32 Cp_PREFIX CpUserFilterMsg(_U32 channel, _U32 id, _U32 enable);


/*!
** \brief   Install callback functions
** \param   channel        CAN channel of the hardware
** \param   rx_handler     Pointer to callback function which handles
**                         the receive interrupt of the CAN controller
** \param   tx_handler     Pointer to callback function which handles
**                         the transmit interrupt of the CAN controller
** \param   err_handler    Pointer to callback function which handles
**                         the error interrupt of the CAN controller
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** This function will install three different callback routines in the
** interrupt handler. If you do not want to install a callback for a
** certain interrupt condition or disable the callback, the parameter
** must be set to NULL.
**
*/
Cp_EXPORT _U32 Cp_PREFIX CpUserIntFunctions(  _U32 channel,
                                    _U32 (* rx_handler)  (_U32, CpStruct_CAN *),
                                    _U32 (* tx_handler)  (_U32, CpStruct_CAN *),
                                    _U32 (* err_handler) (_U32) );


/*!
** \brief   Read CAN message from Receive FIFO
** \param   channel        CAN channel of the hardware
** \param   msgPtr         Pointer to a structure of type CpStruct_CAN
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
*/
Cp_EXPORT _U32 Cp_PREFIX CpUserMsgRead(_U32 channel, CpStruct_CAN * msgPtr);


/*!
** \brief   Write CAN message to transmit FIFO
** \param   channel        CAN channel of the hardware
** \param   msgPtr         Pointer to a structure of type CpStruct_CAN
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
*/
Cp_EXPORT _U32 Cp_PREFIX CpUserMsgWrite(_U32 channel, const CpStruct_CAN * msgPtr);

#ifdef __cplusplus
}
#endif


#endif   /* _CANpie_User_ */

