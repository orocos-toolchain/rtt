/*****************************************************************************\
*  CANpie                                                                     *
*                                                                             *
*  File        : cp_win.h                                                     *
*  Description : This file imports the CANpie functions from a DLL to the     *
*                application program.                                         *
*                Call the function CpWinOpen() within your application        *
*                program first and supply the name of the DLL as parameter.   *
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
*  0.6      15.06.2000  Initial version                                 UK    *
*  0.7      07.11.2000  Added several core functions                    UK    *
*                                                                             *
\*****************************************************************************/


#ifndef  _CANpie_Windows_DLL_Support_
#define  _CANpie_Windows_DLL_Support_

#include "cpconst.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef  _CANpie_Win_Functions_Included_
//----------------------------------------------------------------------------//
// CpUser functions                                                           //
//----------------------------------------------------------------------------//
_U08 (* CpUserAppInit)     (_U08, _U16, _U16, _U16);
_U08 (* CpUserAppDeInit)   (_U08 channel);
_U08 (* CpUserBaudrate)    (_U08 channel, _U08 baud);
_U08 (* CpUserFifoClear)   (_U08 channel, _U08 buffer);
_U08 (* CpUserFilterAll)   (_U08 channel, _BIT enable);
_U08 (* CpUserFilterMsg)   (_U08 channel, _U16 id, _BIT enable);

_U08 (* CpUserIntFunctions)(_U08 channel,
                            _U08 (* rx_handler)  (CpStruct_CAN *),
                            _U08 (* tx_handler)  (CpStruct_CAN *),
                            _U08 (* err_handler) (_U08) );

_U08 (* CpUserMsgRead)     (_U08 channel, CpStruct_CAN * msgPtr);
_U08 (* CpUserMsgWrite)    (_U08 channel, CpStruct_CAN * msgPtr);


//----------------------------------------------------------------------------//
// CpMsg functions                                                            //
//----------------------------------------------------------------------------//
_U16 (* CpMsgGetStdId)     (CpStruct_CAN * msg);
void (* CpMsgSetStdId)     (CpStruct_CAN * msg, _U16 id, _BIT rtr);

_U32 (* CpMsgGetExtId)     (CpStruct_CAN * msg);
void (* CpMsgSetExtId)     (CpStruct_CAN * msg, _U32 id, _BIT rtr);

_U08 (* CpMsgGetDlc)       (CpStruct_CAN * msg);
void (* CpMsgSetDlc)       (CpStruct_CAN * msg, _U08 dlc);

_U08 (* CpMsgGetData)      (CpStruct_CAN * msg, _U08 byteNum);
void (* CpMsgSetData)      (CpStruct_CAN * msg, _U08 byteNum, _U08 byteVal);

void (* CpMsgClear)        (CpStruct_CAN * msg);


//----------------------------------------------------------------------------//
// CpCore functions                                                           //
//----------------------------------------------------------------------------//
_U08 (* CpCoreBaudrate)    (_U08 channel, _U08 btr0, _U08 btr1, _U08 sjw);
_U08 (* CpCoreCANMode)     (_U08 channel, _U08 mode);
_U08 (* CpCoreCANState)    (_U08 channel);

_U08 (* CpCoreHDI)         (_U08 channel, CpStruct_HDI* hdi);

_U08 (* CpCoreFilterAll)   (_U08 channel, _BIT enable);
_U08 (* CpCoreFilterMsg)   (_U08 channel, _U16 id, _BIT enable);

_U08 (* CpCoreRegRead)     (_U08 channel, _U16 regAdr, _U08 * valuePtr);
_U08 (* CpCoreRegWrite)    (_U08 channel, _U16 regAdr, _U08 value);

#if   CP_FULL_CAN == 1
_U08 (* CpCoreAllocBuffer)    (_U08 channel, CpStruct_CAN * msg, _U08 direction);
_U08* (* CpCoreBufferData)    (_U08 channel, _U08 ubBufferV);
_U08 (* CpCoreBufferSend)     (_U08 channel, _U08 ubBufferV);
_U08 (* CpCoreBufferTransmit) (_U08 channel, CpStruct_CAN * msg);
_U08 (* CpCoreDeAllocBuffer)  (_U08 channel, _U08 msg_buffer);
#endif

