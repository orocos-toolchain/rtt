/*****************************************************************************\
*  CANpie                                                                     *
*                                                                             *
*  File        : cp_sja1000.c                                                 *
*  Description : Core functions for SJA1000 with ATMEL ATMEGA                 *
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
*  1.0      02.11.2000  Initial version, adopted from cpcore.c,         uk    *
*                       access to SJA1000 in BasicCAN mode                    *
*                                                                             *
\*****************************************************************************/

#include "cpconfig.h"
#include "cpcore.h"
#include "cpfifo.h"
#include "cpfilter.h"
#include "cpmacro.h"

#include "cp_sja1000.h"

#include <iom161.h>



#define        BUFFER_SIZE_MAX   8
#define        BUFFER_CONFIG     0x80000000
#define        BUFFER_DIR_TX     0x40000000
#define        BUFFER_TX_REQ     0x10000000


#pragma interrupt_handler CpCoreIntHandler:2

/*------------------------------------------------------------------------
** Global variables used for the core functions
**
*/
static _U08    CpVar_CAN_Status[CP_CHANNEL_MAX];

_U32 (* CpInt_ReceiveHandler[CP_CHANNEL_MAX] )(_TsCpCanMsg *);
_U32 (* CpInt_TransmitHandler[CP_CHANNEL_MAX])(_TsCpCanMsg *);
_U32 (* CpInt_ErrorHandler[CP_CHANNEL_MAX]   )(_U32);

_TsCpCanMsg    tsCpMsgBufferTableG[BUFFER_SIZE_MAX];


//-----------------------------------------------------------------------------
// the ´CpVar_InitFlag´ is used to keep track if a channel
// is already initialized,
// Bit 0 = channel 0
// Bit 1 = channel 1
// ....
// Bit 15 = channel 15
// a bit value of 0 means not initialized
// a bit value of 1 means initialized
_U16 CpVar_InitFlag = 0;


//-----------------------------------------------------------------------------
// the variable ´CpVarInterruptStatusG´ is used to keep track what kind of
// interrupt is set on the SJA1000 (receive / transmit / error)
//
_U08  CpVarInterruptStatusG;


/*------------------------------------------------------------------------
** CpStruct_BitTimingTable
** The bit timing structure has to be fitted to the used CAN controller.
** If more than one CAN channel is used the structure has to be extended.
*/
const CpStruct_BitTimingValue CpStruct_BitTimingTable[] = {

   { 0x67, 0x2F, 0x00 },   /*  10 KBit/sec   */
   { 0x53, 0x2F, 0x01 },   /*  20 KBit/sec   */
   { 0x47, 0x2F, 0x02 },   /*  50 KBit/sec   */
   { 0x43, 0x2F, 0x03 },   /* 100 KBit/sec   */
   { 0x03, 0x1C, 0x04 },   /* 125 KBit/sec   */
   { 0x01, 0x1C, 0x05 },   /* 250 KBit/sec   */
   { 0x00, 0x1C, 0x06 },   /* 500 KBit/sec   */
   { 0x00, 0x12, 0x07 },   /* 800 KBit/sec   */
   { 0x00, 0x14, 0x08 }    /*   1 MBit/sec   */
};


