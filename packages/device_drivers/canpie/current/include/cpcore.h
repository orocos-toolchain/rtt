/*****************************************************************************\
*  CANpie                                                                     *
*                                                                             *
*  File        : cpcore.h                                                     *
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


#ifndef  _CANpie_Core_
#define  _CANpie_Core_

#include "compiler.h"
#include "cpconst.h"


/*-----------------------------------------------------------------------------
**    Online documentation for Doxygen
*/

/*!   \file    cpcore.h
**    \brief   CANpie core functions
**
**    The user functions always call the core functions, they never access the
**    hardware directly. That means the user functions do not have to be
**    modified when implementing the CANpie on an existing hardware. A typical
**    user function is the writing of a CAN message to the FIFO buffer
**    (e.g. CpUserMsgWrite() ).
**
*/

/*------------------------------------------------------------------------
** External variables (defined in the cpcore.c file)
** for include purpose in other implementation files (e.g. cpuser.c)
**
*/

#ifdef   __C51__
extern CpStruct_BitTimingValue code CpStruct_BitTimingTable[];
#else
extern const CpStruct_BitTimingValue   CpStruct_BitTimingTable[];
#endif

extern _U32 (* CpInt_ReceiveHandler[]) (_U32, CpStruct_CAN *);
extern _U32 (* CpInt_TransmitHandler[]) (_U32, CpStruct_CAN *);
extern _U32 (* CpInt_ErrorHandler[]) (_U32);

extern _U16 CpVar_InitFlag;


/*-------------------------------------------------------------------------
** A driver with only one channel and small memory resources does not need
** the 'channel' parameter.
** The definition CP_SMALL_CODE is checked for the value '1' and the
** function prototypes are converted then. The function call in the
** application stays the same (with 'channel' parameter).
**
*/
#if   CP_SMALL_CODE == 1

#define  CpCoreAllocBuffer(CH, A, B)         CpCoreAllocBuffer(A, B)
#define  CpCoreBaudrate(CH, A, B, C)         CpCoreBaudrate(A, B, C)
#define  CpCoreBufferData(CH, A)             CpCoreBufferData(A)
#define  CpCoreBufferDLC(CH, A, B)           CpCoreBufferData(A, B)
#define  CpCoreBufferSend(CH, A)             CpCoreBufferSend(A)
#define  CpCoreBufferTransmit(CH, A)         CpCoreBufferTransmit(A)
#define  CpCoreCANMode(CH, A)                CpCoreCANMode(A)
#define  CpCoreCANState(CH)                  CpCoreCANState()
#define  CpCoreDeAllocBuffer(CH, A)          CpCoreDeAllocBuffer(A)
#define  CpCoreDeInitDriver(CH)              CpCoreDeInitDriver()
#define  CpCoreFilterAll(CH, A)              CpCoreFilterAll(A)
#define  CpCoreFilterMsg(CH, A, B)           CpCoreFilterMsg(A, B)
#define  CpCoreHDI(CH, A)                    CpCoreHDI(A)
#define  CpCoreInitDriver(CH)                CpCoreInitDriver()

#define  CpCoreMsgReceive(CH)                CpCoreMsgReceive()
#define  CpCoreMsgTransmit(CH)               CpCoreMsgTransmit()
#define  CpCoreRegRead(CH, A, B)             CpCoreRegRead(A, B)
#define  CpCoreRegWrite(CH, A, B)            CpCoreRegWrite(A, B)

#endif


/*-------------------------------------------------------------------------
** Function Prototypes
*/


/*!
** \brief   Allocate buffer in FullCAN controller
** \param   ubChannelV     CAN channel of the hardware
** \param   msg            Pointer to a CAN message structure, the field
**                         v_MsgFlags holds the number of the message
**                         buffer inside the CAN controller
** \param   direction      Direction of message (receive or transmit)
**                         <ul>
**                         <li>CP_BUFFER_DIR_RX: receive
**                         <li>CP_BUFFER_DIR_TX: transmit
**                         </ul>
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** This function allocates the message buffer in a FullCAN controller
** (e.g. AN82527). The number of the message buffer inside the
** FullCAN controller is coded via the field v_MsgFlags in the CAN
** message structure. For message direction "Transmit" the function
** CpCoreBufferTransmit() has to be called. For message direction
** "Receive" an interrupt handler has to be configured via a
** function call to CpUserIntFunctions().
**
*/
#if   CP_FULL_CAN == 1
Cp_EXPORT _U08 Cp_PREFIX   CpCoreAllocBuffer (  _U08 ubChannelV,
																CpStruct_CAN * msg,
                                                _U08 direction);
