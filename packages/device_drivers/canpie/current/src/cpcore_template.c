/*****************************************************************************\
*  CANpie                                                                     *
*                                                                             *
*  File        : cpcore.c                                                     *
*  Description : CANpie core functions                                        *
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
*  0.3      04.09.1999  Adaption to version 0.3 of the CANpie Doc.      UK    *
*  0.4      18.01.2000  Adaption to Keil C compiler                     UK    *
*  0.5                  - no changes -                                        *
*  0.6      15.06.2000  set to CANpie release 0.6                       UK    *
*  0.7      02.11.2000  new functions for buffer operations             UK    *
*  0.8      15.02.2001  set to CANpie release 0.8                       UK   	*
*                                                                             *
\*****************************************************************************/

#include "cpconfig.h"
#include "cpcore.h"
#include "cpfifo.h"
#include "cpfilter.h"
#include "cpmacro.h"


/*-------------------------------------------------------------------
** Global variables used for the core functions
**
*/
static _U08    CpVar_CAN_Status[CP_CHANNEL_MAX];

_U08 (* CpInt_ReceiveHandler[CP_CHANNEL_MAX] )(CpStruct_CAN *);
_U08 (* CpInt_TransmitHandler[CP_CHANNEL_MAX])(CpStruct_CAN *);
_U08 (* CpInt_ErrorHandler[CP_CHANNEL_MAX]   )(_U08);

//-------------------------------------------------------------------
// the ´CpVar_InitFlag´ is used to keep track if a channel
// is already initialized,
// Bit 0 = channel 0
// Bit 1 = channel 1
// ....
// Bit 15 = channel 15
// a bit value of 0 means not initialized
// a bit value of 1 means initialized
_U16 CpVar_InitFlag = 0;




/*-------------------------------------------------------------------
** CpStruct_BitTimingTable
** The bit timing structure has to be fitted to the used CAN 
** controller. If more than one CAN channel is used the structure 
** has to be extended.
*/
const CpStruct_BitTimingValue CpStruct_BitTimingTable[] = {
   { 0x00, 0x00, 0x00 },   /*  10 KBit/sec   */
   { 0x01, 0x01, 0x01 },   /*  20 KBit/sec   */
   { 0x02, 0x02, 0x02 },   /*  50 KBit/sec   */
   { 0x03, 0x03, 0x03 },   /* 100 KBit/sec   */
   { 0x04, 0x04, 0x04 },   /* 125 KBit/sec   */
   { 0x05, 0x05, 0x05 },   /* 250 KBit/sec   */
   { 0x06, 0x06, 0x06 },   /* 500 KBit/sec   */
   { 0x07, 0x07, 0x07 },   /* 800 KBit/sec   */
   { 0x08, 0x08, 0x08 }    /*   1 MBit/sec   */
};


