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

#include <pkgconf/system.h>
#include <pkgconf/device_drivers_canpie.h>

#include "can/cpconfig.h"
#include "can/cpcore.h"
#include "can/cpfifo.h"
#include "can/cpfilter.h"
#include "can/cpmacro.h"

/* LVD: Include header for LVD CAN controller */
#include "can/pelican.h" 

#if defined(OROPKG_OS_RTAI) || defined(OROPKG_OS_LXRT)
#include <rtai.h>
#include <rtai_sched.h>
#include "can/cplxrt.h"
#endif


// Holds the number of interrupts processed.
 unsigned int cp_recv_int;
 unsigned int cp_emcy_int;
 unsigned int cp_trns_int;


#define LVD_DEBUG

#ifdef LVD_DEBUG
#if defined(OROPKG_OS_RTAI) || defined(OROPKG_OS_LXRT)
#define DEBUG(...) rt_printk(__VA_ARGS__)
#else
#define DEBUG(...) printk(__VA_ARGS__)
#endif
#else
#define DEBUG(...) while(0) {}
#endif 

/*-------------------------------------------------------------------
** Global variables used for the core functions
**
*/
static _U08    CpVar_CAN_Status[CP_CHANNEL_MAX];

_U08 (* CpInt_ReceiveHandler[CP_CHANNEL_MAX] )(_U08, CpStruct_CAN *);
_U08 (* CpInt_TransmitHandler[CP_CHANNEL_MAX])(_U08, CpStruct_CAN *);
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
   { BTR_20KB  & 0XFF, BTR_20KB  >> 8, 0x00 },   /*  10 KBit/sec   not supported */
   { BTR_20KB  & 0XFF, BTR_20KB  >> 8, 0x00 },   /*  20 KBit/sec   */
   { BTR_50KB  & 0XFF, BTR_50KB  >> 8, 0x00 },   /*  50 KBit/sec   */
   { BTR_100KB & 0XFF, BTR_100KB >> 8, 0x00 },   /* 100 KBit/sec   */
   { BTR_125KB & 0XFF, BTR_125KB >> 8, 0x00 },   /* 125 KBit/sec   */
   { BTR_250KB & 0XFF, BTR_250KB >> 8, 0x00 },   /* 250 KBit/sec   */
   { BTR_500KB & 0XFF, BTR_500KB >> 8, 0x00 },   /* 500 KBit/sec   */
   { BTR_500KB & 0XFF, BTR_500KB >> 8, 0x00 },   /* 800 KBit/sec   not supported */
   { BTR_1MB   & 0XFF, BTR_1MB   >> 8, 0x00 }    /*   1 MBit/sec   */
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
  //	BYTE_t cr;
	DEBUG("CpCoreBaudrate\n");
