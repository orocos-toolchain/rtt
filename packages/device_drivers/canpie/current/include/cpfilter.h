/*****************************************************************************\
*  CANpie                                                                     *
*                                                                             *
*  File        : cpfilter.h                                                   *
*  Description :                                                              *
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
*  0.1      06.09.1999  Initial version                                 UK    *
*  0.2                  - no changes -                                        *
*  0.3                  - no changes -                                        *
*  0.4                  - no changes -                                        *
*  0.5                  - no changes -                                        *
*  0.6      15.06.2000  set to CANpie release 0.6                       UK    *
*  0.7                  - no changes -                                      	*
*  0.8                  - no changes -                                      	*
*                                                                             *
\*****************************************************************************/


#ifndef  _CANpie_Filter_
#define  _CANpie_Filter_


#include "cpconst.h"

/*-----------------------------------------------------------------------------
**    Online documentation for Doxygen
*/

/*!
** \file    cpfilter.h
** \brief   CANpie filter functions
**
** On its way to the Receive FIFO the CAN message must pass an optional
** software identifier filter. This software filter can be used by the
** programmer to enhance the capabilities of the CAN controller. The
** software filter only supports CAN standard frames (2.0A), because
** otherwise the overhead for extended frames would be too big. For
** transmission of messages the filter is not  used.<p>
** <b>
** These functions are called by the CANpie framework. It is not
** intended to use them in an application program.
** </b>
*/

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

#define  CpFilterAll(CH, A)                  CpFilterAll(A)
#define  CpFilterMsg(CH, A, B)               CpFilterMsg(A, B)
#define  CpFilterTest(CH, A)                 CpFilterTest(A)

#endif


/*-------------------------------------------------------------------------
** Function prototypes
**
*/

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
** This function is called by the user function CpUserFilterAll()
** if the member <b>SupportSWFilter</b> of the HDI structure
** (CpStruct_HDI) is set to TRUE and the driver does not use a
** firmware filter.
**
*/
_U08 Cp_PREFIX CpFilterAll(_U08 channel, _BIT enable);


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
** This function is called by the user function CpUserFilterMsg()
** if the member <b>SupportSWFilter</b> of the HDI structure
** (CpStruct_HDI) is set to TRUE and the driver does not use a
** firmware filter.
**
*/
_U08 Cp_PREFIX CpFilterMsg(_U08 channel, _U16 id, _BIT enable);


/*!
** \brief   Test identifier
** \param   channel        CAN channel of the hardware
** \param   id             value of the identifier
**
** \return  CpErr_OK for accepting an identifier,<br>
**          CpErr_GENERIC for rejecting an identifer
**
** This function is called by the framework to check if an identifier
** is valid..
**
*/
_U08 Cp_PREFIX CpFilterTest(_U08 channel, _U16 id);


#endif   /* _CANpie_Filter_ */

