/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:16 CET 2005  apci1032.h 

                        apci1032.h -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
/*
 +-----------------------------------------------------------------------+
 | (C) ADDI-DATA GmbH          Dieselstrasse 3      D-77833 Ottersweier  |
 +-----------------------------------------------------------------------+
 | Tel : +49 (0) 7223/9493-0     | email    : info@addi-data.com         |
 | Fax : +49 (0) 7223/9493-92    | Internet : http://www.addi-data.com   |
 +-----------------------------------------------------------------------+
 | Project   : API APCI1032      | Compiler : GCC                        |
 | Modulname : APCI1032.H        | Version  : 2.96 (SuSE 7.1 - 2.2.18    |
 +-------------------------------+---------------------------------------+
 | Author    : STOLZ, Eric       | Date     : 22.08.2001                 |
 +-----------------------------------------------------------------------+
 | Description : APCI1032 header MAIN                              |
 +-----------------------------------------------------------------------+
 |                             UPDATE'S                                  |
 +-----------------------------------------------------------------------+
 |   Date   |   Author  |          Description of updates                |
 +----------+-----------+------------------------------------------------+
 |          |           |                                                |
 +-----------------------------------------------------------------------+
*/
#include <linux/ioctl.h>

//GENERAL DEFINE

#define APCI1032_BOARD_NAME                      "APCI1032"
#define APCI1032_BOARD_VENDOR_ID                 0x15B8
#define APCI1032_BOARD_DEVICE_ID                 0x1003
#define APCI1032_MAX_BOARD_NBR                   10


#define VOID           void
#define INT            int
#define UINT           unsigned int
#define SHORT          short
#define USHORT         unsigned short
#define CHAR           char
#define BYTE           unsigned char
#define WORD           unsigned int
#define LONG           long
#define ULONG          unsigned long
#define PUSHORT        unsigned short *
#define PINT           int *
#define PUINT          unsigned int *
#define PSHORT         short *
#define PUSHORT        unsigned short *
#define PCHAR          char *
#define PBYTE          unsigned char *
#define PWORD          unsigned int *
#define PLONG          long *
#define PULONG         unsigned long *

#define LOBYTE(W)      (unsigned char)   ((W) & 0xFF)
#define HIBYTE(W)      (unsigned char)   (((W) >> 8) & 0xFF)
#define MAKEWORD(H,L)  (unsigned short) ((L) | ((H) << 8))
#define MAKESHORT(H,L) ((L) | ((H) << 8))
#define LOSHORT(W)     ((W) & 0xFFFF)
#define HISHORT(W)     (((W) >> 16) & 0xFFFF)
#define MAKELONG(H,L)  ((L) | ((H) << 16))
#define LOWORD(W)      (unsigned short) ((W) & 0xFFFF)
#define HIWORD(W)      (unsigned short) (((W) >> 16) & 0xFFFF)
#define MAKEDWORD(H,L) (unsigned int)((L)|( (H)<<16) )

#define CMD_MAX 30  /* Number of ioctl functions */

/************************/
/*  PCI BUS board infos */
/************************/

typedef struct
{
    unsigned int ui_BaseAddress[ 5 ];
    unsigned char b_Interrupt;
    unsigned char b_SlotNumber;
}

str_BaseInformations;

/************************/
/* Hardware board infos */
/************************/

typedef struct
{
    str_BaseInformations s_BaseInformations; /* PCI BUS INFORMATIONS    */
}

str_BoardInformations;

typedef struct
{
    str_BoardInformations s_BoardInformations[ APCI1032_MAX_BOARD_NBR ];
    unsigned char b_NumberOfBoard;
}

str_APCI1032_DriverStructure;

extern str_BoardInformations *BoardInformations;

/* Hardware board information structure */
extern str_APCI1032_DriverStructure s_APCI1032_DriverStructure;

/* IOCTL define part */
#define APCI1032_MAGIC (char) 'A'

#define CMD_APCI1032_CheckAndGetPCISlotNumber   _IOR(APCI1032_MAGIC, 1,BYTE[APCI1032_MAX_BOARD_NBR+1])
#define CMD_APCI1032_SetBoardInformation        _IOW(APCI1032_MAGIC, 2,unsigned short)
#define CMD_APCI1032_GetHardwareInformation     _IOR(APCI1032_MAGIC, 3,str_BaseInformations)

#define CMD_APCI1032_Read1DigitalInput          _IOR(APCI1032_MAGIC, 4,unsigned char)
#define CMD_APCI1032_Read8DigitalInput          _IOR(APCI1032_MAGIC, 5,unsigned char)
#define CMD_APCI1032_Read16DigitalInput         _IOR(APCI1032_MAGIC, 6,unsigned long)
#define CMD_APCI1032_Read32DigitalInput         _IOR(APCI1032_MAGIC, 7,unsigned long)

/* Prototype functions */
int i_APCI1032_CheckAndGetPCISlotNumber ( unsigned char *pb_SlotArray );
int i_APCI1032_SetBoardInformation ( unsigned char b_SlotNumber, int * pi_BoardHandle );
int i_APCI1032_GetHardwareInformation ( int i_BoardHandle, unsigned int * pui_BaseAddress, unsigned char * pb_InterruptNbr, unsigned char * pb_SlotNumber );
int i_APCI1032_CloseBoardHandle ( int i_BoardHandle );

int i_APCI1032_Read1DigitalInput ( int i_BoardHandle, unsigned char b_Channel, unsigned long * pb_ChannelValue );
int i_APCI1032_Read8DigitalInput ( int i_BoardHandle, unsigned char b_Port, unsigned long * pb_PortValue );
int i_APCI1032_Read16DigitalInput ( int i_BoardHandle, unsigned char b_Port, unsigned long * pul_InputValue );
int i_APCI1032_Read32DigitalInput ( int i_BoardHandle, unsigned long * pul_InputValue );
