/*****************************************************************************\
*  CANpie                                                                     *
*                                                                             *
*  File        : cpfifo.c                                                     *
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
*  0.1      04.09.1999  Initial version                                 UK    *
*  0.2      18.01.2000  Fixed bug in pointer calculation                UK    *
*  0.3                  - no changes -                                        *
*  0.4                  - no changes -                                        *
*  0.5                  - no changes -                                        *
*  0.6      15.06.2000  added fixed FIFO size support                   UK    *
*                       set to CANpie release 0.6                             *
*  0.7      30.11.2000  Bugfix for static FIFO (overflow)               UK    *
*  0.8      14.02.2001  complete rewrite, check FIFO status             UK    *
*                                                                             *
\*****************************************************************************/

#include "can/cpfifo.h"
#include "pkgconf/system.h"



/*------------------------------------------------------------------------
** there are two types of FIFO implementations
** CP_FIFO_TYPE == 0 : use dynamic FIFO memory, malloc() function
** CP_FIFO_TYPE == 1 : use fixed FIFO memory size
** the definition is done in the cpconfig.h file
*/


//-----------------------------------------------
// holds the status of the FIFO
// Bit 0 = 1: Receive FIFO full
// Bit 1 = 1: Transmit FIFO full
//
_U08           CpVar_FifoStatus[CP_CHANNEL_MAX];

#define  RCV_FIFO_FULL     0x01
#define  TRM_FIFO_FULL     0x02


//----------------------------------------------------------------------------//
//                                                                            //
// Dynamic memory size FIFO implementation                                    //
//                                                                            //
//----------------------------------------------------------------------------//

#if CP_FIFO_TYPE == 0

#ifndef __KERNEL__
#include <stdlib.h>     // for malloc() and free()
#else
#if defined(OROPKG_OS_LXRT) || defined(OROPKG_OS_RTAI)
#include <rtai.h>
#include <rtai_malloc.h>
#include <rtai_sem.h>
#define free(x) rt_free(x)
#define malloc(x) rt_malloc(x)
#else
#include <linux/slab.h>
#define free(x) kfree(x)
#define malloc(x) kmalloc(x,GFP_KERNEL)
#endif
#endif

/*------------------------------------------------------------------------
** variables used for the FIFO functions
**
*/

CpStruct_CAN * CpVar_FifoRcv[CP_CHANNEL_MAX];
_U16           CpVar_FifoRcvSize[CP_CHANNEL_MAX];
_U16           CpVar_FifoRcvHead[CP_CHANNEL_MAX];
_U16           CpVar_FifoRcvTail[CP_CHANNEL_MAX];

CpStruct_CAN * CpVar_FifoTrm[CP_CHANNEL_MAX];
_U16           CpVar_FifoTrmSize[CP_CHANNEL_MAX];
_U16           CpVar_FifoTrmHead[CP_CHANNEL_MAX];
_U16           CpVar_FifoTrmTail[CP_CHANNEL_MAX];


#if defined(OROPKG_OS_LXRT) || defined(OROPKG_OS_RTAI)
static SEM cp_tx_fifo_sem;
static SEM cp_rx_fifo_sem;
#endif