//----------------------------------------------------------------------------//
// CpCoreAllocBuffer()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
_U08 CpCoreAllocBuffer(_U08 ubChannelV, _TsCpCanMsg * ptsCanMsgV, _U08 ubDirectionV)
{
   _TsCpCanMsg *  pBufferT;      // pointer to buffer
   _U08           ubBufNumT;     // buffer number
   _U08           ubDataCntT;    // data count

   //----------------------------------------------------------------
   // the buffer number inside the message structure
   // is in the range from 1 .. 15 (CP_BUFFER_15)
   // access to structure starts at index 0
   ubBufNumT = CpMacGetBufNum(ptsCanMsgV);

   //--- check for valid buffer number ------------------------------
   if(ubBufNumT < CP_BUFFER_1 ) return(CpErr_GENERIC);
   if(ubBufNumT > CP_BUFFER_14) return(CpErr_GENERIC);
   ubBufNumT = ubBufNumT - 1;

   //--- get pointer to buffer and setup the data -------------------
   pBufferT = &(tsCpMsgBufferTableG[ubBufNumT]);
   pBufferT->v_MsgId    = ptsCanMsgV->v_MsgId;
   pBufferT->v_MsgFlags = ptsCanMsgV->v_MsgFlags;
   for(ubDataCntT = 0; ubDataCntT < CpMacGetDlc(ptsCanMsgV); ubDataCntT++) 
   {
      pBufferT->v_MsgData[ubDataCntT] = ptsCanMsgV->v_MsgData[ubDataCntT];
   }

   //----------------------------------------------------------------
   // mark buffer as configured by setting highest bit in user data
   pBufferT->v_MsgFlags |= BUFFER_CONFIG;
   if(ubDirectionV == CP_BUFFER_DIR_TX)
   {
      pBufferT->v_MsgFlags |= BUFFER_DIR_TX;
   }

   return(CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBaudrate()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
_U08 CpCoreBaudrate(_U08 ubChannelV, _U08 ubBtr0V, _U08 ubBtr1V, _U08 ubBaudSelV)
{
   _U08  ubCanBtr0T;
   _U08  ubCanBtr1T;

   //----------------------------------------------------------------
   // setup the values for btr0 and btr1
   //
   if(ubBaudSelV == CP_BAUD_BTR)
   {
      ubCanBtr0T = ubBtr0V;
      ubCanBtr1T = ubBtr1V;
   }
   else
   {
      //--- take values from table -----------------------------
      ubCanBtr0T = CpStruct_BitTimingTable[ubBaudSelV].btr0;
      ubCanBtr1T = CpStruct_BitTimingTable[ubBaudSelV].btr1;
   }

   //---------------------------------------------------------------#
   // insert hardware dependent code here
   //

   //--- switch to reset mode ---------------------------------------
   while ( !(CpMacRegRead(eREG_CONTROL) & CR_RR_BIT) ) 
   {
      CpMacRegWrite(eREG_CONTROL, CR_RR_BIT);
   }


   //---set timing bit ----------------------------------------------
   CpMacRegWrite(eREG_TIMING0, ubCanBtr0T);
   CpMacRegWrite(eREG_TIMING1, ubCanBtr1T);
   

   //--- switch back to operating mode and enable interrupts --------
   while ( CpMacRegRead(eREG_CONTROL) & CR_RR_BIT ) 
   {
      CpMacRegWrite(eREG_CONTROL, CpVarInterruptStatusG);
   }

   //
   // end of hardware dependent code
   //---------------------------------------------------------------#

   
   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferGetData()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
_U08 CpCoreBufferGetData(_U08 ubChannelV, _U08 ubBufferV, _U08 * pubDataV)
{
   _U08           ubDataCntT;
   _TsCpCanMsg *  pBufferT;      // pointer to buffer

   //--- check for valid buffer number ------------------------------
   if(ubBufferV > BUFFER_SIZE_MAX) return(CpErr_GENERIC);

   //--- get pointer to buffer --------------------------------------
   pBufferT = &(tsCpMsgBufferTableG[ubBufferV - 1]);


   //----------------------------------------------------------------
   // copy the data
   //
   for(ubDataCntT = 0; ubDataCntT < 8; ubDataCntT++)
   {
      *pubDataV = pBufferT->v_MsgData[ubDataCntT];
      pubDataV++;
   }

   return(CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSetData()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
_U08 CpCoreBufferSetData(_U08 ubChannelV, _U08 ubBufferV, _U08 * pubDataV)
{
   _U08           ubDataCntT;
   _TsCpCanMsg *  pBufferT;      // pointer to buffer

   //--- check for valid buffer number ------------------------------
   if(ubBufferV > BUFFER_SIZE_MAX) return(CpErr_GENERIC);

   //--- get pointer to buffer --------------------------------------
   pBufferT = &(tsCpMsgBufferTableG[ubBufferV - 1]);


   //----------------------------------------------------------------
   // copy the data
   //
   for(ubDataCntT = 0; ubDataCntT < 8; ubDataCntT++)
   {
      pBufferT->v_MsgData[ubDataCntT] = *pubDataV;
      pubDataV++;
   }

   return(CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSetDlc()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
_U08 CpCoreBufferSetDlc(_U08 ubChannelV, _U08 ubBufferV, _U08 ubDlcV)
{
   _TsCpCanMsg *  pBufferT;      // pointer to buffer

   //--- check for valid buffer number ------------------------------
   if(ubBufferV > BUFFER_SIZE_MAX) return(CpErr_GENERIC);

   //--- get pointer to buffer --------------------------------------
   pBufferT = &(tsCpMsgBufferTableG[ubBufferV - 1]);

   CpMacSetDlc(pBufferT, ubDlcV);

   return(CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferSend()                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
_U08 CpCoreBufferSend(_U08 ubChannelV, _U08 ubBufferV)
{
   _TsCpCanMsg *  pBufferT;      // pointer to buffer

   //--- check for valid buffer number ------------------------------
   if(ubBufferV > BUFFER_SIZE_MAX) return(CpErr_GENERIC);

   //--- get pointer to buffer and setup the data -------------------
   pBufferT = &(tsCpMsgBufferTableG[ubBufferV - 1]);

   pBufferT->v_MsgFlags |= BUFFER_TX_REQ;

   //--- start transmission -----------------------------------------
   CpCoreMsgTransmit(ubChannelV);

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreCanState()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
_U08 CpCoreCanState(_U08 channel)
{
   _U08  ubStatusT;


   //--- get status of CAN controller -------------------------------
   ubStatusT = CpMacRegRead(eREG_STATUS);
   if(ubStatusT & SR_BS_BIT)  return(CpErr_BUS_OFF);
   if(ubStatusT & SR_ES_BIT)  return(CpErr_BUS_WARNING);
   if(ubStatusT & SR_DOS_BIT) return(CpErr_OVERRUN);
   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreDeAllocBuffer()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreDeAllocBuffer (_U08 ubChannelV, _U08 ubBufferV)
{
   _TsCpCanMsg *  pBufferT;      // pointer to buffer


   //--- check for valid buffer number ------------------------------
   if(ubBufferV > BUFFER_SIZE_MAX) return(CpErr_GENERIC);

   //--- get pointer to buffer and setup the data -------------------
   pBufferT = &(tsCpMsgBufferTableG[ubBufferV - 1]);


   //----------------------------------------------------------------
   // mark buffer "clear" by releasing highest bit in user data
   pBufferT->v_MsgFlags &= (~BUFFER_CONFIG);

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreInitDriver()                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreInitDriver(_U08 channel)
{
   //--- specify desired interrupts ---------------------------------
   CpVarInterruptStatusG = CR_RIE_BIT | CR_TIE_BIT;


   //----------------------------------------------------------------
   // Reset CAN controller here: pulldown Reset line (pin 17)
   // on this hardware we do so by writing to address offset 31
   //
   // Todo: adopt this to the specific hardware
   //
   // CpMacRegWrite(31, 0xFF);

   

   //--- switch to reset mode ---------------------------------------
   while ( !(CpMacRegRead(eREG_CONTROL) & CR_RR_BIT) ) 
   {
      CpMacRegWrite(eREG_CONTROL, CR_RR_BIT);
   }


   //--- clear all interrupts ---------------------------------------
   CpMacRegWrite(eREG_INTERRUPT, 0x00);

   //--- set acceptance code reg ------------------------------------
   CpMacRegWrite(eREG_ACC_CODE, 0);
   CpMacRegWrite(eREG_ACC_MASK, 0xff);


   //--- initialize output control ----------------------------------
   CpMacRegWrite(eREG_OUTPUTCTRL, 0xDE);


   //--- switch back to operating mode and enable interrupt ---------
   while ( CpMacRegRead(eREG_CONTROL) & CR_RR_BIT ) 
   {
      CpMacRegWrite(eREG_CONTROL, CpVarInterruptStatusG);
   }

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreIntHandler()                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
void CpCoreIntHandler(void)
{
   _U08  ubIntBitsT;


   //--- read the Interrupt Register --------------------------------
   ubIntBitsT = CpMacRegRead(eREG_INTERRUPT);

   //--- check if interrupt receive bit is set ----------------------
   if( ubIntBitsT & IR_RI_BIT) 
   {
      CpCoreMsgReceive(CP_CHANNEL_1);
   }

   //--- check if interrupt transmit bit is set ---------------------
   if( ubIntBitsT & IR_TI_BIT) 
   {
      CpCoreMsgTransmit(CP_CHANNEL_1);
   }

}


//----------------------------------------------------------------------------//
// CpCoreMsgTransmit()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreMsgReceive(_U08 ubChannelV)
{
   _U08           ubErrCodeT;          // error code value
   _U16           uwIdentifierT;
   _U08           ubDlcT;
   _U08           ubDataCntT;
   _U08           ubBufferCntT;
   _TsCpCanMsg *  ptsCanMsgT;          // pointer to CAN message structure


   //--- check 'Receive Buffer Access'-------------------------------
   if( !((CpMacRegRead(eREG_STATUS)) & SR_RBS_BIT) ) 
   {
      return (CpErr_RCV_EMPTY);
   }


   //--- check overrun status ---------------------------------------
   if((CpMacRegRead(eREG_STATUS)) & SR_DOS_BIT) 
   {
      ubErrCodeT = CpErr_OVERRUN;
   }


   //----------------------------------------------------------------
   // read message from the CAN controller
   //


   //--- Identifier ---------------------------------------
   uwIdentifierT = CpMacRegRead(eREG_RCVID);
   uwIdentifierT = uwIdentifierT << 8;
   uwIdentifierT = uwIdentifierT | CpMacRegRead(eREG_RCVID + 1);
   uwIdentifierT = uwIdentifierT >> 5;

   //----------------------------------------------------------------
   // check the CAN message buffers (FullCAN emulation)
   //
   ubBufferCntT = 0;
   while(ubBufferCntT < BUFFER_SIZE_MAX) 
   {
      ptsCanMsgT = &(tsCpMsgBufferTableG[ubBufferCntT]);

      //--- is the buffer configured? --------------------------
      if( (ptsCanMsgT->v_MsgFlags & BUFFER_CONFIG) > 0) 
      {

         //--- is it a receive buffer ---------------------
         if( (ptsCanMsgT->v_MsgFlags & BUFFER_DIR_TX) == 0)
         {
            //--- is the identifier correct ----------
            if(uwIdentifierT == CpMacGetStdId(ptsCanMsgT)) break;
         }
      
      }

      ubBufferCntT++;
   }
   
   //----------------------------------------------------------------
   // identifier is not in receive buffer list
   // clear the interrupt and leave the function
   //
   if(ubBufferCntT == BUFFER_SIZE_MAX) 
   {

      //--- release receive buffer ------------------------
      CpMacRegWrite(eREG_COMMAND, CMR_RRB_BIT);

      //--- clear data overrun ----------------------------
      if(CpMacRegRead(eREG_STATUS) & SR_DOS_BIT) 
      {
         CpMacRegWrite(eREG_COMMAND, CMR_CDO_BIT);
      }

      return(CpErr_RCV_EMPTY);
   }

   //--- set RTR bit if requested------------------------------------
   if(CpMacRegRead(eREG_RCVID + 1) & 0x10) CpMacSetRemote(ptsCanMsgT);


   //--- write DLC into buffer --------------------------------------
   ubDlcT =  CpMacRegRead(eREG_RCVID + 1) & 0x0F;
   CpMacSetDlc(ptsCanMsgT, ubDlcT);


   //--- write data into buffer -------------------------------------
   for(ubDataCntT = 0; ubDataCntT < ubDlcT; ubDataCntT++)
   {
      CpMacSetData(ptsCanMsgT, ubDataCntT, CpMacRegRead(eREG_RCVBUF + ubDataCntT) );
   }

   //
   //--- end of CAN message read operation --------------------------



   //--- release receive buffer -------------------------------------
   CpMacRegWrite(eREG_COMMAND, CMR_RRB_BIT);


   //--- clear data overrun -----------------------------------------
   if(CpMacRegRead(eREG_STATUS) & SR_DOS_BIT) 
   {
      CpMacRegWrite(eREG_COMMAND, CMR_CDO_BIT);
   }


   //--- put message to FIFO ----------------------------------------
   // ubErrCodeT = CpFifoPush(channel, CP_FIFO_RCV, &canMsgT);
   if(CpInt_ReceiveHandler[0]) 
   {
      ubErrCodeT = (* CpInt_ReceiveHandler[0] )(ptsCanMsgT);
   }

   return (ubErrCodeT);
}


//----------------------------------------------------------------------------//
// CpCoreMsgTransmit()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
_U08 CpCoreMsgTransmit(_U08 ubChannelV)
{
   _U08           ubTmpT;              // temporary byte value
   _U16           uwIdentifierT;       // identifier
   _TsCpCanMsg *  pCanMsgT;            // pointer to CAN message structure


   //----------------------------------------------------------------
   // check 'Transmit Buffer Status'
   // it must be on '1' (released), otherwise we can't write to
   // the SJA1000, this should never happen, since it is interrupt
   // driven
   //
   if( ( CpMacRegRead(eREG_STATUS) & SR_TBS_BIT ) == 0x00) 
   {
      return(CpErr_TRM_FULL);
   }


   //----------------------------------------------------------------
   // check 'Transmission Complete Status'
   // it must be on '1' (complete), otherwise we can't write to
   // the SJA1000, this should never happen, since it is interrupt
   // driven
   //
   if ( ( CpMacRegRead(eREG_STATUS) & SR_TCS_BIT ) == 0x00) 
   {
      return(CpErr_TRM_FULL);
   }


   //----------------------------------------------------------------
   // get buffer number
   // the while() loop will ensure that we don't run outside the
   // buffer boundaries
   //
   ubTmpT = 0;
   while(ubTmpT < BUFFER_SIZE_MAX )
   {
      pCanMsgT = &( tsCpMsgBufferTableG[ubTmpT] );
      if(pCanMsgT->v_MsgFlags & BUFFER_TX_REQ)
      {
         //-----------------------------------------------------
         // release this transmission request
         //
         pCanMsgT->v_MsgFlags &= (~BUFFER_TX_REQ);
         break;
      }

      ubTmpT++;
   }
   if(ubTmpT == BUFFER_SIZE_MAX) return(CpErr_OK);


   //----------------------------------------------------------------
   // write into transmit buffer
   //

   //--- get Identifier from _TsCpCanMsg ------------------
   uwIdentifierT = CpMacGetStdId(pCanMsgT);

   //--- and write it into transmit register --------------
   CpMacRegWrite(eREG_XMTID, (_U08) (uwIdentifierT >> 3) );

   //--- lower part of the identifier ---------------------
   ubTmpT = (_U08) (uwIdentifierT << 5);
   ubTmpT = ubTmpT | CpMacGetDlc(pCanMsgT);

   //--- check for RTR bit --------------------------------
   if( CpMacIsRemote(pCanMsgT) ) 
   {

      //--- send remote request message --------------
      CpMacRegWrite(eREG_XMTLEN, ubTmpT | 0x10 );
   }
   else 
   {

      //--- send data message ------------------------
      CpMacRegWrite(eREG_XMTLEN, ubTmpT);

      //--- put data into transmit register-----------
      CpMacRegWrite(eREG_XMTBUF + 0, pCanMsgT->v_MsgData[0]);
      CpMacRegWrite(eREG_XMTBUF + 1, pCanMsgT->v_MsgData[1]);
      CpMacRegWrite(eREG_XMTBUF + 2, pCanMsgT->v_MsgData[2]);
      CpMacRegWrite(eREG_XMTBUF + 3, pCanMsgT->v_MsgData[3]);
      CpMacRegWrite(eREG_XMTBUF + 4, pCanMsgT->v_MsgData[4]);
      CpMacRegWrite(eREG_XMTBUF + 5, pCanMsgT->v_MsgData[5]);
      CpMacRegWrite(eREG_XMTBUF + 6, pCanMsgT->v_MsgData[6]);
      CpMacRegWrite(eREG_XMTBUF + 7, pCanMsgT->v_MsgData[7]);
   }
   //
   //--- end of write into transmit buffer --------------------------


   //--- set bit for transmit request -------------------------------
   CpMacRegWrite(eREG_COMMAND, CMR_TR_BIT);


   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreRegRead()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreRegRead(_U08 ubChannelV, _U16 uwRegAdrV, _U08 * pubValueV)
{
   if(uwRegAdrV > eREG_MAX) return(CpErr_REGISTER);

   *pubValueV = CpMacRegRead(uwRegAdrV);
   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreRegWrite()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreRegWrite(_U08 ubChannelV, _U16 uwRegAdrV, _U08 ubValueV)
{
   if(uwRegAdrV > eREG_MAX) return(CpErr_REGISTER);

   CpMacRegWrite(uwRegAdrV, ubValueV);
   return (CpErr_OK);
}

