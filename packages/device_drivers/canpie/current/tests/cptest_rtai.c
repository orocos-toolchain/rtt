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
#include "cpmacro.h"

#include <rtai.h>
#include <rtai_sched.h>
#include <linux/kernel.h>
#include <linux/module.h>

RT_TASK rt_task;
void task(int i);

/*------------------------------------------------------------------------
**	printMsgCAN()
**
*/
void printMsgCAN(CpStruct_CAN * msg)
{
	_U08	cnt;
   _U08	dlc;

	//---	identifier -----------------------------
	printk("ID=%04X "	, CpMsgGetExtId(msg) );

	//--- DLC ------------------------------------
	dlc = CpMsgGetDlc(msg);
	printk("DLC=%01X ", dlc);

	//---	data -----------------------------------
	if(dlc) printk("DATA= ");
	for (cnt = 0; cnt < dlc; cnt++) {
		printk("%02X ", CpMsgGetData(msg, cnt) );
	}

	printk("\n");
}


/*------------------------------------------------------------------------
**	printHDI()
**
*/
void printHDI(CpStruct_HDI * hdi)
{
	printk("\n");
	printk("--------------------------------------------------\n");
	printk("# Hardware Description Interface                  \n");
	printk("CANpie Version   : %d.%d  \n",	hdi->v_VersionMajor,
														hdi->v_VersionMinor);
		
	printk("Driver Name      : %s \n", hdi->v_DriverName);
	printk("Version          : %s \n", hdi->v_VersionNumber);

	printk("CAN Controller   : ");
	switch(hdi->v_ControllerType) {
	case CP_CC_82C200:
		printk("Philips 82C200 \n");
		break;

	case CP_CC_SJA1000:
		printk("Philips SJA1000 \n");
		break;

	case CP_CC_AN82527:
		printk("Intel AN82527 \n");
		break;

	default:
		printk("unknown \n");
		break;
	}

	printk("Frame Type       : ");
	switch( (hdi->v_SupportFlags) & 0x03) {
	case 0:
		printk("2.0 A\n");
		break;
	case 1:
		printk("2.0 B passive\n");
		break;
	case 2:
		printk("2.0 B\n");
		break;
   default:
		printk("unknown\n");
		break;
	}

	printk("FullCAN          : ");
	if ( (hdi->v_SupportFlags) & 0x04 ) printk("yes\n");
	else printk ("no\n");

	printk("Interrupt-Handler: ");
	if ( (hdi->v_SupportFlags) & 0x08 ) printk("yes\n");
	else printk ("no\n");

	printk("ID-Filter        : ");
	if ( (hdi->v_SupportFlags) & 0x10 ) printk("yes\n");
	else printk ("no\n");

	printk("Message Timestamp: ");
	if ( (hdi->v_SupportFlags) & 0x20 ) printk("yes\n");
	else printk ("no\n");

	printk("User Data Field  : ");
	if ( (hdi->v_SupportFlags) & 0x40 ) printk("yes\n");
	else printk ("no\n");

}

_U08 TransCallBack(CpStruct_CAN * msg)
{
   printk("\n   callback transmit...\n");
   return CP_CALLBACK_PUSH_FIFO;
}


_U08 RecCallBack(CpStruct_CAN * msg)
{
   printk("\n   callback receive...\n");
   return CP_CALLBACK_PUSH_FIFO;
}

/*------------------------------------------------------------------------
**	main()
**
*/
int init_module(void)
{
	_U08				status;
	CpStruct_CAN	msg1, msg2, msg3, msg4;
	CpStruct_CAN	fifoMsg;
	CpStruct_HDI   myHdi;


	//--- print startup message --------------------------------------
	
	printk("--------------------------------------------------\n");
	printk("+ Testprogram for CANpie                         +\n");
	printk("+ Version: 0.3                                   +\n");
	printk("--------------------------------------------------\n");

	
	//--- requesting Hardware Description Interface ------------------
	CpCoreHDI(0, &myHdi);
	printHDI(&myHdi);

	//--- start the initialization routine ---------------------------
	printk("\n");
	printk("Initialize: CpUserAppInit() \n");
	status =	CpUserAppInit(CP_CHANNEL_1, 6, 6, 10);
	printk("Status: %d \n", status);

   //--- callback function ------------------------------------------
   CpUserIntFunctions(CP_CHANNEL_1, RecCallBack, TransCallBack, NULL);
   
   //---	init the CAN message ---------------------------------------
   CpMacMsgClear(&msg1);
   CpMacSetStdId(&msg1,0x0);
   CpMacSetDlc(&msg1, 0x02);
   CpMacSetData(&msg1,0, 0x01);
   CpMacSetData(&msg1,1, 0x00);
   
   
   status = CpUserMsgWrite(CP_CHANNEL_1, &msg1);
   if (status != CpErr_OK)
	  printk("0  MsgWrite error %d\n",status);
   rt_busy_sleep(10000000);
   status = CpUserMsgWrite(CP_CHANNEL_1, &msg1);
   if (status != CpErr_OK)
	  printk("1  MsgWrite error %d\n",status);
   
   rt_set_oneshot_mode();
   start_rt_timer(0);
   
    rt_task_init (&rt_task, task, 0, 2000, 5, 1, 0);
    rt_task_make_periodic_relative_ns(&rt_task, 1000000, nano2count(10*1000*1000)); 
    rt_task_resume (&rt_task);
	    
    return 0;
}

