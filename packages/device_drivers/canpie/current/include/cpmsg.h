/*****************************************************************************\
*  CANpie                                                                     *
*                                                                             *
*  File        : cpmsg.h                                                      *
*  Description : This file supplies some support functions for access of the  *
*                "CpStruct_CAN" message structure. It is recommended to use   *
*                these functions and not to access the structure directly in  *
*                the application.                                             *
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
*  0.1      09.09.1999  Initial version                                 UK    *
*  0.2                  - no changes -                                        *
*  0.3                  - no changes -                                        *
*  0.4                  - no changes -                                        *
*  0.5                  - no changes -                                        *
*  0.6      15.06.2000  added online documentation                      UK    *
*                       set to CANpie release 0.6                             *
*                                                                             *
\*****************************************************************************/



#ifndef  _CANpie_MessageSupport_
#define  _CANpie_MessageSupport_


#include "cpconst.h"

/*-----------------------------------------------------------------------------
**    Online documentation for Doxygen
*/

/*!
** \file    cpmsg.h
** \brief   CANpie message support functions
**
** This file includes additional function for an easy access to the members
** of the CAN message structure (CpStruct_CAN). It is recommended to use
** these functions rather than accessing the structure elements directly.
** Another way (better performance) to access the structure is to use the
** macros defined in the 'cpmacro.h' file.
*/


/*-------------------------------------------------------------------------
** Function prototypes
**
*/

/*!
** \brief   Get Standard Identifier
** \param   msg   Pointer to a structure of type CpStruct_CAN
** \return  Identifier value
**
** This function retrieves the identifier value of a given CAN
** message structure (11 bit identifier).
**
*/
Cp_EXPORT _U16 Cp_PREFIX CpMsgGetStdId(CpStruct_CAN * msg);


/*!
** \brief   Set Standard Identifier
** \param   msg   Pointer to a structure of type CpStruct_CAN
** \param   id    identifier value
** \param   rtr   RTR flag
**
** This function sets the identifier value of a given CAN
** message structure. If the parameter 'rtr' is TRUE, the
** RTR bit in the CAN message is set.
**
*/
Cp_EXPORT void Cp_PREFIX CpMsgSetStdId(CpStruct_CAN * msg, _U16 id, _BIT rtr);


/*!
** \brief   Get Extended Identifier
** \param   msg   Pointer to a structure of type CpStruct_CAN
** \return  Identifier value
**
** This function retrieves the identifier value of a given CAN
** message structure (29 bit identifier).
**
*/
Cp_EXPORT _U32 Cp_PREFIX CpMsgGetExtId(CpStruct_CAN * msg);


/*!
** \brief   Set Extended Identifier
** \param   msg   Pointer to a structure of type CpStruct_CAN
** \param   id    identifier value
** \param   rtr   RTR flag
**
** This function sets the identifier value of a given CAN
** message structure. If the parameter 'rtr' is TRUE, the
** RTR bit in the CAN message is set.
**
*/
Cp_EXPORT void Cp_PREFIX CpMsgSetExtId(CpStruct_CAN * msg, _U32 id, _BIT rtr);


/*!
** \brief   Get Data Length Code
** \param   msg   Pointer to a structure of type CpStruct_CAN
** \return  Data Length Code
**
** This function retrieves the data length code (DLC) of a given 
** CAN message structure.
**
*/
Cp_EXPORT _U08 Cp_PREFIX CpMsgGetDlc(CpStruct_CAN * msg);


/*!
** \brief   Set Data Length Code
** \param   msg   Pointer to a structure of type CpStruct_CAN
** \param   dlc   Data Length Code
**
** This function sets the data length code (DLC) of a given 
** CAN message structure.
**
*/
Cp_EXPORT void Cp_PREFIX CpMsgSetDlc(CpStruct_CAN * msg, _U08 dlc);


/*!
** \brief   Get message data
** \param   msg      Pointer to a structure of type CpStruct_CAN
** \param   byteNum  zero based byte position
** \return  data value of byte 'byteNum'
**
** This function retrieves the value of a data byte from a given 
** CAN message structure. The parameter 'byteNum' is zero based.
** That means the last byte from a message with DLC = 8 is 
** accessed with a byteNum value of 7.
**
*/
Cp_EXPORT _U08 Cp_PREFIX CpMsgGetData(CpStruct_CAN * msg, _U08 byteNum);


/*!
** \brief   Set message data
** \param   msg      Pointer to a structure of type CpStruct_CAN
** \param   byteNum  zero based byte position
** \param   byteVal  value of byte 'byteNum'
**
** This function sets the value of a data byte from a given 
** CAN message structure. The parameter 'byteNum' is zero based.
** That means the last byte from a message with DLC = 8 is 
** accessed with a byteNum value of 7.
**
*/
Cp_EXPORT void Cp_PREFIX CpMsgSetData(CpStruct_CAN * msg, _U08 byteNum, _U08 byteVal);


/*!
** \brief   Clear message structure
** \param   msg      Pointer to a structure of type CpStruct_CAN
**
** This function clears all members of a given CAN message structure. 
** It is intended for initialisation of a message.
**
** \b Example
** \code
** CpStruct_CAN   myMessage;
** CpMsgClear(&myMessage);             // clear the message
** CpMsgSetStdId(&myMessage, 100, 0);  // identifier is 100 dec, no RTR
** CpMsgSetDlc(&myMessage, 2);         // data length code is 2
** \endcode
**
*/
Cp_EXPORT void Cp_PREFIX CpMsgClear(CpStruct_CAN * msg);

#endif   /* _CANpie_MessageSupport_ */

