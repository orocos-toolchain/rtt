/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:16 CET 2005  cpgnu.h 

                        cpgnu.h -  description
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
 
 

#ifndef CP_GNU_H
#define CP_GNU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pkgconf/device_drivers_canpie.h"

#include <stdarg.h>
#include "compiler.h"

_U32 Cp_PREFIX CpUserAppInit(_U32 channel, _U32 rcvFifoSize, _U32 trmFifoSize, _U32 timeout)
{
    return CpErr_OK;
}

_U32 Cp_PREFIX CpUserAppDeInit(_U32 channel)
{
    return CpErr_OK;
}


_U32 Cp_PREFIX CpUserBaudrate(_U32 channel, _U32 baud)
{
    return CpErr_OK;
}


_U32 Cp_PREFIX CpUserFifoClear(_U32 channel, _U32 buffer)
{
    return CpErr_OK;
}


_U32 Cp_PREFIX CpUserFilterAll(_U32 channel, _U32 enable)
{
    return CpErr_OK;
}


_U32 Cp_PREFIX CpUserFilterMsg(_U32 channel, _U32 id, _U32 enable)
{
    return CpErr_OK;
}

_U32 Cp_PREFIX CpUserIntFunctions(  _U32 channel,
                                    _U32 (* rx_handler)  (_U32, CpStruct_CAN *),
                                    _U32 (* tx_handler)  (_U32, CpStruct_CAN *),
                                    _U32 (* err_handler) (_U32) )
{
  return !CpErr_OK;
}


 _U32 Cp_PREFIX CpUserMsgRead(_U32 channel, CpStruct_CAN * msgPtr)
{
    return !CpErr_OK;
}

 _U32 Cp_PREFIX CpUserMsgWrite(_U32 channel, const CpStruct_CAN * msgPtr)
{
    return CpErr_OK;
}

#ifdef __cplusplus
}
#endif // "C"

#endif