void task(int i)
{
	_U08				status;
	CpStruct_CAN	msg1, msg2, msg3, msg4;
	CpStruct_CAN	fifoMsg;
	
	int counter = 0;

	while(1)
	{
		counter++;
/**
 * RTR
 */
   if (counter == 1000)
   {
   CpMacMsgClear(&msg2);
   CpMacSetStdId(&msg2,0x602);
   CpMacSetDlc(&msg2, 0x08);
   CpMacSetData(&msg2,0, 0x23);
   CpMacSetData(&msg2,1, 0x02);
   CpMacSetData(&msg2,2, 0x18);
   CpMacSetData(&msg2,3, 0x01);
   CpMacSetData(&msg2,4, 0x82);
   CpMacSetData(&msg2,5, 0x03);
   CpMacSetData(&msg2,6, 0x00);
   CpMacSetData(&msg2,7, 0x00);
   status = CpUserMsgWrite(CP_CHANNEL_1, &msg2);
   if (status != CpErr_OK)
	  printk("2  MsgWrite error %d\n",status);
   }
   if (counter == 2000)
   {
   CpMacMsgClear(&msg3);
   CpMacSetStdId(&msg3,0x202);
   CpMacSetDlc(&msg3, 0x01);
   CpMacSetData(&msg3,0, 0x01);
   status = CpUserMsgWrite(CP_CHANNEL_1, &msg3);
   if (status != CpErr_OK)
	  printk("3  MsgWrite error %d\n",status);
   }
   if (counter == 4000)
   {
   CpMacMsgClear(&msg3);
   CpMacSetStdId(&msg3,0x202);
   CpMacSetDlc(&msg3, 0x01);
   CpMacSetData(&msg3,0, 0x00);
   status = CpUserMsgWrite(CP_CHANNEL_1, &msg3);
   if (status != CpErr_OK)
	  printk("3  MsgWrite error %d\n",status);
   }
   if (counter == 3000)
   {
   CpMacMsgClear(&msg3);
   CpMacSetStdId(&msg3,0x602);
   CpMacSetDlc(&msg3, 0x08);
   CpMacSetData(&msg3,0, 0x2F);
   CpMacSetData(&msg3,1, 0x02);
   CpMacSetData(&msg3,2, 0x18);
   CpMacSetData(&msg3,3, 0x02);
   CpMacSetData(&msg3,4, 0x04);
   CpMacSetData(&msg3,5, 0x00);
   CpMacSetData(&msg3,6, 0x00);
   CpMacSetData(&msg3,7, 0x00);
   status = CpUserMsgWrite(CP_CHANNEL_1, &msg3);
   if (status != CpErr_OK)
	  printk("3  MsgWrite error %d\n",status);
   }
   //---	read message from buffer -----------------------------------
   //printk("Reading messages from buffer...\n");

   do {
	   status = CpUserMsgRead(CP_CHANNEL_1, &fifoMsg);
	   //printk("Status: %d \n", status);
	   if (status == CpErr_OK)
		   printMsgCAN(&fifoMsg);
   } while (status == CpErr_OK);

	rt_task_wait_period();
   }	
	
}


void cleanup_module(void)
{
        _U08                            status;
	CpStruct_CAN	fifoMsg;

	printk("Reading messages from buffer...\n");

   do {
	   status = CpUserMsgRead(CP_CHANNEL_1, &fifoMsg);
	   printk("Status: %d \n", status);
	   if (status == CpErr_OK)
		   printMsgCAN(&fifoMsg);
   } while (status == CpErr_OK);

   stop_rt_timer();
   rt_task_delete(&rt_task);
	
	//--- remove the driver ------------------------------------------
	printk("De-Initialize: CpUserAppDeInit() \n");
	status =	CpUserAppDeInit(0);
	printk("Status: %d \n", status);


}