#endif

/*!
** \brief   Set baudrate of CAN controller
** \param   ubChannelV     CAN channel of the hardware
** \param   btr0           Value for Bit Timing Register 0
** \param   btr1           Value for Bit Timing Register 1
** \param   sjw            Value for Synchronisation Jump Width
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** This function directly writes to the bit timing registers of the CAN
** controller. It is called by CpUserBaudrate() with pre-defined values
** for common baudrates.
**
*/
Cp_EXPORT _U08 Cp_PREFIX   CpCoreBaudrate(_U08 ubChannelV,
														_U08 btr0, _U08 btr1,
                                          _U08 sjw);


/*!
** \brief   Get Pointer to data fields
** \param   ubChannelV     CAN channel of the hardware
** \param   ubBufferV      Buffer number
**
** \return  Pointer to the first data byte of a CAN message
**
** This function is the fastest method to get a pointer to the
** data bytes of a CAN message. It is used in combination with
** the function CpCoreBufferSend(). The function returns a pointer
** to the first data byte inside a FullCAN message buffer. All further
** bytes can be accessed by incrementing the pointer. If the function
** call fails, the value 0L is returned.
** buffer has to be configured by a call to CpCoreAllocBuffer() before.
** In contrast to the function CpCoreBufferTransmit() no data is
** copied, which makes this function quite fast. The following
** example demonstrates the access to the data bytes of a CAN message.
** \code
**  _U08 *   pDataT;     // pointer to data
**
** //--- get pointer to data of message buffer 1 ----------
** pDataT  = CpCoreBufferData(CP_CHANNEL_1, CP_BUFFER_1);
**
** *pDataT = 0x11;      // byte 0: set to 11hex
** pDataT++;            // point to next byte
** *pDataT = 0x22;      // byte 1: set to 22hex
**
** //--- send this message out ----------------------------
** CpCoreBufferSend(CP_CHANNEL_1, CP_BUFFER_1);
** \endcode
**
*/
#if   CP_FULL_CAN == 1
Cp_EXPORT _U08 * Cp_PREFIX CpCoreBufferData(_U08 ubChannelV, _U08 ubBufferV);
#endif


/*!
** \brief   Change DLC of specified buffer
** \param   ubChannelV     CAN channel of the hardware
** \param   ubBufferV      Buffer number
**	\param	ubDlcV			New Data Length Code
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** This function changes the Data Length Code (DLC) of the selected buffer
**	ubBufferV. The DLC must be in the range from 0 to 8.
*/
#if   CP_FULL_CAN == 1
Cp_EXPORT _U08 Cp_PREFIX 	CpCoreBufferDLC(	_U08 ubChannelV, _U08 ubBufferV,
															_U08 ubDlcV);
#endif


/*!
** \brief   Send message out of FullCAN buffer
** \param   ubChannelV     CAN channel of the hardware
** \param   ubBufferV      Buffer number
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** This function will send a message out of a FullCAN buffer. The
** buffer has to be configured by a call to CpCoreAllocBuffer() before.
** In contrast to the function CpCoreBufferTransmit() no data is
** copied, which makes this function quite fast.
**
*/
#if   CP_FULL_CAN == 1
Cp_EXPORT _U08 Cp_PREFIX   CpCoreBufferSend(_U08 ubChannelV, _U08 ubBufferV);
#endif


/*!
** \brief   Transmit message from a FullCAN buffer
** \param   ubChannelV     CAN channel of the hardware
** \param   msg            Pointer to a CAN message structure, the
**                         field v_MsgFlags holds the number of the
**                         message buffer inside the CAN controller
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** This function will transmit a message in a FullCAN buffer. The
** buffer has to be configured by a call to CpCoreAllocBuffer() before.
** Only the data length code and the data fields are copied to the
** message buffer by this function call.
**
*/
#if   CP_FULL_CAN == 1
Cp_EXPORT _U08 Cp_PREFIX   CpCoreBufferTransmit(_U08 ubChannelV,
																CpStruct_CAN * msg);
