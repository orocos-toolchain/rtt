/*****************************************************************************\
*  CANpie                                                                     *
*                                                                             *
*  File        : cpfilter.c                                                   *
*  Description : Implementation of a Software filter for Standard CAN frames  *
*                (11 bit identifier). If the definition 'CP_SW_FILTER' is set *
*                to 0 or 2 (no filter / hardware filter), you don't need to   *
*                include this file for compilation.                           *
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
*  0.1      06.09.1999  Initial version                                 UK    *
*  0.2                  - no changes -                                        *
*  0.3                  - no changes -                                        *
*  0.4                  - no changes -                                        *
*  0.5                  - no changes -                                        *
*  0.6      15.06.2000  set to CANpie release 0.6                       UK    *
*  0.7                  - no changes -                                      	*
*  0.8                  - no changes -                                      	*
*                                                                             *
\*****************************************************************************/

#include "rtt/can/cpfilter.h"


//------------------------------------------------------------------------
// check if we need this file, see the cpconfig.h
//
#if   CP_SW_FILTER == 1

//------------------------------------------------------------------------
// For the software filter an array with 2048 bits (i.e. 256 byte * 8 bit)
// is defined for each channel.


_U08           CpVar_Filter[CP_CHANNEL_MAX][256];


//----------------------------------------------------------------------------//
// CpFilterAll()                                                              //
// Enable / disable filter for Standard Frames                                //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpFilterAll(_U08 channel, _BIT enable)
{
   _U08  mask;
   _U08  cnt;


#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if( (channel + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif


   //--- initialize the filter array --------------------------------
   if (enable) mask = 0xFF;
   else mask = 0x00;

   for (cnt = 0; cnt < 255; cnt ++) {
      CpVar_Filter[channel][cnt] = mask;
   }

   return(CpErr_OK);   
}


//----------------------------------------------------------------------------//
// CpFilterMsg()                                                              //
// Enable / disable filter for message with identifier uwIdV                  //
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpFilterMsg(_U08 ubChannelV, _U16 uwIdV, _BIT btEnableV)
{
   _U08  ubMaskT;
   _U08  ubPosByteT;
   _U08  ubPosBitT;


#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if( (ubChannelV + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif


   //--- get position of the bit inside the filter array ------------
   ubPosByteT = (_U08) (uwIdV >> 3);
   ubPosBitT  = (_U08) (uwIdV & 0x00000007);
   ubMaskT    = (_U08) (0x01 << ubPosBitT);

   if(btEnableV)
   {
      CpVar_Filter[ubChannelV][ubPosByteT] |= ubMaskT;
   }
   else
   {
      ubMaskT = (_U08) (~ubMaskT);
      CpVar_Filter[ubChannelV][ubPosByteT] &= ubMaskT;
   }

   return(CpErr_OK);   
}


//----------------------------------------------------------------------------//
// CpFilterTest()                                                             //
// Test message with identifier uwIdV                  								//
//----------------------------------------------------------------------------//
_U08 Cp_PREFIX CpFilterTest(_U08 channel, _U16 id)
{
   _U08  mask;
   _U08  posByte;
   _U08  posBit;


#if   CP_SMALL_CODE == 0
   //--- test the channel number ------------------------------------
   if( (channel + 1) > CP_CHANNEL_MAX) return (CpErr_CHANNEL);
#endif


   //--- get position of the bit inside the filter array ------------
   posByte = (_U08) (id >> 3);
   posBit  = (_U08) (id & 0x00000007);
   mask    = (_U08) (0x01 << posBit);

   //--- check mask with array value --------------------------------
   if( CpVar_Filter[channel][posByte] & mask) return (CpErr_OK);
   
   return (CpErr_GENERIC);
}


#endif