//----------------------------------------------------------------------------//
// CpCoreAllocBuffer()                                                        //
// Allocate buffer of FullCAN controller                                      //
//----------------------------------------------------------------------------//
#if   CP_FULL_CAN == 1
_U08 Cp_PREFIX CpCoreAllocBuffer (_U08 ubChannelV, CpStruct_CAN * pCanMsgV,
                                  _U08 ubDirectionV)
{
#if   CP_SMALL_CODE == 0
   //---	test the channel number ------------------------------------
	if( ubChannelV >= CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   return (CpErr_OK);
}
#endif


//----------------------------------------------------------------------------//
// CpCoreBaudrate()                                                           //
// Setup baudrate of CAN controller                                           //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreBaudrate(_U08 ubChannelV, 
                              _U08 ubBtr0V, _U08 ubBtr1V, _U08 ubSjwV)
{
#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if( ubChannelV >= CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferDLC()                                                     		//
// Change DLC of buffer                                                			//
//----------------------------------------------------------------------------//
#if   CP_FULL_CAN == 1
Cp_EXPORT _U08 Cp_PREFIX CpCoreBufferDLC(	_U08 ubChannelV,
														_U08 ubBufferV, _U08 ubDlcV)
{
#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if( ubChannelV >= CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   return (CpErr_OK);
}
#endif


//----------------------------------------------------------------------------//
// CpCoreBufferData()                                                     		//
// Get Pointer to data fields                                          			//
//----------------------------------------------------------------------------//
#if   CP_FULL_CAN == 1
Cp_EXPORT _U08 * Cp_PREFIX CpCoreBufferData(_U08 ubChannelV, _U08 ubBufferV)
{
	_U08 *	pubDataT;	// pointer to message data field
	
	//----------------------------------------------------------------
	//	on failure a NULL pointer is returned
	//
	pubDataT = 0L;
	
#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if( ubChannelV >= CP_CHANNEL_MAX) return (pubDataT);
#endif

   return (pubDataT);
}
#endif


//----------------------------------------------------------------------------//
// CpCoreBufferSend()                                                     		//
// Send message of FullCAN buffer                                      			//
//----------------------------------------------------------------------------//
#if   CP_FULL_CAN == 1
Cp_EXPORT _U08 Cp_PREFIX   CpCoreBufferSend(_U08 ubChannelV, _U08 ubBufferV)
{
#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if( ubChannelV >= CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   return (CpErr_OK);
}
#endif


//----------------------------------------------------------------------------//
// CpCoreBufferTransmit()                                                     //
// Transmit contents of buffer                                                //
//----------------------------------------------------------------------------//
#if   CP_FULL_CAN == 1
_U08 Cp_PREFIX CpCoreBufferTransmit(_U08 ubChannelV, 
                                    CpStruct_CAN * pCanMsgV)
{
#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if( (ubChannelV + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   return (CpErr_OK);
}
#endif


//----------------------------------------------------------------------------//
// CpCoreCANMode()                                                            //
// Set state of CAN controller                                                //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX    CpCoreCANMode(_U08 ubChannelV, _U08 ubModeV)
{
#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if (ubChannelV >= CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

#if   CP_CHANNEL_MAX == 1
   return (CpVar_CAN_Status[0]);
#else
   return (CpVar_CAN_Status[ubChannelV]);
#endif
}


//----------------------------------------------------------------------------//
// CpCoreCANState()                                                           //
// Retrive state of CAN controller                                            //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX    CpCoreCANState(_U08 ubChannelV)
{
	_U08	ubCurrentStateT;
	
#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if (ubChannelV >= CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   //----------------------------------------------------------------
   // TODO:
   // place the state of the CAN controller into the variable
   // ubCurrentState. Possible values are in the enumeration
   // CP_STATE.
   //
	ubCurrentStateT = CP_STATE_ACTIVE;
	
   // end of TODO
   //----------------------------------------------------------------

   return (ubCurrentStateT);
}


//----------------------------------------------------------------------------//
// CpCoreDeAllocBuffer()                                                      //
// De-Allocate a FullCAN buffer                                               //
//----------------------------------------------------------------------------//
#if   CP_FULL_CAN == 1
_U08 Cp_PREFIX CpCoreDeAllocBuffer (_U08 ubChannelV, _U08 ubBufferNumV)
{
#if   CP_SMALL_CODE == 0
   //---	test the channel number ------------------------------------
	if( (ubChannelV + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

	return (CpErr_OK);
}
#endif


//----------------------------------------------------------------------------//
// CpCoreDeInitDriver()                                                       //
// De-Initialize the CAN driver                                               //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreDeInitDriver(_U08 ubChannelV)
{
#if   CP_SMALL_CODE == 0
   //---	test the channel number ------------------------------------
	if( (ubChannelV + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreInitDriver()                                                         //
// Initialize the CAN driver                                                  //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreInitDriver(_U08 ubChannelV)
{
#if   CP_SMALL_CODE == 0
   //---	test the channel number ------------------------------------
	if( (ubChannelV + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreIntHandler()                                                         //
// Interrupt Handler of CANpie                                                //
//----------------------------------------------------------------------------//
void CpCoreIntHandler(void)
{


}


//----------------------------------------------------------------------------//
// CpCoreHDI()                                                                //
// Fill Hardware Description Interface (HDI) structure                        //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreHDI(_U08 ubChannelV, CpStruct_HDI * pHdiV)

{
#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if( (ubChannelV + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif


   //----------------------------------------------------------------
   // set the support flags according to the configuration
   // options
   // THIS IS ONLY A SAMPLE!
   pHdiV->v_SupportFlags  = 0;  // frame type 2.0A

#if   CP_FULL_CAN == 1
   pHdiV->v_SupportFlags |= 0x04;
#endif

#if   CP_INT_HANDLER == 1
   pHdiV->v_SupportFlags |= 0x08;
#endif

#if   CP_SW_FILTER != 0
   pHdiV->v_SupportFlags |= 0x10;
#endif

#if   CP_MSG_TIME == 1
   pHdiV->v_SupportFlags |= 0x20;
#endif

#if   CP_USR_DATA == 1
   pHdiV->v_SupportFlags |= 0x40;
#endif

   //----------------------------------------------------------------
   // insert the controller type here
   //
   pHdiV->v_ControllerType = 0;


   //----------------------------------------------------------------
   // insert the IRQ number (if applicable) here
   //
   pHdiV->v_IRQNumber = 0;

   
   //----------------------------------------------------------------
   // CANpie version major and minor are automatically set
   //
   pHdiV->v_VersionMajor  = CP_VERSION_MAJOR;
   pHdiV->v_VersionMinor  = CP_VERSION_MINOR;


   //----------------------------------------------------------------
   // insert the driver name here
   //
   pHdiV->v_DriverName    = "My little dummy driver";


   //----------------------------------------------------------------
   // insert the driver version number here
   //
   pHdiV->v_VersionNumber = "Firmware 0.77";

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreFilterAll()                                                          //
// Enable / Disable filter for all messages                                   //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreFilterAll(_U08 ubChannelV, _BIT btEnableV)
{
#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if( (ubChannelV + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif


   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreFilterMsg()                                                          //
// Enable / Disable filter for a single messages                              //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreFilterMsg(_U08 ubChannelV, _U16 uwIdV, _BIT btEnableV)
{
#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if( ubChannelV >= CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif


   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreMsgReceive()                                                         //
// Read CAN message from controller                               				//
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreMsgReceive(_U08 ubChannelV)
{
   _U08           ubErrCodeT;  	// error code value
   CpStruct_CAN   canMsgT;       // CAN message structure


#if   CP_SMALL_CODE == 0
   //--- check if channel is valid ----------------------------------
   if (ubChannelV >= CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif


   //----------------------------------------------------------------
   // TODO:
   // fill the structure canMsgT with the contents of the CAN
   // controller buffer. Find below an example how to fill the
   // structure.
   //
   CpMacSetStdId(&canMsgT, 100);
   CpMacSetDlc(&canMsgT, 2);
   CpMacSetData(&canMsgT, 0, 0x11);
   CpMacSetData(&canMsgT, 1, 0x22);

   // end of TODO
   //----------------------------------------------------------------


   //----------------------------------------------------------------
   //	test the received message against the software ID filter
   //
#if   CP_SW_FILTER == 1
   ubErrCodeT = CpFilterTest(ubChannelV, CpMacGetStdId(&canMsgT) );
   if (ubErrCodeT) return (ubErrCodeT);
#endif
   
   //----------------------------------------------------------------
   //	check for receive handler
   //
   if(CpInt_ReceiveHandler[ubChannelV])
   {
      ubErrCodeT = (* CpInt_ReceiveHandler[ubChannelV] )(&canMsgT);
      if(ubErrCodeT == CP_CALLBACK_PROCESSED) return (CpErr_OK);
   }


   //----------------------------------------------------------------
   //	put message to FIFO
   //
   ubErrCodeT = CpFifoPush(ubChannelV, CP_FIFO_RCV, &canMsgT);
   return (ubErrCodeT);
}


//----------------------------------------------------------------------------//
// CpCoreMsgTransmit()                                                        //
// Write CAN message to controller                               					//
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreMsgTransmit(_U08 ubChannelV)
{
#if   CP_SMALL_CODE == 0
   //--- check if channel is valid ----------------------------------
   if (ubChannelV >= CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreRegRead()                                                            //
// Read one byte from specified CAN controller register                       //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreRegRead(_U08 ubChannelV, _U16 uwRegAdrV, _U08 * pubValueV)
{
#if   CP_SMALL_CODE == 0
   //--- check if channel is valid ----------------------------------
   if (ubChannelV >= CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   uwRegAdrV = uwRegAdrV + CP_CAN_BASE_ADR;

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreRegWrite()                                                           //
// Write one byte to specified CAN controller register                        //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreRegWrite(_U08 ubChannelV, _U16 uwRegAdrV, _U08 ubValueV)
{
#if   CP_SMALL_CODE == 0
   //--- check if channel is valid ----------------------------------
   if (ubChannelV >= CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   uwRegAdrV = uwRegAdrV + CP_CAN_BASE_ADR;

   return (CpErr_OK);
}