#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
    if( ubChannelV >= CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

//**
// Since resetting is _bad_ we set baudrate in init.
//
    
    /* Command: reset */
/*
    cr = PCAN_MODR_RM;
	do { write_reg_bcan(PCAN_MODR,cr); } while ( (read_reg_bcan(PCAN_MODR) & cr) != cr);

	CpVar_CAN_Status[ubChannelV] = CP_MODE_STOP;

	// Set bus timing parameters 
	write_reg_bcan(PCAN_BTR0,ubBtr0V);
	write_reg_bcan(PCAN_BTR1,ubBtr1V);
*/
    /* Switch to bus-on state */
/*    cr = 0;
	do { write_reg_bcan(PCAN_MODR,cr); } while ((read_reg_bcan(PCAN_MODR) & cr) != cr);

	// Command: enable receive interrupt, error warning limit 
	cr = PCAN_IER_RIE|PCAN_IER_EIE;
	write_reg_bcan(PCAN_IER,cr);

	CpVar_CAN_Status[ubChannelV] = CP_MODE_START;
*/
   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreBufferDLC()                                                     		//
// Change DLC of buffer                                                			//
//----------------------------------------------------------------------------//
#if   CP_FULL_CAN == 1
Cp_EXPORT _U08 Cp_PREFIX CpCoreBufferDLC( _U08 ubChannelV,
					  _U08 ubBufferV, _U08 ubDlcV)
{
	DEBUG("CpCoreBufferDLC\n");
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
  DEBUG("CpCoreBufferData\n");
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
   DEBUG("CpCorBufferSend\n");
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
   DEBUG("CpCorBufferTransmit\n");
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
   BYTE_t cr;

   DEBUG("CpCoreCANMode\n");
#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if (ubChannelV >= CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   switch (ubModeV)
     {
     case CP_MODE_STOP: 
       cr = PCAN_MODR_RM; 
       break;
     case CP_MODE_START: 
       cr = 0; 
       break;
     case CP_MODE_SLEEP: 
       cr = PCAN_MODR_SM; 
       break;
     default :
       return (CpVar_CAN_Status[ubChannelV]);
   }

   do { write_reg_bcan(PCAN_MODR,cr); } while ( (read_reg_bcan(PCAN_MODR) & cr) != cr);

   CpVar_CAN_Status[ubChannelV] = ubModeV;

#if   CP_CHANNEL_MAX == 1
   return (CpVar_CAN_Status[0]);
#else
   return (CpVar_CAN_Status[ubChannelV]);
#endif
}


//----------------------------------------------------------------------------//
// CpCoreCANState()                                                           //
// Retrieve state of CAN controller                                            //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX    CpCoreCANState(_U08 ubChannelV)
{
   _U08	ubCurrentStateT;
   BYTE_t  b;
	
   DEBUG("CpCoreCANState\n");
#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if (ubChannelV >= CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   ubCurrentStateT = CP_STATE_ACTIVE;

   b = read_reg_bcan(PCAN_SR);

   if (b & PCAN_SR_BS) ubCurrentStateT = CP_STATE_BUS_OFF; 
   if (b & PCAN_SR_ES) ubCurrentStateT = CP_STATE_BUS_WARN; 
   
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
   BYTE_t cr;

  DEBUG("CpCoreDeInitDriver\n");
#if   CP_SMALL_CODE == 0
   //---	test the channel number ------------------------------------
   if( (ubChannelV + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif
   /* disable can interrupt 
    */
#if defined(OROPKG_OS_RTAI) || defined(OROPKG_OS_LXRT)
   rt_disable_irq(CAN_IRQ_LINE);
   rt_free_global_irq(CAN_IRQ_LINE);
   //rt_free_linux_irq(CAN_IRQ_LINE, 0);
#endif
   /* Command: reset */
   cr = PCAN_MODR_RM;
   do { write_reg_bcan(PCAN_MODR,cr); } while ( (read_reg_bcan(PCAN_MODR) & cr) != cr);

   CpVar_CAN_Status[ubChannelV] = CP_MODE_STOP;

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreInitDriver()                                                         //
// Initialize the CAN driver                                                  //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreInitDriver(_U08 ubChannelV)
{
	BYTE_t cr;
	BYTE_t b;
	DEBUG("CpCoreInitDriver\n");

#if   CP_SMALL_CODE == 0
   //---	test the channel number ------------------------------------
	if( (ubChannelV + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

	/* Command: reset */
	cr = PCAN_MODR_RM;
	DEBUG("reset...");
	do { write_reg_bcan(PCAN_MODR,cr); } while ( (read_reg_bcan(PCAN_MODR) & PCAN_MODR_RM) != cr);
	DEBUG("ok\n");

	CpVar_CAN_Status[ubChannelV] = CP_MODE_STOP;

	/* Set clock divider register */
	b = (BYTE_t)((CDR_16MHZ & 0x07)|PCAN_CDR_PCAN);
	write_reg_bcan(PCAN_CDR, b);

	/* Command: disable all interrupt */
	cr = 0x00;
	write_reg_bcan(PCAN_IER,cr);

	/* Acceptance code and mask */
	b = (BYTE_t) 0xFF;
	write_reg_bcan(PCAN_AMR0,b);
	write_reg_bcan(PCAN_AMR1,b);
	write_reg_bcan(PCAN_AMR2,b);
	write_reg_bcan(PCAN_AMR3,b);
	b = (BYTE_t) 0x00;
	write_reg_bcan(PCAN_ACR0,b);
	write_reg_bcan(PCAN_ACR1,b);
	write_reg_bcan(PCAN_ACR2,b);
	write_reg_bcan(PCAN_ACR3,b);

	/* Set bus timing parameters */
    // WAS : BTR_1MB
	write_reg_bcan(PCAN_BTR0,BTR_500KB & 0xFF);
	write_reg_bcan(PCAN_BTR1,BTR_500KB >> 8);

	/* Set output control register */
	b = (BYTE_t) OCR_PUSHPULL;
	write_reg_bcan(PCAN_OCR, b); 

    /* Switch to bus-on state */
    cr = 0;
	DEBUG("setting bus on...");
	do { write_reg_bcan(PCAN_MODR,cr); } while ( (read_reg_bcan(PCAN_MODR) & PCAN_MODR_RM) == PCAN_MODR_RM);
	DEBUG("ok\n");

	/* Command: enable receive interrupt, error warning limit, enable transmit interrupt */
	cr = PCAN_IER_RIE|PCAN_IER_EIE|PCAN_IER_TIE;
    /* Additional interrupts enabled. for testing */
    cr |= PCAN_IER_BEIE|PCAN_IER_ALIE|PCAN_IER_EPIE|PCAN_IER_WUIE|PCAN_IER_DOIE;
	write_reg_bcan(PCAN_IER,cr);

	CpVar_CAN_Status[ubChannelV] = CP_MODE_START;

	/**
	 * TODO : register interrupt handler
	 * */
#if defined(OROPKG_OS_RTAI) || defined(OROPKG_OS_LXRT)
	//rt_request_linux_irq( CAN_IRQ_LINE, &CpCoreIntHandler, "CANInterrupt", 0 );
	rt_request_global_irq(CAN_IRQ_LINE,&CpCoreIntHandler);
	rt_startup_irq(CAN_IRQ_LINE);
	DEBUG("Interrupt requested and enabled ! \n");
#else
	DEBUG("Interrupt handler not available! \n");
#endif
	
			
   return (CpErr_OK);
}



//----------------------------------------------------------------------------//
// CpCoreIntHandler()                                                         //
// Interrupt Handler of CANpie                                                //
//----------------------------------------------------------------------------//
void CpCoreIntHandler(void)//( int irq, void* dev_id, struct pt_regs* regs )//(void)
{
  /**
   * This interrupt routing only supports one channel,
   * being channel zero of the CAN Controller.
   */
	BYTE_t  Ir;
	BYTE_t  cr;
	_U08 error;

	DEBUG("CpCoreIntHandler\n");
	Ir = read_reg_bcan(PCAN_IR);

	if ( (Ir & PCAN_IR_RI) == PCAN_IR_RI) 
	{
		DEBUG("Receive Interrupt\n");
        /* Read the new message */
		//CpCoreMsgReceive(0);
		rt_sem_signal( &cp_rx_sem );
        ++cp_recv_int;
	}
	else if ( (Ir & PCAN_IR_TI) == PCAN_IR_TI)
        {
            DEBUG("Transmit Interrupt\n");
            /* Transmit any messages still in fifo */
            //CpCoreMsgTransmit(0);
			rt_sem_signal( &cp_tx_sem );
            ++cp_trns_int;
        }
    else if ((Ir & PCAN_IR_EI) == PCAN_IR_EI)
		{
			DEBUG("Emergency Interrupt\n");
			if ((read_reg_bcan(PCAN_SR) & PCAN_SR_ES) == PCAN_SR_ES)
			{
                ++cp_emcy_int;
			  /* error warning limit */
			  //----------------------------------------------------------------
			  //	check for error handler
			  //
/* 			  if(CpInt_ErrorHandler[0]) */
/* 			   { */
/* 			     error = (* CpInt_ErrorHandler[0] )(0); */
/* 			   } */

			  /* clear reset mode bit : Switch to bus-on state */
			  cr = 0;
			  do { write_reg_bcan(PCAN_MODR,cr); } while ( (read_reg_bcan(PCAN_MODR) & PCAN_MODR_RM) == PCAN_MODR_RM );
			}
            
		}
    else if ((Ir & PCAN_IR_BEI) == PCAN_IR_BEI) {
        // Bus-error interrupt
        DEBUG("Bus-error interrupt\n");
    }
    else if ((Ir & PCAN_IR_ALI) == PCAN_IR_ALI) {
        DEBUG("Arbitration lost interrupt\n");
        
    }
    else if ((Ir & PCAN_IR_EPI) == PCAN_IR_EPI) {
        DEBUG("Error-passive interrupt\n");
    }
    else if ((Ir & PCAN_IR_WUI) == PCAN_IR_WUI) {
        DEBUG("Wake-up interrupt\n");
    }
    else if ((Ir & PCAN_IR_DOI) == PCAN_IR_DOI) {
        DEBUG("Data-overrun interrupt\n");
    }

#if defined(OROPKG_OS_RTAI) || defined(OROPKG_OS_LXRT)
	rt_enable_irq(CAN_IRQ_LINE);
#endif
}


//----------------------------------------------------------------------------//
// CpCoreHDI()                                                                //
// Fill Hardware Description Interface (HDI) structure                        //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreHDI(_U08 ubChannelV, CpStruct_HDI * pHdiV)

{
  DEBUG("CpCoreHDI\n");
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
   pHdiV->v_ControllerType = CP_CC_SJA1000;


   //----------------------------------------------------------------
   // insert the IRQ number (if applicable) here
   //
   pHdiV->v_IRQNumber = 10;

   
   //----------------------------------------------------------------
   // CANpie version major and minor are automatically set
   //
   pHdiV->v_VersionMajor  = CP_VERSION_MAJOR;
   pHdiV->v_VersionMinor  = CP_VERSION_MINOR;


   //----------------------------------------------------------------
   // insert the driver name here
   //
   pHdiV->v_DriverName    = "LVD CAN driver for eNET-CAN (PC-104)";


   //----------------------------------------------------------------
   // insert the driver version number here
   //
   pHdiV->v_VersionNumber = "Version 0.1";

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreFilterAll()                                                          //
// Enable / Disable filter for all messages                                   //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreFilterAll(_U08 ubChannelV, _BIT btEnableV)
{
   BYTE_t b;

  DEBUG("CpCoreFilterAll\n");
#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if( (ubChannelV + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   /* Acceptance code and mask */
   if (btEnableV) b = (BYTE_t) 0xFF; 
   else b = (BYTE_t) 0x00;
   write_reg_bcan(PCAN_AMR0,b);
   write_reg_bcan(PCAN_AMR1,b);
   write_reg_bcan(PCAN_AMR2,b);
   write_reg_bcan(PCAN_AMR3,b);

   b = (BYTE_t) 0x00;
   write_reg_bcan(PCAN_ACR0,b);
   write_reg_bcan(PCAN_ACR1,b);
   write_reg_bcan(PCAN_ACR2,b);
   write_reg_bcan(PCAN_ACR3,b);

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreFilterMsg()                                                          //
// Enable / Disable filter for a single messages                              //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreFilterMsg(_U08 ubChannelV, _U16 uwIdV, _BIT btEnableV)
{
  DEBUG("CpCoreFilterMsg\n");
#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if( ubChannelV >= CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   if (!btEnableV) return CpErr_NOT_SUPPORTED;

   /* set mask bits */
   write_reg_bcan(PCAN_AMR0,0x00);
   write_reg_bcan(PCAN_AMR1,0x1F);
   write_reg_bcan(PCAN_AMR2,0xFF);
   write_reg_bcan(PCAN_AMR3,0xFF);

   /* set code bits */
   write_reg_bcan(PCAN_ACR0,(uwIdV >> 3) & 0xFF);
   write_reg_bcan(PCAN_ACR1,(uwIdV << 5) & 0xE0);
   write_reg_bcan(PCAN_ACR2,0x00);
   write_reg_bcan(PCAN_ACR3,0x00);

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

   BYTE_t   frame_info;
   BYTE_t   Data[8];
   BYTE_t*  Data_ptr = Data; // HACK XXX macro otherwises does not compile
   /*BYTE_t	cr;*/

   XADDR_t  cm;    /* can message addr = unsinged char */
   DLEN_t   len;    /* data length */
   int      Desc;  /* descriptor */

   int i; /* fix: for loop */

  DEBUG("CpCoreMsgReceive\n");
#if   CP_SMALL_CODE == 0
   //--- check if channel is valid ----------------------------------
   if (ubChannelV >= CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   ubErrCodeT = CpErr_OK;

   while ((read_reg_bcan(PCAN_SR) & PCAN_SR_RBS) == PCAN_SR_RBS)
   {
	
   	frame_info = read_reg_bcan(PCAN_RXFI);
   
   	if (frame_info & EFF_MESSAGE)
   	{
		/* EXTENDED frame format */
	 	Desc  = ((int)read_reg_bcan(PCAN_RXID1) << 24);
	 	Desc |= ((int)read_reg_bcan(PCAN_RXID2) << 16);
	 	Desc |= ((int)read_reg_bcan(PCAN_RXID3) << 8);
	 	Desc |= (int)read_reg_bcan(PCAN_RXID4);

     		cm    = BCANREG(PCAN_RXEFFD);
     		len   = frame_info & 0x0F;

       		read_data_bcan(Data_ptr,cm,len);
   	}
   	else
   	{
	 	/* BASIC frame format */
	 	Desc  = ((int)read_reg_bcan(PCAN_RXID1) << 8);
	 	Desc |= (int)read_reg_bcan(PCAN_RXID2);
	 	cm    = BCANREG(PCAN_RXSFFD);
	 	len   = frame_info & 0x0F;

       		read_data_bcan(Data_ptr,cm,len);
   	}

   	/* release the receive buffer */
   	write_reg_bcan(PCAN_CMR, PCAN_CMR_RRB);
   

   	/* copy the data to the structure */
    /* XXX use strncpy or so */
   	if (frame_info & EFF_MESSAGE) CpMacSetExtId(&canMsgT,((Desc >> 3) & 0x1FFFFFFF));
   	else CpMacSetStdId(&canMsgT,Desc >> 5);
   	CpMacSetDlc(&canMsgT,frame_info & 0x0F);
   	for (i=0; (i < 8) && (i < (frame_info & 0x0F) );i++) CpMacSetData(&canMsgT,i,Data[i]);
   	for (i=frame_info & 0x0F; i < 8 ;i++) CpMacSetData(&canMsgT,i,0x00);

   	//----------------------------------------------------------------
   	//	test the received message against the software ID filter
   	//
#if   CP_SW_FILTER == 1
   	ubErrCodeT = CpFilterTest(ubChannelV, CpMacGetStdId(&canMsgT) );
   	if (ubErrCodeT) 
		continue;
#endif

	//----------------------------------------------------------------
	//	check for receive handler
	//
	if(CpInt_ReceiveHandler[ubChannelV])
	{
		ubErrCodeT = (* CpInt_ReceiveHandler[ubChannelV] )(ubChannelV, &canMsgT);
		if(ubErrCodeT == CP_CALLBACK_PROCESSED) 
			continue;
	}


	//----------------------------------------------------------------
	//	put message to FIFO
	//
	ubErrCodeT = CpFifoPush(ubChannelV, CP_FIFO_RCV, &canMsgT);
   }
   return (ubErrCodeT);
}


//----------------------------------------------------------------------------//
// CpCoreMsgTransmit()                                                        //
// Write CAN message to controller                               					//
// This method is called if the user put something in the fifo,
// or a transmit interrupt is received.
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreMsgTransmit(_U08 ubChannelV)
{
   _U08           ubErrCodeT;  	// error code value
   CpStruct_CAN   canMsgT;       // CAN message structure

   BYTE_t  frame_info;
   BYTE_t  Data[8];
     BYTE_t* Data_ptr = Data;
   BYTE_t  b;
   DLEN_t  len;
   XADDR_t cm;   /* can message address */
   int     Desc;  /* descriptor */
   int     i; /* fix: for loop */

  DEBUG("CpCoreMsgTransmit\n");
#if   CP_SMALL_CODE == 0
   //--- check if channel is valid ----------------------------------
   if (ubChannelV >= CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   /* check if previous message has been sent already */
   if ((read_reg_bcan(PCAN_SR) & PCAN_SR_TBS) != PCAN_SR_TBS) 
   {
       DEBUG("Last Msg not sent !\n");
       return (CpErr_OK);// we will handle it in the interrupt handler...
   }

   // ------------------------------------
   // From here one, we are sure the message can be transmited.
   // ------------------------------------

   //----------------------------------------------------------------
   //	get message from FIFO
   //
   ubErrCodeT = CpFifoPop(ubChannelV,CP_FIFO_TRM,&canMsgT);
   if (ubErrCodeT) {
       DEBUG("Fifo CpFifoPop error !\n");
       return (ubErrCodeT);
   }

	//----------------------------------------------------------------
	//	check for transmit handler, return result is not checked
	//
	if(CpInt_TransmitHandler[ubChannelV])
	{
		(* CpInt_TransmitHandler[ubChannelV] )(ubChannelV, &canMsgT);
	}

   if (CpMacIsExtended(&canMsgT))
   {
	 /* EXTENDED frame format */
  	 /* Write frame info bytes */
  	 frame_info = EFF_MESSAGE + CpMacGetDlc(&canMsgT);
	   if (CpMacIsRemote(&canMsgT)) frame_info += (1 << 7);
  	 write_reg_bcan(PCAN_TXFI,frame_info);

  	 /* Write descriptor bytes */
	 Desc = CpMacGetExtId(&canMsgT) << 3;
  	 b   = (BYTE_t)(Desc >> 24);
  	 write_reg_bcan(PCAN_TXID1,b);
  	 b   = (BYTE_t)((Desc >> 16) & 0xFF);
  	 write_reg_bcan(PCAN_TXID2,b);
  	 b   = (BYTE_t)((Desc >> 8) & 0xFF);
  	 write_reg_bcan(PCAN_TXID3,b);
  	 b   = (BYTE_t)(Desc & 0xFF);
  	 write_reg_bcan(PCAN_TXID4,b);

  	 /* Fill the transmit buffer */
  	 cm   = BCANREG(PCAN_TXEFFD);
  	 len  = (DLEN_t)(frame_info & 0x0F);
	 for (i=0; i < len;i++) Data[i] = CpMacGetData(&canMsgT,i);

  	 write_data_bcan(cm,Data_ptr,len);
     DEBUG("Extended message Transmitted !\n");
   }
   else
   {
	 /* BASIC frame format */
  	 /* Write frame info bytes */
  	 frame_info = CpMacGetDlc(&canMsgT);
     if (CpMacIsRemote(&canMsgT)) 
         DEBUG("Remote MSG with ");
     if (CpMacIsRemote(&canMsgT)) 
         frame_info += PCAN_FINFO_RTR;
  	 write_reg_bcan(PCAN_TXFI,frame_info);

  	 /* Write descriptor bytes */
	 Desc = CpMacGetStdId(&canMsgT) << 5;
	 DEBUG("COBID in Trans: %X\n", Desc >> 5 );
	 //Desc = 0x202 << 5;
	 //DEBUG("Desc in Trans: %X\n",CpMacGetStdId(&canMsgT));
  	 b   = (BYTE_t)(Desc >> 8);
  	 write_reg_bcan(PCAN_TXID1,b);
  	 b   = (BYTE_t)(Desc & 0xFF);
  	 write_reg_bcan(PCAN_TXID2,b);

  	 /* Fill the transmit buffer */
  	 cm   = BCANREG(PCAN_TXSFFD);
  	 len  = (DLEN_t)(frame_info & PCAN_FINFO_DLC_MASK );
	 for (i=0; i < len;i++) Data[i] = CpMacGetData(&canMsgT,i);

  	 write_data_bcan(cm,Data_ptr,len);
     DEBUG("Standard message Transmitted !\n");
   }
   
   /* Issue transmit request */
   write_reg_bcan(PCAN_CMR, PCAN_CMR_TR);
   //write_reg_bcan(PCAN_CMR, PCAN_CMR_SRR);

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreRegRead()                                                            //
// Read one byte from specified CAN controller register                       //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreRegRead(_U08 ubChannelV, _U16 uwRegAdrV, _U08 * pubValueV)
{
  DEBUG("CpCoreRegRead\n");
#if   CP_SMALL_CODE == 0
   //--- check if channel is valid ----------------------------------
   if (ubChannelV >= CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   uwRegAdrV = uwRegAdrV + CP_CAN_BASE_ADR;

   *pubValueV = read_reg_bcan(uwRegAdrV);

   return (CpErr_OK);
}


//----------------------------------------------------------------------------//
// CpCoreRegWrite()                                                           //
// Write one byte to specified CAN controller register                        //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpCoreRegWrite(_U08 ubChannelV, _U16 uwRegAdrV, _U08 ubValueV)
{
  DEBUG("CpCoreRegWrite\n");
#if   CP_SMALL_CODE == 0
   //--- check if channel is valid ----------------------------------
   if (ubChannelV >= CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif

   uwRegAdrV = uwRegAdrV + CP_CAN_BASE_ADR;

   write_reg_bcan(uwRegAdrV,ubValueV);

   return (CpErr_OK);
}

