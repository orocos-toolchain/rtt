/*****************************************************************************\
*  CANpie                                                                     *
*                                                                             *
*  File        : cpmacro.h                                                    *
*  Description : This file supplies some support macros for access of the     *
*                "CpStruct_CAN" message structure. The macros are a faster    *
*                solution compared to functions. However, there is the lack   *
*                of error checking. Please be aware of this when using these  *
*                macros.                                                      *
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
*  0.6      27.07.1999  Initial version                                 UK    *
*  0.7      09.02.2001  Fixed CpMacSetBufNum()                          UK    *
*  0.8                  - no changes -                                      	*
*                                                                             *
\*****************************************************************************/


#ifndef  _CANpie_MacroSupport_
#define  _CANpie_MacroSupport_


#include "cpconst.h"

/*-----------------------------------------------------------------------------
**    Online documentation for Doxygen
*/

/*!
** \file    cpmacro.h
** \brief   CANpie macros
**
** In order to create small and fast code, CANpie supplies a set of
** macros to access the CAN message structure (CpStruct_CAN). These
** macros can be used instead of the functions defined in the cpmsg.h
** header file. However keep in mind that macros can't be used to check
** for value ranges or parameter consistence. <p>
** \b Example
** \code
** //--- setup a CAN message ----------------------------------------
** CpStruct_CAN   myMessage;
** CpMacMsgClear(&myMessage);             // clear the message
** CpMacSetStdId(&myMessage, 100, 0);     // identifier is 100 dec, no RTR
** CpMacSetDlc(&myMessage, 2);            // data length code is 2
** CpMacSetData(&myMessage, 0, 0x11);     // byte 0 has the value 0x11
** CpMacSetData(&myMessage, 1, 0x22);     // byte 1 has the value 0x22
** //... do something with it ....
**
** //--- evaluate a message that was received -----------------------
** CpStruct_CAN   receiveMsg;
** //... receive the stuff ....
**
** if(CpMacIsExtended(&receiveMsg))
** {
**    //--- this is an Extended Frame ---------------------
**    DoExtendedMessageService();
**    return;
** }
**
** if(CpMacIsRemote(&receiveMsg))
** {
**    //... do something with RTR frames
** }
** \endcode
*/



/*!
** \def     CpMacGetBufNum(MSG_PTR)
** \brief   Get message buffer number
** \param   MSG_PTR  Pointer to a CpStruct_CAN message
**
** A FullCAN controller has the feature of message buffers. The buffer number
** is coded in the field v_MsgFlags of the structure CpStruct_CAN. With this
** macro the number of the buffer (index starts at 1) can be retrieved.
*/
#define  CpMacGetBufNum(MSG_PTR)          (_U08)(((MSG_PTR)->v_MsgFlags) >> 4)


/*!
** \def     CpMacGetData(MSG_PTR, POS)
** \brief   Get Data
** \param   MSG_PTR  Pointer to a CpStruct_CAN message
** \param   POS      Zero based index of byte position
** \see     The corresponding function CpMsgGetData()
**
** This macro retrieves the data of a CAN message. The parameter
** POS must be within the range 0 .. 7.
*/
#define  CpMacGetData(MSG_PTR, POS)       ( (MSG_PTR)->v_MsgData[POS] )


/*!
** \def     CpMacGetDlc(MSG_PTR)
** \brief   Get Data Length Code
** \param   MSG_PTR  Pointer to a CpStruct_CAN message
** \see     The corresponding function CpMsgGetDlc()
**
** This macro retrieves the data length code (DLC) of a CAN message.
*/
#define  CpMacGetDlc(MSG_PTR)             (_U08)(((MSG_PTR)->v_MsgFlags) & CP_MASK_DLC_BITS)


/*!
** \def     CpMacGetExtId(MSG_PTR)
** \brief   Get 29 Bit Identifier Value
** \param   MSG_PTR  Pointer to a CpStruct_CAN message
** \see     The corresponding function CpMsgGetExtId()
**
** This macro retrieves the value for the identifier of an
** extended frame (CAN 2.0B).
*/
#define  CpMacGetExtId(MSG_PTR)           (((MSG_PTR)->v_MsgId) & CP_MASK_EXT_FRAME)


/*!
** \def     CpMacGetStdId(MSG_PTR)
** \brief   Get 11 Bit Identifier Value
** \param   MSG_PTR  Pointer to a CpStruct_CAN message
** \see     The corresponding function CpMsgGetStdId()
**
** This macro retrieves the value for the identifier of an
** standard frame (CAN 2.0A). The value is scaled to an unsigned
** 16 bit value.
*/
#define  CpMacGetStdId(MSG_PTR)           (_U16)(((MSG_PTR)->v_MsgId) & CP_MASK_STD_FRAME)


