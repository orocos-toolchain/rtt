/*****************************************************************************\
*	CANpie																	                  *
*																			                     *
*	File			: cptest.c																      *
*	Description :                                                              *
*	Author		: Uwe Koppe                                                    *
*	e-mail		: koppe@microcontrol.net													*
*                                                                         		*
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
*                                                                         		*
*   This program is free software; you can redistribute it and/or modify  		*
*   it under the terms of the GNU General Public License as published by  		*
*   the Free Software Foundation; either version 2 of the License, or     		*
*   (at your option) any later version.                                   		*
*                                                                         		*
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
*																										*
*  History                                                                    *
*  Vers.    Date        Comment                                         Aut.  *
*  -----    ----------  ---------------------------------------------   ----  *
*  0.1      12.02.1999  Initial version                                 UK    *
*  0.2      04.09.1999  Adaption to version 0.3 of the CANpie Doc.      UK    *
*																										*
\*****************************************************************************/

#include "cpcore.h"
#include "cpconst.h"
#include "cpfifo.h"
#include "cpmsg.h"
#include "cpuser.h"

#include <stdio.h>		

/*------------------------------------------------------------------------
**	printMsgCAN()
**
*/
void printMsgCAN(CpStruct_CAN * msg)
{
	_U08	cnt;
   _U08	dlc;

	//---	identifier -----------------------------
	printf("ID=%04d "	, CpMsgGetExtId(msg) );

	//--- DLC ------------------------------------
	dlc = CpMsgGetDlc(msg);
	printf("DLC=%01d ", dlc);

	//---	data -----------------------------------
	if(dlc) printf("DATA= ");
	for (cnt = 0; cnt < dlc; cnt++) {
		printf("%02X ", CpMsgGetData(msg, cnt) );
	}

	printf("\n");
}


/*------------------------------------------------------------------------
**	printHDI()
**
*/
void printHDI(CpStruct_HDI * hdi)
{
	printf("\n");
	printf("--------------------------------------------------\n");
	printf("# Hardware Description Interface                  \n");
	printf("CANpie Version   : %d.%d  \n",	hdi->v_VersionMajor,
														hdi->v_VersionMinor);
		
	printf("Driver Name      : %s \n", hdi->v_DriverName);
	printf("Version          : %s \n", hdi->v_VersionNumber);

	printf("CAN Controller   : ");
	switch(hdi->v_ControllerType) {
	case CP_CC_82C200:
		printf("Philips 82C200 \n");
		break;

	case CP_CC_SJA1000:
		printf("Philips SJA1000 \n");
		break;

	case CP_CC_AN82527:
		printf("Intel AN82527 \n");
		break;

	default:
		printf("unknown \n");
		break;
	}

	printf("Frame Type       : ");
	switch( (hdi->v_SupportFlags) & 0x03) {
	case 0:
		printf("2.0 A\n");
		break;
	case 1:
		printf("2.0 B passive\n");
		break;
	case 2:
		printf("2.0 B\n");
		break;
   default:
		printf("unknown\n");
		break;
	}

	printf("FullCAN          : ");
	if ( (hdi->v_SupportFlags) & 0x04 ) printf("yes\n");
	else printf ("no\n");

	printf("Interrupt-Handler: ");
	if ( (hdi->v_SupportFlags) & 0x08 ) printf("yes\n");
	else printf ("no\n");

	printf("ID-Filter        : ");
	if ( (hdi->v_SupportFlags) & 0x10 ) printf("yes\n");
	else printf ("no\n");

	printf("Message Timestamp: ");
	if ( (hdi->v_SupportFlags) & 0x20 ) printf("yes\n");
	else printf ("no\n");

	printf("User Data Field  : ");
	if ( (hdi->v_SupportFlags) & 0x40 ) printf("yes\n");
	else printf ("no\n");

}

_U08 MeineFunktion(CpStruct_CAN * msg)
{
   printf("Jetzt sind wir in MeineFunktion()..\n");
   return 33;
}

/*------------------------------------------------------------------------
**	main()
**
*/
int main(void)
{
	_U08				status;
	CpStruct_CAN	msg1, msg2, msg3, msg4;
	CpStruct_CAN	fifoMsg;
	CpStruct_HDI   myHdi;


	//--- print startup message --------------------------------------
	
	printf("--------------------------------------------------\n");
	printf("+ Testprogram for CANpie                         +\n");
	printf("+ Version: 0.3                                   +\n");
	printf("--------------------------------------------------\n");

	
	//--- requesting Hardware Description Interface ------------------
	CpCoreHDI(0, &myHdi);
	printHDI(&myHdi);

	//--- start the initialization routine ---------------------------
	printf("\n");
	printf("Initialize: CpUserAppInit() \n");
	status =	CpUserAppInit(CP_CHANNEL_1, 6, 6, 10);
	printf("Status: %d \n", status);

   //--- callback function ------------------------------------------
   CpUserIntFunctions(CP_CHANNEL_1, MeineFunktion, NULL, NULL);

   //---	init the CAN message ---------------------------------------
	msg1.v_MsgId 		= 100;
	msg1.v_MsgFlags 	= 2;
	msg1.v_MsgData[0] = 1;
	msg1.v_MsgData[1] = 3;

	msg2.v_MsgId 		= 200;
	msg2.v_MsgFlags 	= 2;
	msg2.v_MsgData[0] = 2;
	msg2.v_MsgData[1] = 4;

   msg3.v_MsgId 		= 300;
	msg3.v_MsgFlags 	= 3;
	msg3.v_MsgData[0] = 3;
	msg3.v_MsgData[1] = 2;
   msg3.v_MsgData[2] = 1;

	msg4.v_MsgId 		= 440;
	msg4.v_MsgFlags 	= 4;
	msg4.v_MsgData[0] = 1;
	msg4.v_MsgData[1] = 2;
   msg4.v_MsgData[2] = 4;
	msg4.v_MsgData[3] = 8;

	//---	test fifo --------------------------------------------------
	printf("\n");
	printf("Push 8 messages to receive buffer...\n");
	status = CpFifoPush(CP_CHANNEL_1, CP_FIFO_RCV, &msg1);
 	status = CpFifoPush(CP_CHANNEL_1, CP_FIFO_RCV, &msg2);
   status = CpFifoPush(CP_CHANNEL_1, CP_FIFO_RCV, &msg3);
   status = CpFifoPush(CP_CHANNEL_1, CP_FIFO_RCV, &msg4);
   status = CpFifoPush(CP_CHANNEL_1, CP_FIFO_RCV, &msg1);
 	status = CpFifoPush(CP_CHANNEL_1, CP_FIFO_RCV, &msg2);
   status = CpFifoPush(CP_CHANNEL_1, CP_FIFO_RCV, &msg3);
   status = CpFifoPush(CP_CHANNEL_1, CP_FIFO_RCV, &msg4);
 	
	CpUserFilterAll(CP_CHANNEL_1, 0);

	//---	read message from buffer -----------------------------------
	printf("Reading messages from buffer...\n");

   do {
	   status = CpUserMsgRead(CP_CHANNEL_1, &fifoMsg);
	   printf("Status: %d \n", status);
  	   printMsgCAN(&fifoMsg);
   } while (status == CpErr_OK);



	//--- remove the driver ------------------------------------------
	printf("De-Initialize: CpUserAppDeInit() \n");
	status =	CpUserAppDeInit(0);
	printf("Status: %d \n", status);

	return (0);
}
