//----------------------------------------------------------------------------//
// CpFifoSetup()                                                              //
// Setup FIFO size                                                            //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpFifoSetup(_U08 channel, _U08 buffer, _U16 size)
{
#if   CP_SMALL_CODE == 0
   //--- test the channel number --------------------------
   if( (channel + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   //--- allocate memory for Receive FIFO -----------------
   if(buffer == CP_FIFO_RCV)
   {
      CpVar_FifoRcv[channel] = malloc(CP_CAN_MESSAGE_SIZE * size);
      if(CpVar_FifoRcv[channel] == 0L) return (CpErr_FIFO_SIZE);

      /*----------------------------------
      ** store FIFO size and make it
      ** empty (head = tail = 0)
      */
      CpVar_FifoRcvSize[channel] = size;
      CpVar_FifoRcvHead[channel] = 0;
      CpVar_FifoRcvTail[channel] = 0;
   }

   //--- allocate memory for Transmit FIFO ----------------
   if(buffer == CP_FIFO_TRM)
   {
      CpVar_FifoTrm[channel] = malloc(CP_CAN_MESSAGE_SIZE * size);
      if(CpVar_FifoTrm[channel] == 0L) return (CpErr_FIFO_SIZE);

      /*----------------------------------
      ** store FIFO size and make it
      ** empty (head = tail = 0)
      */
      CpVar_FifoTrmSize[channel] = size;
      CpVar_FifoTrmHead[channel] = 0;
      CpVar_FifoTrmTail[channel] = 0;
   }

   //--- FIFOs are not full -----------------------------------------
   CpVar_FifoStatus[channel] = 0;

#if defined(OROPKG_OS_LXRT) || defined(OROPKG_OS_RTAI)
   rt_sem_init(&cp_tx_fifo_sem, 1);
   rt_sem_init(&cp_rx_fifo_sem, 1);
#endif

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpFifoRemove()                                                             //
// remove FIFO and free memory                                                //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpFifoRemove(_U08 channel, _U08 buffer)
{
#if   CP_SMALL_CODE == 0
   //--- test the channel number --------------------------
   if( (channel + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   if(buffer == CP_FIFO_RCV) free(CpVar_FifoRcv[channel]);
   if(buffer == CP_FIFO_TRM) free(CpVar_FifoTrm[channel]);

#if defined(OROPKG_OS_LXRT) || defined(OROPKG_OS_RTAI)
   rt_sem_delete(&cp_tx_fifo_sem);
   rt_sem_delete(&cp_rx_fifo_sem);
#endif

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpFifoClear()                                                              //
// clear contents of FIFO                                                     //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpFifoClear(_U08 channel, _U08 buffer)
{
#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if( (channel + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif
   //--- delete messages from Receive FIFO ----------------
   if(buffer == CP_FIFO_RCV)
   {
	   rt_sem_wait(&cp_rx_fifo_sem);
      CpVar_FifoRcvHead[channel] = 0;
      CpVar_FifoRcvTail[channel] = 0;
      
      CpVar_FifoStatus[channel]  &= ~RCV_FIFO_FULL;
	  rt_sem_signal(&cp_rx_fifo_sem);
   }

   //--- delete messages from Transmit FIFO ---------------
   if(buffer == CP_FIFO_TRM)
   {
	   rt_sem_wait(&cp_tx_fifo_sem);
      CpVar_FifoTrmHead[channel] = 0;
      CpVar_FifoTrmTail[channel] = 0;

      CpVar_FifoStatus[channel]  &= ~TRM_FIFO_FULL;
	  rt_sem_signal(&cp_tx_fifo_sem);
   }

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpFifoPush()                                                               //
// push message to FIFO                                                       //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpFifoPush(_U08 channel, _U08 buffer, const CpStruct_CAN * msg)
{
   CpStruct_CAN * fifoPtr = 0;
   register _U08  cnt;
   register _U16  uwFifoHeadT;   // head position of FIFO
   register _U16  uwFifoTailT;   // tail position of FIFO
   register _U16  uwFifoSizeT;   // max number of entries in FIFO
   SEM* sem_to_signal;


#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if( (channel + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif


   //--- setup variables for Receive/Transmit FIFOs------------------
   if(buffer == CP_FIFO_RCV)
   {
      //--- test if FIFO is full --------------------------
      if(CpVar_FifoStatus[channel] & RCV_FIFO_FULL) return (CpErr_FIFO_FULL);
	  rt_sem_wait(&cp_rx_fifo_sem);
	  sem_to_signal = &cp_rx_fifo_sem;

      uwFifoHeadT = CpVar_FifoRcvHead[channel];
      uwFifoTailT = CpVar_FifoRcvTail[channel];
      uwFifoSizeT = CpVar_FifoRcvSize[channel];

      fifoPtr = CpVar_FifoRcv[channel];
   }
   else
   {
      //--- test if FIFO is full --------------------------
      if(CpVar_FifoStatus[channel] & TRM_FIFO_FULL) { 
          return (CpErr_FIFO_FULL);
      }
	  rt_sem_wait(&cp_tx_fifo_sem);
	  sem_to_signal = &cp_tx_fifo_sem;

      uwFifoHeadT = CpVar_FifoTrmHead[channel];
      uwFifoTailT = CpVar_FifoTrmTail[channel];
      uwFifoSizeT = CpVar_FifoTrmSize[channel];

      fifoPtr = CpVar_FifoTrm[channel];
   }

   
   //----------------------------------------------------------------
   // FIFO is not full, insert the message
   //
   fifoPtr = fifoPtr + uwFifoTailT;

   fifoPtr->v_MsgId     = msg->v_MsgId;
   fifoPtr->v_MsgFlags  = msg->v_MsgFlags;
   for (cnt = 0; cnt < 8; cnt++)
   {
      fifoPtr->v_MsgData[cnt] = msg->v_MsgData[cnt];
   }

#if CP_MSG_TIME == 1
   fifoPtr->v_MsgTime   = msg->v_MsgTime;
#endif

#if CP_USR_DATA == 1
   fifoPtr->v_UsrData   = msg->v_UsrData;
#endif


   //----------------------------------------------------------------
   // test if FIFO is full:
   //
   if(uwFifoHeadT == uwFifoTailT + 1)
   {
      if(buffer == CP_FIFO_RCV) CpVar_FifoStatus[channel] |= RCV_FIFO_FULL;
      else CpVar_FifoStatus[channel] |= TRM_FIFO_FULL; 

	  rt_sem_signal(sem_to_signal);
      return (CpErr_OK);
   }

   if( (uwFifoHeadT == 0) && (uwFifoTailT == uwFifoSizeT - 1) )
   {
      if(buffer == CP_FIFO_RCV) CpVar_FifoStatus[channel] |= RCV_FIFO_FULL;
      else CpVar_FifoStatus[channel] |= TRM_FIFO_FULL; 

	  rt_sem_signal(sem_to_signal);
      return (CpErr_OK);
   }

   //----------------------------------------------------------------
   // increment tail position and save it
   // test overflow condition
   uwFifoTailT += 1;
   if(uwFifoTailT == uwFifoSizeT) uwFifoTailT = 0;
   
   
   if(buffer == CP_FIFO_RCV)
   {
      CpVar_FifoRcvTail[channel] = uwFifoTailT;
   }
   else
   {
      CpVar_FifoTrmTail[channel] = uwFifoTailT;
   }

   rt_sem_signal(sem_to_signal);
   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpFifoPop()                                                                //
// pop message from FIFO                                                      //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpFifoPop(_U08 channel, _U08 buffer, CpStruct_CAN * msg)
{
   CpStruct_CAN * fifoPtr = 0;
   register _U08  cnt;
   register _U16  uwFifoHeadT;   // head position of FIFO
   register _U16  uwFifoTailT;   // tail position of FIFO
   register _U16  uwFifoSizeT;   // max number of entries in FIFO
   SEM* sem_to_signal;

#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if( (channel + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   //--- setup variables for Receive/Transmit FIFOs------------------
   if(buffer == CP_FIFO_RCV)
   {
	  rt_sem_wait(&cp_rx_fifo_sem);
	  sem_to_signal = &cp_rx_fifo_sem;

      uwFifoHeadT = CpVar_FifoRcvHead[channel];
      uwFifoTailT = CpVar_FifoRcvTail[channel];
      uwFifoSizeT = CpVar_FifoRcvSize[channel];

      fifoPtr = CpVar_FifoRcv[channel];
   }
   else
   {
	  rt_sem_wait(&cp_tx_fifo_sem);
	  sem_to_signal = &cp_tx_fifo_sem;

      uwFifoHeadT = CpVar_FifoTrmHead[channel];
      uwFifoTailT = CpVar_FifoTrmTail[channel];
      uwFifoSizeT = CpVar_FifoTrmSize[channel];

      fifoPtr = CpVar_FifoTrm[channel];
   }


   //----------------------------------------------------------------
   // test if FIFO is empty
   //
   if(uwFifoHeadT == uwFifoTailT)
   {
	  rt_sem_signal(sem_to_signal);
      return (CpErr_FIFO_EMPTY);
   }

   //--- get message from FIFO --------------------------------------
   fifoPtr = fifoPtr + uwFifoHeadT;


   msg->v_MsgId      = fifoPtr->v_MsgId;
   msg->v_MsgFlags   = fifoPtr->v_MsgFlags;
   for (cnt = 0; cnt < 8; cnt++)
   {
      msg->v_MsgData[cnt] = fifoPtr->v_MsgData[cnt];
   }

#if CP_MSG_TIME == 1
   msg->v_MsgTime = fifoPtr->v_MsgTime;
#endif

#if CP_USR_DATA == 1
   msg->v_UsrData = fifoPtr->v_UsrData;
#endif


   //----------------------------------------------------------------
   // increment head position and save it 
   //

   uwFifoHeadT += 1;
   if(uwFifoHeadT == uwFifoSizeT) uwFifoHeadT = 0;

   if(buffer == CP_FIFO_RCV)
   {
      CpVar_FifoRcvHead[channel] = uwFifoHeadT;

      //---------------------------------------------------
      // if there is a FIFO full condition: clear it
      //
      if(CpVar_FifoStatus[channel] & RCV_FIFO_FULL)
      {
         CpVar_FifoStatus[channel]  &= ~RCV_FIFO_FULL;

         //--- move tail to next free entry ---------------
         uwFifoTailT += 1;
         if(uwFifoTailT == uwFifoSizeT) uwFifoTailT = 0;
         CpVar_FifoRcvTail[channel] = uwFifoTailT;
      }
      
   }
   else
   {
      CpVar_FifoTrmHead[channel] = uwFifoHeadT;

      //---------------------------------------------------
      // if there is a FIFO full condition: clear it
      //
      if(CpVar_FifoStatus[channel] & TRM_FIFO_FULL)
      {
         CpVar_FifoStatus[channel]  &= ~TRM_FIFO_FULL;

         //--- move tail to next free entry ---------------
         uwFifoTailT += 1;
         if(uwFifoTailT == uwFifoSizeT) uwFifoTailT = 0;
         CpVar_FifoTrmTail[channel] = uwFifoTailT;
      }
   }

   rt_sem_signal(sem_to_signal);

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
//                                                                            //
// Fixed memory size FIFO implementation                                      //
//                                                                            //
//----------------------------------------------------------------------------//


#elif CP_FIFO_TYPE == 1

#if defined(OROPKG_OS_LXRT) || defined(OROPKG_OS_RTAI)
#error "Use the non fixed size memory implementation for RTAI/LXRT"
#endif

/*------------------------------------------------------------------------
** variables used for the FIFO functions
**
*/

CpStruct_CAN   CpVar_FifoRcv[CP_CHANNEL_MAX][CP_FIFO_TRM_SIZE];
_U08           CpVar_FifoRcvHead[CP_CHANNEL_MAX];
_U08           CpVar_FifoRcvTail[CP_CHANNEL_MAX];

CpStruct_CAN   CpVar_FifoTrm[CP_CHANNEL_MAX][CP_FIFO_RCV_SIZE];
_U08           CpVar_FifoTrmHead[CP_CHANNEL_MAX];
_U08           CpVar_FifoTrmTail[CP_CHANNEL_MAX];


//----------------------------------------------------------------------------//
// CpFifoSetup()                                                              //
// Setup FIFO size                                                            //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpFifoSetup(_U08 channel, _U08 buffer, _U16 size)
{
#if   CP_SMALL_CODE == 0
   //--- test the channel number --------------------------
   if( (channel + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   //--- setup Receive FIFO -----------------------------------------
   if(buffer == CP_FIFO_RCV)
   {

      /*----------------------------------
      ** store FIFO size and make it
      ** empty (head = tail = 0)
      */
      CpVar_FifoRcvHead[channel] = 0;
      CpVar_FifoRcvTail[channel] = 0;
   }

   //--- setup Transmit FIFO ----------------------------------------
   if(buffer == CP_FIFO_TRM)
   {

      /*----------------------------------
      ** store FIFO size and make it
      ** empty (head = tail = 0)
      */
      CpVar_FifoTrmHead[channel] = 0;
      CpVar_FifoTrmTail[channel] = 0;
   }

   //--- FIFOs are not full -----------------------------------------
   CpVar_FifoStatus[channel] = 0;

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpFifoRemove()                                                             //
// remove FIFO and free memory                                                //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpFifoRemove(_U08 channel, _U08 buffer)
{
   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpFifoClear()                                                              //
// clear contents of FIFO                                                     //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpFifoClear(_U08 channel, _U08 buffer)
{
#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if( (channel + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   //--- delete messages from Receive FIFO --------------------------
   if(buffer == CP_FIFO_RCV) 
   {
      CpVar_FifoRcvHead[channel] = 0;
      CpVar_FifoRcvTail[channel] = 0;
   }

   //--- delete messages from Transmit FIFO -------------------------
   if(buffer == CP_FIFO_TRM) 
   {
      CpVar_FifoTrmHead[channel] = 0;
      CpVar_FifoTrmTail[channel] = 0;
   }

   
   //--- FIFOs are not full -----------------------------------------
   CpVar_FifoStatus[channel] = 0;

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpFifoPush()                                                               //
// push message to FIFO                                                       //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpFifoPush(_U08 channel, _U08 buffer, CpStruct_CAN * msg)
{
   CpStruct_CAN * fifoPtr = 0;
   register _U08  cnt;
   register _U08  ubFifoHeadT;   // head position of FIFO
   register _U08  ubFifoTailT;   // tail position of FIFO
   register _U08  ubFifoSizeT;   // max number of entries in FIFO


#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if( (channel + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   //--- setup variables for Receive/Transmit FIFOs------------------
   if(buffer == CP_FIFO_RCV)
   {
      //--- test if FIFO is full --------------------------
      if(CpVar_FifoStatus[channel] & RCV_FIFO_FULL) return (CpErr_FIFO_FULL);

      ubFifoHeadT = CpVar_FifoRcvHead[channel];
      ubFifoTailT = CpVar_FifoRcvTail[channel];
      ubFifoSizeT = CP_FIFO_RCV_SIZE;

      fifoPtr = CpVar_FifoRcv[channel];
   }
   else
   {
      //--- test if FIFO is full --------------------------
      if(CpVar_FifoStatus[channel] & TRM_FIFO_FULL) return (CpErr_FIFO_FULL);

      ubFifoHeadT = CpVar_FifoTrmHead[channel];
      ubFifoTailT = CpVar_FifoTrmTail[channel];
      ubFifoSizeT = CP_FIFO_TRM_SIZE;

      fifoPtr = CpVar_FifoTrm[channel];
   }


   //----------------------------------------------------------------
   // FIFO is not full, insert the message
   //

   fifoPtr = fifoPtr + ubFifoTailT;

   fifoPtr->v_MsgId     = msg->v_MsgId;
   fifoPtr->v_MsgFlags  = msg->v_MsgFlags;
   for (cnt = 0; cnt < 8; cnt++)
   {
      fifoPtr->v_MsgData[cnt] = msg->v_MsgData[cnt];
   }

#if CP_MSG_TIME == 1
   fifoPtr->v_MsgTime   = msg->v_MsgTime;
#endif

#if CP_USR_DATA == 1
   fifoPtr->v_UsrData   = msg->v_UsrData;
#endif

   //----------------------------------------------------------------
   // test if FIFO is full:
   //
   if(ubFifoHeadT == ubFifoTailT + 1)
   {
      if(buffer == CP_FIFO_RCV) CpVar_FifoStatus[channel] |= RCV_FIFO_FULL;
      else CpVar_FifoStatus[channel] |= TRM_FIFO_FULL; 

      return (CpErr_OK);
   }

   if( (ubFifoHeadT == 0) && (ubFifoTailT == ubFifoSizeT - 1) )
   {
      if(buffer == CP_FIFO_RCV) CpVar_FifoStatus[channel] |= RCV_FIFO_FULL;
      else CpVar_FifoStatus[channel] |= TRM_FIFO_FULL; 

      return (CpErr_OK);
   }

   //----------------------------------------------------------------
   // increment tail position and save it
   // test overflow condition
   ubFifoTailT += 1;
   if(ubFifoTailT == ubFifoSizeT) ubFifoTailT = 0;

   if(buffer == CP_FIFO_RCV)
   {
      CpVar_FifoRcvTail[channel] = ubFifoTailT;
   }
   else
   {
      CpVar_FifoTrmTail[channel] = ubFifoTailT;
   }

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpFifoPop()                                                                //
// pop message from FIFO                                                      //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpFifoPop(_U08 channel, _U08 buffer, CpStruct_CAN * msg)
{
   CpStruct_CAN * fifoPtr = 0;
   register _U08  cnt;
   register _U08  ubFifoHeadT;   // head position of FIFO
   register _U08  ubFifoTailT;   // tail position of FIFO
   register _U08  ubFifoSizeT;   // max number of entries in FIFO


#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if( (channel + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif


   //--- setup variables for Receive/Transmit FIFOs------------------
   if(buffer == CP_FIFO_RCV)

   {
      ubFifoHeadT = CpVar_FifoRcvHead[channel];
      ubFifoTailT = CpVar_FifoRcvTail[channel];
      ubFifoSizeT = CP_FIFO_RCV_SIZE;

      fifoPtr = CpVar_FifoRcv[channel];
   }
   else
   {
      ubFifoHeadT = CpVar_FifoTrmHead[channel];
      ubFifoTailT = CpVar_FifoTrmTail[channel];
      ubFifoSizeT = CP_FIFO_TRM_SIZE;

      fifoPtr = CpVar_FifoTrm[channel];
   }


   //----------------------------------------------------------------
   // test if FIFO is empty
   //
   if(ubFifoHeadT == ubFifoTailT)
   {
      return (CpErr_FIFO_EMPTY);
   }


   //--- get message from FIFO --------------------------------------
   fifoPtr = fifoPtr + ubFifoHeadT;


   msg->v_MsgId      = fifoPtr->v_MsgId;
   msg->v_MsgFlags   = fifoPtr->v_MsgFlags;
   for (cnt = 0; cnt < 8; cnt++)
   {
      msg->v_MsgData[cnt] = fifoPtr->v_MsgData[cnt];
   }

#if CP_MSG_TIME == 1
   msg->v_MsgTime = fifoPtr->v_MsgTime;
#endif

#if CP_USR_DATA == 1
   msg->v_UsrData = fifoPtr->v_UsrData;
#endif

   //----------------------------------------------------------------
   // increment head position and save it 
   //

   ubFifoHeadT += 1;
   if(ubFifoHeadT == ubFifoSizeT) ubFifoHeadT = 0;

   if(buffer == CP_FIFO_RCV)
   {
      CpVar_FifoRcvHead[channel] = ubFifoHeadT;

      //---------------------------------------------------
      // if there is a FIFO full condition: clear it
      //
      if(CpVar_FifoStatus[channel] & RCV_FIFO_FULL)
      {
         CpVar_FifoStatus[channel]  &= ~RCV_FIFO_FULL;

         //--- move tail to next free entry ---------------
         ubFifoTailT += 1;
         if(ubFifoTailT == ubFifoSizeT) ubFifoTailT = 0;
         CpVar_FifoRcvTail[channel] = ubFifoTailT;
      }
      
   }
   else
   {
      CpVar_FifoTrmHead[channel] = ubFifoHeadT;

      //---------------------------------------------------
      // if there is a FIFO full condition: clear it
      //
      if(CpVar_FifoStatus[channel] & TRM_FIFO_FULL)
      {
         CpVar_FifoStatus[channel]  &= ~TRM_FIFO_FULL;

         //--- move tail to next free entry ---------------
         ubFifoTailT += 1;
         if(ubFifoTailT == ubFifoSizeT) ubFifoTailT = 0;
         CpVar_FifoTrmTail[channel] = ubFifoTailT;
      }
   }

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// Error for definition CP_FIFO_TYPE                                                                           //
//----------------------------------------------------------------------------//
#else
#error Unsupported definition for CP_FIFO_TYPE. Please check cpconfig.h!
#endif