#endif


/*!
** \brief   Set state of CAN controller
** \param   ubChannelV     CAN channel of the hardware
** \param   mode           Mode of the controller
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** This function changes the operating mode of the CAN controller.
** Possible values for mode are:
** \li CP_MODE_STOP - stop the CAN controller
** \li CP_MODE_START - start the CAN controller
**
*/
Cp_EXPORT _U08 Cp_PREFIX   CpCoreCANMode(_U08 ubChannelV, _U08 mode);


/*!
** \brief   Retrive state of CAN controller
** \param   ubChannelV     CAN channel of the hardware
** \return  Error code taken from the CpErr enumeration
**
** This function returns the present state of the CAN controller. Possible
** return codes are:
** \li CpErr_OK: CAN controller is active, no error conditions occurred
** \li CpErr_BUS_PASSIVE: CAN controller is in bus passive state
** \li CpErr_BUS_OFF
** \li CpErr_BUS_WARNING
**
**
*/
Cp_EXPORT _U08 Cp_PREFIX   CpCoreCANState(_U08 ubChannelV);


/*!
** \brief   Free message buffer of FullCAN controller
** \param   ubChannelV     CAN channel of the hardware
** \param   msg_buffer     requested message buffer of the CAN controller
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
*/
#if   CP_FULL_CAN == 1
Cp_EXPORT _U08 Cp_PREFIX   CpCoreDeAllocBuffer(	_U08 ubChannelV,
																_U08 msg_buffer);
#endif

/*!
** \brief   De-Initialize the CAN driver
** \param   ubChannelV     CAN channel of the hardware
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** The implementation of this function is dependant on the operating
** system. Typical tasks might be:
** <ul>
** <li>clear the interrupt vector / routine
** <li>close all open paths to the hardware
** </ul>
**
*/
Cp_EXPORT _U08 Cp_PREFIX   CpCoreDeInitDriver(_U08 ubChannelV);


/*!
** \brief   Set global hardware filter for CAN messages
** \param   ubChannelV     CAN channel of the hardware
** \param   enable         TRUE for accepting all identifiers,
**                         FALSE for rejecting all identifiers
**
** \return  Error code taken from the CpErr enumeration. Possible
**          return values are:
**          <ul>
**          <li>CpErr_NOT_SUPPORTED - Function is not supported by the driver
**          <li>CpErr_OK - Function returned without error condition
**          </ul>
**
** This function sets up a hardware CAN identifier filter. The filter
** might be a firmware on an intelligent PC card.
**
*/
Cp_EXPORT _U08 Cp_PREFIX   CpCoreFilterAll(_U08 ubChannelV, _BIT enable);


/*!
** \brief Hardware filter for CAN messages
** \param   ubChannelV     CAN channel of the hardware
** \param   id             value of the identifier
** \param   enable         TRUE for accepting an identifier,
**                         FALSE for rejecting an identifier
**
** \return  Error code taken from the CpErr enumeration. Possible
**          return values are:
**          <ul>
**          <li>CpErr_NOT_SUPPORTED - Function is not supported by the driver
**          <li>CpErr_OK - Function returned without error condition
**          </ul>
**
** This function sets a filter for CAN messages. If the flag <b>enable</b>
** is set to TRUE, the CAN identifier <b>id</b> is accepted. If the flag
** <b>enable</b> is set to FALSE, the reception of messages with the
** identifier <b>id</b> is not possible. Please notice that this function
** only works for standard messages (2.0A, 11 bit identifier).
**
*/
Cp_EXPORT _U08 Cp_PREFIX   CpCoreFilterMsg(	_U08 ubChannelV, _U16 id,
															_BIT enable);


/*!
** \brief   Get hardware description information
** \param   ubChannelV     CAN channel of the hardware
** \param   hdi            Pointer to the Hardware Description Interface
**                         structure (CpStruct_HDI)
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** This function retrieves information about the used hardware.
**
*/
Cp_EXPORT _U08 Cp_PREFIX   CpCoreHDI(_U08 ubChannelV, CpStruct_HDI* hdi);