/*!
** \def     CpMacIsExtended(MSG_PTR)
** \brief   Check the frame type
** \param   MSG_PTR  Pointer to a CpStruct_CAN message
**
** This macro checks the frame type. If the frame is CAN 2.0A
** (Standard Frame), the value 0 is returned. If the frame is
** CAN 2.0B (Extended Frame), the value 1 is returned.
*/
#define  CpMacIsExtended(MSG_PTR)         (_U08)(((MSG_PTR)->v_MsgId & CP_MASK_EXT_BIT ) != 0)


/*!
** \def     CpMacIsRemote(MSG_PTR)
** \brief   Check for remote frame
** \param   MSG_PTR  Pointer to a CpStruct_CAN message
**
** This macro checks if the Remote Transmission bit (RTR) is
** set. If the RTR bit is set, the macro will return 1, otherwise
** 0.
*/
#define  CpMacIsRemote(MSG_PTR)           (_U08)(((MSG_PTR)->v_MsgId & CP_MASK_RTR_BIT) != 0)


/*!
** \def     CpMacMsgClear(MSG_PTR)
** \brief   Clear message structure
** \param   MSG_PTR  Pointer to a CpStruct_CAN message
** \see     The corresponding function CpMsgClear()
**
** This macro sets the identifier field and the flags field
** of a CAN message structure to 0.
*/
#define  CpMacMsgClear(MSG_PTR)           (MSG_PTR)->v_MsgId= 0;(MSG_PTR)->v_MsgFlags = 0


/*!
** \def     CpMacSetBufNum(MSG_PTR)
** \brief   Set message buffer number
** \param   MSG_PTR  Pointer to a CpStruct_CAN message
**
** A FullCAN controller has the feature of message buffers. The buffer number
** is coded in the field v_MsgFlags of the structure CpStruct_CAM. With this
** macro the number of the buffer (index starts at 1) can be set to a certain
** value.
*/
#define  CpMacSetBufNum(MSG_PTR, VAL)     (MSG_PTR)->v_MsgFlags &= (~CP_MASK_BUF_BITS); (MSG_PTR)->v_MsgFlags |= (VAL << 4)


/*!
** \def     CpMacSetData(MSG_PTR, POS, VAL)
** \brief   Set Data
** \param   MSG_PTR  Pointer to a CpStruct_CAN message
** \param   POS      Zero based index of byte position
** \param   VAL      Value of data byte in CAN message
** \see     The corresponding function CpMsgSetData()
**
** This macro sets the data in a CAN message. The parameter
** POS must be within the range 0 .. 7, since there is no
** error checking.
*/
#define  CpMacSetData(MSG_PTR, POS, VAL)  (MSG_PTR)->v_MsgData[POS] = VAL


/*!
** \def     CpMacSetDlc(MSG_PTR)
** \brief   Set Data Length Code
** \param   MSG_PTR  Pointer to a CpStruct_CAN message
** \param   DLC      Data length code
** \see     The corresponding function CpMsgSetDlc()
**
** This macro sets the data length code (DLC) of a CAN message.
** The parameter DLC must be within the range from 0..8. No error
** checking is provided.
*/
#define  CpMacSetDlc(MSG_PTR, DLC)        (MSG_PTR)->v_MsgFlags &= (~CP_MASK_DLC_BITS); (MSG_PTR)->v_MsgFlags |= DLC


/*!
** \def     CpMacSetExtId(MSG_PTR)
** \brief   Set 29 Bit Identifier Value
** \param   MSG_PTR  Pointer to a CpStruct_CAN message
** \param   VAL      Identifier value
** \see     The corresponding function CpMsgSetExtId()
**
** This macro sets the identifer value for an
** extended frame (CAN 2.0B).
*/
#define  CpMacSetExtId(MSG_PTR, VAL)      (MSG_PTR)->v_MsgId = VAL | CP_MASK_EXT_BIT


/*!
** \def     CpMacSetRemote(MSG_PTR)
** \brief   Set RTR bit
** \param   MSG_PTR  Pointer to a CpStruct_CAN message
**
** This macro sets the Remote Transmission bit (RTR).
*/
#define  CpMacSetRemote(MSG_PTR)          (MSG_PTR)->v_MsgId |= CP_MASK_RTR_BIT


/*!
** \def     CpMacSetStdId(MSG_PTR)
** \brief   Set 11 Bit Identifier Value
** \param   MSG_PTR  Pointer to a CpStruct_CAN message
** \param   VAL      Identifier value
** \see     The corresponding function CpMsgSetExtId()
**
** This macro sets the identifer value for an
** standard frame (CAN 2.0A).
*/
#define  CpMacSetStdId(MSG_PTR, VAL)      (MSG_PTR)->v_MsgId = VAL



#endif   /* _CANpie_MacroSupport_ */

