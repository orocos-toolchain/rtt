
#ifndef CP_GNU_H
#define CP_GNU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pkgconf/device_drivers_canpie.h"

#include <stdarg.h>

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
