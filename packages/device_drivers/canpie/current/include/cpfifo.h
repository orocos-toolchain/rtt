/*****************************************************************************\
*  CANpie                                                                     *
*                                                                             *
*  File        : cpfifo.h                                                     *
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


#ifndef  _CANpie_Fifo_
#define  _CANpie_Fifo_


#include "cpconst.h"

/*-----------------------------------------------------------------------------
**    Online documentation for Doxygen
*/

/*!   \file    cpfifo.h
**    \brief   CANpie FIFO functions
**
**    The FIFO functions care for the handling of messages between the user
**    interface (user functions) and the specific hardware implementation.
**    The are currently two implementations for the FIFO: one with dynamic
**    memory allocation (using malloc() and free() functions) and a second
**    with static memory size. The latter is typically used for microcontroller
**    implementations with low memory resources. Selection of the type is done
**    by setting the definition CP_FIFO_TYPE in the file cpconfig.h to the
**    appropriate value. The size of the static FIFOs can be setup in the
**    cpfifo.h file via the definitions CP_FIFO_TRM_SIZE and CP_FIFO_RCV_SIZE.
**
*/


/*-------------------------------------------------------------------------
** A driver with only one channel and small memory resources does not need
** the 'channel' parameter.
** The definition CP_SMALL_CODE is checked for the value '1' and the
** function prototypes are converted then. The function call in the
** application stays the same (with 'channel' parameter).
**
*/
#if   CP_SMALL_CODE == 1

#define  channel  0

#if   CP_FIFO_TYPE == 1
#define  CpFifoSetup(CH, A, B)               CpFifoSetup(A)
#else
#define  CpFifoSetup(CH, A, B)               CpFifoSetup(A, B)
#endif

#define  CpFifoRemove(CH, A)                 CpFifoRemove(A)
#define  CpFifoClear(CH, A)                  CpFifoClear(A)
#define  CpFifoPush(CH, A, B)                CpFifoPush(A, B)
#define  CpFifoPop(CH, A, B)                 CpFifoPop(A, B)

#endif


/*-------------------------------------------------------------------------
** Definitions for static size of FIFOs
** size is given in number of messages
**
*/
#define  CP_FIFO_RCV_SIZE     2
#define  CP_FIFO_TRM_SIZE     8



/*-------------------------------------------------------------------------
** Function prototypes
**
*/


/*!
** \brief   Setup FIFO
** \param   channel        CAN channel of the hardware
** \param   buffer         FIFO type (receive or transmit)
**                         <ul>
**                         <li>CP_FIFO_RCV: receive
**                         <li>CP_FIFO_TRM: transmit
**                         </ul>
** \param   size           size of the FIFO (number of messages)
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** This function reserves memory for the specified FIFO buffer. If
** a static FIFO is used (CP_FIFO_TYPE == 1) the parameter 'size'
** is not evaluated. 
**
*/
_U08 Cp_PREFIX CpFifoSetup(_U08 channel, _U08 buffer, _U16 size);


/*!
** \brief   Remove FIFO
** \param   channel        CAN channel of the hardware
** \param   buffer         FIFO type (receive or transmit)
**                         <ul>
**                         <li>CP_FIFO_RCV: receive
**                         <li>CP_FIFO_TRM: transmit
**                         </ul>
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** This function frees the reserved memory for the specified FIFO 
** buffer. If a static FIFO is used (CP_FIFO_TYPE == 1) this function
** simply returns CpErr_OK. 
**
*/
_U08 Cp_PREFIX CpFifoRemove(_U08 channel, _U08 buffer);


/*!
** \brief   Clear FIFO contents
** \param   channel        CAN channel of the hardware
** \param   buffer         FIFO type (receive or transmit)
**                         <ul>
**                         <li>CP_FIFO_RCV: receive
**                         <li>CP_FIFO_TRM: transmit
**                         </ul>
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** This function clears the specified FIFO buffer. All messages inside
** the FIFO are erased. 
**
*/
_U08 Cp_PREFIX CpFifoClear(_U08 channel, _U08 buffer);


/*!
** \brief   Push message into FIFO
** \param   channel        CAN channel of the hardware
** \param   buffer         FIFO type (receive or transmit)
**                         <ul>
**                         <li>CP_FIFO_RCV: receive
**                         <li>CP_FIFO_TRM: transmit
**                         </ul>
** \param   msgPtr         Pointer to a structure of type CpStruct_CAN
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** Push a message into the FIFO buffer, given by the parameter 'buffer'.
** If the buffer is full, the function will return 'CpErr_FIFO_FULL'.
**
*/
_U08 Cp_PREFIX CpFifoPush(_U08 channel, _U08 buffer, const CpStruct_CAN * msgPtr);


/*!
** \brief   Pop message from FIFO
** \param   channel        CAN channel of the hardware
** \param   buffer         FIFO type (receive or transmit)
**                         <ul>
**                         <li>CP_FIFO_RCV: receive
**                         <li>CP_FIFO_TRM: transmit
**                         </ul>
** \param   msgPtr         Pointer to a structure of type CpStruct_CAN
**
** \return  Error code taken from the CpErr enumeration. If no error
**          occured, the function will return CpErr_OK.
**
** Pop a message from the FIFO buffer, given by the parameter 'buffer'.
** If the buffer is empty, the function will return 'CpErr_FIFO_EMPTY'.
** is not evaluated. 
**
*/
_U08 Cp_PREFIX CpFifoPop(_U08 channel, _U08 buffer, CpStruct_CAN * msg);


#endif   /* _CANpie_Fifo_ */

