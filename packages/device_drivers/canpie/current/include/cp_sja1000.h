/*****************************************************************************\
*  CANpie                                                                     *
*                                                                             *
*  File        : cp_sja_1000.h                                                *
*  Description : Register definitions for SJA 1000                            *
*  Author      : Behzad Dashtkian                                             *
*  e-mail      :                                                              *
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
*  1.0      02.11.2000  Initial version                                 BD    *
*                                                                             *
*                                                                             *
\*****************************************************************************/

#ifndef  _CANpie_Core_SJA1000_
#define  _CANpie_Core_SJA1000_


//-------------------------------------------------------------------
// for DOS/Windows access to ISA bus
#ifdef   _MSC_VER
#include <conio.h>
#endif


//-------------------------------------------------------------------
// Register address of the CAN_Controller SJA1000
enum cRegSJA1000  {
     eREG_CONTROL = 0,
     eREG_COMMAND,
     eREG_STATUS,
     eREG_INTERRUPT,
     eREG_ACC_CODE,        // Acceptance Code
     eREG_ACC_MASK,        //Acceptance Mask 
     eREG_TIMING0,         //Bus Timing 0 
     eREG_TIMING1,         //Bus Timing 1 
     eREG_OUTPUTCTRL,      //Output Control 

     eREG_XMTID = 10,      //Transmit Buffer : Identifier
     eREG_XMTLEN,          // RTR Bit and Data Length Code
     eREG_XMTBUF,          //BYTE1 of data field

     eREG_RCVID = 20,      //Receive Buffer : Identifier
     eREG_RCVLEN,          //RTR Bit and Data Length Code 
     eREG_RCVBUF,          //BYTE1 of data field

     eREG_MAX = 30
};


//--- Control Register ---------
#define  CR_RR_BIT      0x01
#define  CR_RIE_BIT     0x02
#define  CR_TIE_BIT     0x04
#define  CR_EIE_BIT     0x08
#define  CR_OIE_BIT     0x10

//--- Command Register ---------
#define  CMR_TR_BIT     0x01
#define  CMR_AT_BIT     0x02
#define  CMR_RRB_BIT    0x04
#define  CMR_CDO_BIT    0x08
#define  CMR_GTS_BIT    0x10

//--- Status Register -----------
#define  SR_RBS_BIT     0x01
#define  SR_DOS_BIT     0x02
#define  SR_TBS_BIT     0x04
#define  SR_TCS_BIT     0x08
#define  SR_RS_BIT      0x10
#define  SR_TS_BIT      0x20
#define  SR_ES_BIT      0x40
#define  SR_BS_BIT      0x80

//--- Interrupt Register --------
#define  IR_RI_BIT      0x01
#define  IR_TI_BIT      0x02
#define  IR_EI_BIT      0x04
#define  IR_DOI_BIT     0x08
#define  IR_WUI_BIT     0x10

//--- Output Control Register ---
#define  OC_OCMODE0_BIT 0x01
#define  OC_OCMODE1_BIT 0x02
#define  OC_OCPOL0_BIT  0x04
#define  OC_OCTN0_BIT   0x08
#define  OC_OCTP0_BIT   0x10
#define  OC_OCPOL1_BIT  0x20
#define  OC_OCTN1_BIT   0x40
#define  OC_OCTP1_BIT   0x80

//---
// Macro definition for read/write access on registers
//

#ifdef   _MSC_VER
#define CpMacRegRead(REG)           (_U08) (_inp(CP_CAN_BASE_ADR + REG))
#define CpMacRegWrite(REG, VAL)     _outp((_U16)(CP_CAN_BASE_ADR + REG), (_U08)VAL)
#endif

#ifdef   __ICCAVR__
#define CpMacRegRead(REG)           *((unsigned char *)(CP_CAN_BASE_ADR + REG))
#define CpMacRegWrite(REG, VAL)     *((unsigned char *)(CP_CAN_BASE_ADR + REG)) = VAL
#endif


#endif