//----------------------------------------------------------------------------//
// CpWinOpen()                                                                //
// call this function with the name of the library/driver in your application //
//----------------------------------------------------------------------------//
_U08 CpWinOpen(char * library);


//----------------------------------------------------------------------------//
// CpWinClose()                                                               //
// call this function to close the driver in your application                 //
//----------------------------------------------------------------------------//
_U08 CpWinClose(void);



//----------------------------------------------------------------------------//
// external function definitions                                              //
//----------------------------------------------------------------------------//
#else
extern _U08 (* CpUserAppInit)     (_U08, _U16, _U16, _U16);
extern _U08 (* CpUserAppDeInit)   (_U08 channel);

extern _U08 (* CpUserBaudrate)    (_U08 channel, _U08 baud);
extern _U08 (* CpUserFifoClear)   (_U08 channel, _U08 buffer);
extern _U08 (* CpUserFilterAll)   (_U08 channel, _BIT enable);
extern _U08 (* CpUserFilterMsg)   (_U08 channel, _U16 id, _BIT enable);

extern _U08 (* CpUserIntFunctions)(_U08 channel,
                            _U08 (* rx_handler)  (CpStruct_CAN *),
                            _U08 (* tx_handler)  (CpStruct_CAN *),
                            _U08 (* err_handler) (_U08) );

extern _U08 (* CpUserMsgRead)     (_U08 channel, CpStruct_CAN * msgPtr);
extern _U08 (* CpUserMsgWrite)    (_U08 channel, CpStruct_CAN * msgPtr);


//----------------------------------------------------------------------------//
// CpMsg functions                                                            //
//----------------------------------------------------------------------------//
extern _U16 (* CpMsgGetStdId)    (CpStruct_CAN * msg);
extern void (* CpMsgSetStdId)    (CpStruct_CAN * msg, _U16 id, _BIT rtr);

extern _U32 (* CpMsgGetExtId)    (CpStruct_CAN * msg);
extern void (* CpMsgSetExtId)    (CpStruct_CAN * msg, _U32 id, _BIT rtr);

extern _U08 (* CpMsgGetDlc)      (CpStruct_CAN * msg);
extern void (* CpMsgSetDlc)      (CpStruct_CAN * msg, _U08 dlc);

extern _U08 (* CpMsgGetData)     (CpStruct_CAN * msg, _U08 byteNum);
extern void (* CpMsgSetData)     (CpStruct_CAN * msg, _U08 byteNum, _U08 byteVal);

extern void (* CpMsgClear)       (CpStruct_CAN * msg);


//----------------------------------------------------------------------------//
// CpCore functions                                                           //
//----------------------------------------------------------------------------//
extern _U08 (* CpCoreBaudrate)    (_U08 channel, _U08 btr0, _U08 btr1, _U08 sjw);
extern _U08 (* CpCoreCANMode)     (_U08 channel, _U08 mode);
extern _U08 (* CpCoreCANState)    (_U08 channel);

extern _U08 (* CpCoreHDI)         (_U08 channel, CpStruct_HDI* hdi);

extern _U08 (* CpCoreFilterAll)   (_U08 channel, _BIT enable);
extern _U08 (* CpCoreFilterMsg)   (_U08 channel, _U16 id, _BIT enable);

extern _U08 (* CpCoreRegRead)     (_U08 channel, _U16 regAdr, _U08 * valuePtr);
extern _U08 (* CpCoreRegWrite)    (_U08 channel, _U16 regAdr, _U08 value);

#if   CP_FULL_CAN == 1
extern _U08 (* CpCoreAllocBuffer)    (_U08 channel, CpStruct_CAN * msg, _U08 direction);
extern _U08* (* CpCoreBufferData)    (_U08 channel, _U08 ubBufferV);
extern _U08 (* CpCoreBufferSend)     (_U08 channel, _U08 ubBufferV);
extern _U08 (* CpCoreBufferTransmit) (_U08 channel, CpStruct_CAN * msg);
extern _U08 (* CpCoreDeAllocBuffer)  (_U08 channel, _U08 msg_buffer);
#endif


#endif   // external functions end --------



#ifdef __cplusplus
}
#endif


#endif