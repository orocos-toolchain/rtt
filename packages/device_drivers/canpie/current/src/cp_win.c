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


//-------------------------------------------------------------------
// this definition is necessary to include the function pointers
// which are declared in the "cp_win.h" file
#define  _CANpie_Windows_DLL_Support_Functions_    1


#include <windows.h>

#include "compiler.h"
#include "cp_win.h"


HINSTANCE   hCANpieDLL = 0L;


//----------------------------------------------------------------------------//
// CpWinOpen()                                                                //
// open the CANpie-DLL for Windows, parameter library is the name of          //
// the DLL                                                                    //
//----------------------------------------------------------------------------//
_U08  CpWinOpen(char * library)
{
   _U08  ubErrCodeT;


   if(library == NULL) hCANpieDLL = LoadLibrary("canpie.dll");
   else hCANpieDLL = LoadLibrary(library);
   if(hCANpieDLL == 0L) return(1);


   //----------------------------------------------------------------//
   // CpUser functions                                               //
   //----------------------------------------------------------------//

   CpUserAppInit     = (_U08 (*)(_U08, _U16, _U16, _U16)) GetProcAddress(hCANpieDLL, "CpUserAppInit");
   if(CpUserAppInit == 0L)       { ubErrCodeT = 10; goto ProcessError;}

   CpUserAppDeInit   = (_U08 (*)(_U08))   GetProcAddress(hCANpieDLL, "CpUserAppDeInit");
   if(CpUserAppDeInit == 0L)     { ubErrCodeT = 11; goto ProcessError;}

   CpUserBaudrate    = (_U08 (*)(_U08, _U08)) GetProcAddress(hCANpieDLL, "CpUserBaudrate");
   if(CpUserBaudrate == 0L)      { ubErrCodeT = 12; goto ProcessError;}

   CpUserFifoClear   = (_U08 (*)(_U08, _U08)) GetProcAddress(hCANpieDLL, "CpUserFifoClear");
   if(CpUserFifoClear == 0L)     { ubErrCodeT = 13; goto ProcessError;}

   CpUserFilterAll   = (_U08 (*)(_U08, _BIT)) GetProcAddress(hCANpieDLL, "CpUserFilterAll");
   if(CpUserFilterAll == 0L)     { ubErrCodeT = 14; goto ProcessError;}

   CpUserFilterMsg   = (_U08 (*)(_U08, _U16, _BIT))GetProcAddress(hCANpieDLL, "CpUserFilterMsg");
   if(CpUserFilterMsg == 0L)     { ubErrCodeT = 15; goto ProcessError;}

   CpUserIntFunctions= (_U08 (*)(_U08,_U08 (*) (CpStruct_CAN *),
                                 _U08 (*)  (CpStruct_CAN *),
                                 _U08 (*) (_U08) )) GetProcAddress(hCANpieDLL, "CpUserIntFunctions");
   if(CpUserIntFunctions == 0L)  { ubErrCodeT = 16; goto ProcessError;}

   CpUserMsgRead     = (_U08 (*)(_U08, CpStruct_CAN *)) GetProcAddress(hCANpieDLL, "CpUserMsgRead");
   if(CpUserMsgRead == 0L)       { ubErrCodeT = 17; goto ProcessError;}

   CpUserMsgWrite    = (_U08 (*)(_U08, CpStruct_CAN *)) GetProcAddress(hCANpieDLL, "CpUserMsgWrite");
   if(CpUserMsgWrite == 0L)      { ubErrCodeT = 18; goto ProcessError;}


   //----------------------------------------------------------------//
   // CpMsg functions                                                //
   //----------------------------------------------------------------//
   CpMsgGetStdId = (_U16 (*)(CpStruct_CAN *))   GetProcAddress(hCANpieDLL, 
                                                               "CpMsgGetStdId");
   if(CpMsgGetStdId == 0L)       { ubErrCodeT = 20; goto ProcessError;}


   CpMsgSetStdId = (void (*)(CpStruct_CAN *, _U16, _BIT)) GetProcAddress(hCANpieDLL, 
                                                                        "CpMsgSetStdId");
   if(CpMsgSetStdId == 0L)       { ubErrCodeT = 21; goto ProcessError;}


   CpMsgGetExtId = (_U32 (*)(CpStruct_CAN *))   GetProcAddress(hCANpieDLL, 
                                                               "CpMsgGetExtId");
   if(CpMsgGetExtId == 0L)       { ubErrCodeT = 22; goto ProcessError;}


   CpMsgSetExtId = (void (*)(CpStruct_CAN *, _U32, _BIT)) GetProcAddress(hCANpieDLL, 
                                                                        "CpMsgSetExtId");
   if(CpMsgSetExtId == 0L)       { ubErrCodeT = 23; goto ProcessError;}


   CpMsgGetDlc   = (_U08 (*)(CpStruct_CAN *))         GetProcAddress(hCANpieDLL, 
                                                               "CpMsgGetDlc");
   if(CpMsgGetDlc == 0L)         { ubErrCodeT = 24; goto ProcessError;}


   CpMsgSetDlc   = (void (*)(CpStruct_CAN *, _U08))   GetProcAddress(hCANpieDLL, 
                                                               "CpMsgSetDlc");
   if(CpMsgSetDlc == 0L)         { ubErrCodeT = 25; goto ProcessError;}


   CpMsgGetData  = (_U08 (*)(CpStruct_CAN *, _U08))   GetProcAddress(hCANpieDLL, 
                                                               "CpMsgGetData");
   if(CpMsgGetData == 0L)        { ubErrCodeT = 26; goto ProcessError;}


   CpMsgSetData  = (void (*)(CpStruct_CAN *, _U08, _U08)) GetProcAddress(hCANpieDLL, 
                                                               "CpMsgSetData");
   if(CpMsgSetData == 0L)        { ubErrCodeT = 27; goto ProcessError;}

   /*   
   CpMsgClear    = (void (*)(CpStruct_CAN *))   GetProcAddress(hCANpieDLL, 
                                                               "CpMsgClear");
   if(CpMsgClear == 0L)          { ubErrCodeT = 28; goto ProcessError;}
   */

   //----------------------------------------------------------------//
   // CpCore functions                                               //
   //----------------------------------------------------------------//
   CpCoreBaudrate = (_U08 (*)(_U08, _U08, _U08, _U08) )  GetProcAddress(hCANpieDLL, 
                                                               "CpCoreBaudrate");
   if(CpCoreBaudrate == 0L)      { ubErrCodeT = 30; goto ProcessError;}


   CpCoreHDI      = (_U08 (*)(_U08, CpStruct_HDI* )   )  GetProcAddress(hCANpieDLL, 
                                                               "CpCoreHDI");
   if(CpCoreHDI == 0L)           { ubErrCodeT = 31; goto ProcessError;}


   CpCoreCANMode  = (_U08 (*)(_U08, _U08) )  GetProcAddress(hCANpieDLL,"CpCoreCANMode");
   if(CpCoreCANMode == 0L)       { ubErrCodeT = 32; goto ProcessError;}


   CpCoreCANState = (_U08 (*)(_U08) )        GetProcAddress(hCANpieDLL,"CpCoreCANState");
   if(CpCoreCANState == 0L)      { ubErrCodeT = 33; goto ProcessError;}


   CpCoreFilterAll= (_U08 (*)(_U08, _BIT) ) GetProcAddress(hCANpieDLL, 
                                                               "CpCoreFilterAll");
   if(CpCoreFilterAll == 0L)     { ubErrCodeT = 34; goto ProcessError;}


   CpCoreFilterMsg= (_U08 (*)(_U08, _U16, _BIT) ) GetProcAddress(hCANpieDLL, 
                                                               "CpCoreFilterMsg");
   if(CpCoreFilterMsg == 0L)     { ubErrCodeT = 35; goto ProcessError;}


   CpCoreRegRead = (_U08 (*)(_U08, _U16, _U08*) ) GetProcAddress(hCANpieDLL, 
                                                               "CpCoreRegRead");
   if(CpCoreRegRead == 0L)       { ubErrCodeT = 36; goto ProcessError;}

   
   CpCoreRegWrite= (_U08 (*)(_U08, _U16, _U08) ) GetProcAddress(hCANpieDLL, 
                                                               "CpCoreRegWrite");
   if(CpCoreRegWrite == 0L)      { ubErrCodeT = 37; goto ProcessError;}

#if   CP_FULL_CAN == 1
   CpCoreAllocBuffer = (_U08 (*)(_U08, CpStruct_CAN*, _U08) )  GetProcAddress(hCANpieDLL, 
                                                               "CpCoreAllocBuffer");


   CpCoreBufferData  = (_U08* (*)(_U08, _U08) )                GetProcAddress(hCANpieDLL, 
                                                               "CpCoreBufferData");


   CpCoreBufferSend  = (_U08 (*)(_U08, _U08) )                 GetProcAddress(hCANpieDLL, 
                                                               "CpCoreBufferSend");

   CpCoreBufferTransmit = (_U08 (*)(_U08, CpStruct_CAN*) )     GetProcAddress(hCANpieDLL, 
                                                               "CpCoreBufferTransmit");

   
   CpCoreDeAllocBuffer = (_U08 (*)(_U08, _U08) )               GetProcAddress(hCANpieDLL, 
                                                               "CpCoreDeAllocBuffer");
#endif
   //------------------------------------------------------
   // no errors
   return(0);


   //------------------------------------------------------
   // error occured, unload the library and return the
   // error code
   //
ProcessError:
   FreeLibrary(hCANpieDLL);
   hCANpieDLL = NULL;
   return(ubErrCodeT);
}


//----------------------------------------------------------------------------//
// CpWinClose()                                                               //
// close the CANpie-DLL for Windows                                           //
//----------------------------------------------------------------------------//
_U08 CpWinClose(void)
{
   if(hCANpieDLL == NULL) return(0);
   FreeLibrary(hCANpieDLL);
   hCANpieDLL = NULL;
   return(0);
}