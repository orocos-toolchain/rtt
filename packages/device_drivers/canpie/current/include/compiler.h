/*****************************************************************************\
*  CANpie                                                                     *
*                                                                             *
*  File        : compiler.h                                                   *
*  Description : Compiler independent data type definitions                   *
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
*  0.1      04.12.1998  Initial version                                 UK    *
*  0.2      29.04.1999  New data type definitions,                      UK    *
*                       Machine dependant definitions                         *
*  1.0      15.06.2000  Final release                                   UK    *
*  1.1      07.09.2000  New symbol for const/code                       UK    *
*                                                                             *
\*****************************************************************************/

#ifndef  _Compiler_Independent_Data_Types_
#define  _Compiler_Independent_Data_Types_


#ifndef  FALSE
#define  FALSE  0
#endif

#ifndef  TRUE
#define  TRUE   1
#endif



//----------------------------------------------------------//
// Target Machine / Compiler dependant definitions          //
//                                                          //
//----------------------------------------------------------//



/*---------------------------------------------------------
** Microsoft Visual C/C++
**
*/
#ifdef _MSC_VER

#define  Cp_CODE     const
#define  Cp_EXPORT   __declspec(dllexport)  
#define  Cp_PREFIX   __stdcall
//#define  Cp_PREFIX 
#define  _BIT     unsigned char
#define  _U08     unsigned char
#define  _S08     char
#define  _U16     unsigned short
#define  _S16     short
#define  _U32     unsigned long
#define  _S32     long

#endif 
/* End of definition: _MSC_VER
**-------------------------------------------------------*/

/*---------------------------------------------------------
** CVI, National Instruments
**
*/
#ifdef _CVI_

#define  Cp_EXPORT
#define  Cp_PREFIX
#define  _BIT     unsigned char
#define  _U08     unsigned char
#define  _S08     char
#define  _U16     unsigned short
#define  _S16     short
#define  _U32     unsigned long
#define  _S32     long

#endif 
/* End of definition: _CVI_
**-------------------------------------------------------*/

/*---------------------------------------------------------
** LINUX, gcc
**
*/
#ifdef __GNUC__

#define  Cp_CODE     const
#define  Cp_EXPORT
#define  Cp_PREFIX
#define  _BIT     unsigned char
#define  _U08     unsigned char
#define  _S08     char
#define  _U16     unsigned short
#define  _S16     short
#define  _U32     unsigned int
#define  _S32     int

#endif 
/* End of definition: _LINUX_
**-------------------------------------------------------*/

/*---------------------------------------------------------
** Keil C for 8051 cores
**
*/
#ifdef __C51__

#define  Cp_CODE     code
#define  Cp_EXPORT
#define  Cp_PREFIX
#define  _BIT     unsigned char
#define  _U08     unsigned char
#define  _S08     char
#define  _U16     unsigned short
#define  _S16     short
#define  _U32     unsigned long
#define  _S32     long

#endif
/* End of definition: __C51__
**-------------------------------------------------------*/

#ifndef  _U32
#error   Data types are not defined! Please check compiler definition.
#endif

#endif   /* _Compiler_Independent_Data_Types_ */

