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
*  0.7      08.11.2000  additional error checking                       UK    *
*                                                                             *
\*****************************************************************************/


#include "rtt/can/cpmsg.h"


/*------------------------------------------------------------------------
** CpMsgGetStdId()
**
*/
_U16 Cp_PREFIX CpMsgGetStdId(CpStruct_CAN * msg)
{
   register _U16  theId;

   //--- check for valid pointer --------------------------
   if(msg == 0L) return (0);

   //--- mask the lower 11 bits ---------------------------
   theId = (_U16) ( (msg->v_MsgId) & CP_MASK_STD_FRAME);
   return (theId);
}


/*------------------------------------------------------------------------
** CpMsgSetStdId()
**
*/
void Cp_PREFIX CpMsgSetStdId(CpStruct_CAN * msg, _U16 id, _BIT rtr)
{
   register _U32  theId;

   //--- check for valid pointer --------------------------
   if(msg == 0L) return;

   //--- use only 11 bits ---------------------------------
   theId = (_U32) id & CP_MASK_STD_FRAME; 
   msg->v_MsgId = (_U32) id;
   if(rtr) msg->v_MsgId = msg->v_MsgId | CP_MASK_RTR_BIT;
}


/*------------------------------------------------------------------------
** CpMsgGetExtId()
**
*/
_U32 Cp_PREFIX CpMsgGetExtId(CpStruct_CAN * msg)
{
   register _U32  theId;

   //--- check for valid pointer --------------------------
   if(msg == 0L) return (0);

   //--- mask the lower 29 bits ---------------------------
   theId = (msg->v_MsgId) & CP_MASK_EXT_FRAME;
   return (theId);
}


/*------------------------------------------------------------------------
** CpMsgGetExtId()
**
*/
void Cp_PREFIX CpMsgSetExtId(CpStruct_CAN * msg, _U32 id, _BIT rtr)
{
   //--- check for valid pointer --------------------------
   if(msg == 0L) return;

   //--- use only 29 bits ---------------------------------
   id = id & CP_MASK_EXT_FRAME;
   if(rtr) id = id | CP_MASK_RTR_BIT;

   //--- set as extended frame ----------------------------
   id = id | CP_MASK_EXT_BIT;
   msg->v_MsgId = (_U32) id;
}


/*------------------------------------------------------------------------
** CpMsgGetDlc()
**
*/
_U08 Cp_PREFIX CpMsgGetDlc(CpStruct_CAN * msg)
{
   register _U08  theDlc;

   //--- check for valid pointer --------------------------
   if(msg == 0L) return (0);

   theDlc = (_U08) ( (msg->v_MsgFlags) & CP_MASK_DLC_BITS);
   return (theDlc);
}


/*------------------------------------------------------------------------
** CpMsgSetDlc()
**
*/
void Cp_PREFIX CpMsgSetDlc(CpStruct_CAN * msg, _U08 dlc)
{
   //--- check for valid pointer --------------------------
   if(msg == 0L) return;

   //--- make sure the Data Length Code is in range -------
   if(dlc > 8) dlc = 8;
   msg->v_MsgFlags = msg->v_MsgFlags & (~CP_MASK_DLC_BITS);
   msg->v_MsgFlags = msg->v_MsgFlags | dlc;
}


/*------------------------------------------------------------------------
** CpMsgGetData()
**
*/
_U08 Cp_PREFIX CpMsgGetData(CpStruct_CAN * msg, _U08 byteNum)
{
   //--- check for valid pointer --------------------------
   if(msg == 0L) return (0);

   //--- byteNum is in the range from 0 to 7 --------------
   if(byteNum > 7) return (0);

   return (msg->v_MsgData[byteNum]);
}


/*------------------------------------------------------------------------
** CpMsgSetData()
**
*/
void Cp_PREFIX CpMsgSetData(CpStruct_CAN * msg, _U08 byteNum, _U08 byteVal)
{
   //--- check for valid pointer --------------------------
   if(msg == 0L) return;

   //--- byteNum is in the range from 0 to 7 --------------
   if(byteNum > 7) return;

   msg->v_MsgData[byteNum] = byteVal;
}


/*------------------------------------------------------------------------
** CpMsgClear()
**
*/
void Cp_PREFIX CpMsgClear(CpStruct_CAN * msg) 
{
   //--- check for valid pointer --------------------------
   if(msg == 0L) return;

   //--- set identifier and flags field to zero -----------
   msg->v_MsgId    = 0;
   msg->v_MsgFlags = 0;
}