/*!
** \brief   Initialize the CAN driver
** \param   ubChannelV     CAN channel of the hardware
**
** \return  Error code taken from the CpErr enumeration. Possible
**          return values are:
**          <ul>
**          <li>CpErr_HARDWARE - Hardware failure occured, initialisation
**              is not possible
**          <li>CpErr_INIT_FAIL - Software failure occured, initialisation
**              is not possible
**          <li>CpErr_OK - Function returned without error condition
**          </ul>
**
** The implementation of this function is dependant on the operating
** system. Typical tasks might be:
** <ul>
** <li>open a path/file to the hardware
** <li>install an interrupt vector
** </ul>
**
*/
Cp_EXPORT _U08 Cp_PREFIX   CpCoreInitDriver(_U08 ubChannelV);


/*!
** \brief   CAN Interrupt Request (IRQ) Handler
**
** The CAN IRQ Handler is responsible for calling core functions
** and sending signals (events) to the application whenever an
** interrupt signal of the CAN controller occurs. This function
** is not intended to be used by the application program.
** <b>
** Do not call this function by the application program. The
** implementation of this function is system dependend.
** </b>
**
*/
void              CpCoreIntHandler(void);
//void CpCoreIntHandler( int irq, void* dev_id, struct pt_regs* regs );

/*!
** \brief   Read a CAN message from controller
** \param   ubChannelV     CAN channel of the hardware
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** This function reads the receive buffer of a CAN controller and
** builds a CAN message from that information. The function is also
** responsible to generate the time stamp for the CAN message. The
** complete CAN message is then transferred to the Receive FIFO.
** <b>
** Do not call this function by the application program. The
** implementation of this function is system dependent.
** </b>
**
*/
Cp_EXPORT _U08 Cp_PREFIX   CpCoreMsgReceive(_U08 ubChannelV);


/*!
** \brief   Transmit a CAN message
** \param   ubChannelV     CAN channel of the hardware
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** This function gets the next CAN message out of the Transmit
** FIFO and writes it to the appropriate registers of the CAN
** controller.
** <b>
** Do not call this function by the application program. The
** implementation of this function is system dependent.
** </b>
**
*/
Cp_EXPORT _U08 Cp_PREFIX   CpCoreMsgTransmit(_U08 ubChannelV);


/*!
** \brief   Read from CAN controller register
** \param   ubChannelV     CAN channel of the hardware
** \param   regAdr         Register address relative to the base address
**                         of the CAN controller.
** \param   valuePtr       Pointer to a variable of type BYTE, which will
**                         hold the result of the read process
**
** \return  Error code taken from the CpErr enumeration. Possible
**          return values are:
**          <ul>
**          <li>CpErr_CHANNEL - Channel number is out of range
**          <li>CpErr_REGISTER - Register address is out of range
**          <li>CpErr_SUPPORTED - Function is not supported
**          <li>CpErr_OK - Function returned without error condition
**          </ul>
**
** This function will read a value from a CAN controller register. The
** first register starts at address 0.
**
*/
Cp_EXPORT _U08 Cp_PREFIX   CpCoreRegRead(	_U08 ubChannelV, _U16 regAdr,
														_U08 * valuePtr);


/*!
** \brief   Write to CAN controller register
** \param   ubChannelV     CAN channel of the hardware
** \param   regAdr         Register address relative to the base address
**                         of the CAN controller.
** \param   value          Value to write
**
** \return  Error code taken from the CpErr enumeration. Possible
**          return values are:
**          <ul>
**          <li>CpErr_CHANNEL - Channel number is out of range
**          <li>CpErr_REGISTER - Register address is out of range
**          <li>CpErr_SUPPORTED - Function is not supported
**          <li>CpErr_OK - Function returned without error condition
**          </ul>
**
** This function will write a value to a CAN controller register. The
** first register starts at address 0.
**
*/
Cp_EXPORT _U08 Cp_PREFIX   CpCoreRegWrite(_U08 ubChannelV, _U16 regAdr,
														_U08 value);



#endif   /* _CANpie_Core_ */
