
#ifndef CP_LXRT_H
#define CP_LXRT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pkgconf/device_drivers_canpie.h"

#undef DECLARE
#define DECLARE static inline

#ifdef __KERNEL__
#include "cpuser.h"
#endif

#define CP_USER_MESSAGE_READ     0
#define CP_USER_MESSAGE_WRITE    1
#define CP_USER_APP_INIT         2
#define CP_USER_APP_DE_INIT      3
#define CP_USER_BAUDRATE         4
#define CP_USER_FIFO_CLEAR       5
#define CP_USER_FILTER_ALL       6
#define CP_USER_FILTER_MSG       7
#define CP_USER_INT_FUNCTIONS    8

#ifndef __KERNEL__

#include <stdarg.h>
#include <rtai_lxrt_user.h>

union rtai_lxrt_t rtai_lxrt(short int dynx, short int lsize, int srq, void *arg);

#ifndef SIZARG
#define SIZARG sizeof(arg)
#endif

    // hack until C++ inlining is fixed
#undef DECLARE
#define DECLARE static inline 


_U08 Cp_PREFIX CpUserAppInit(_U08 channel, _U16 rcvFifoSize, _U16 trmFifoSize, _U16 timeout)
{
  struct { _U08 channel, _U16 rcvFifoSize, _U16 trmFifoSize, _U16 timeout } arg = { channel, rcvFifoSize, trmFifoSize,  timeout};
  retval = rtai_lxrt(ORONUM_CANPIE_LXRT_IDX, SIZARG, CP_USER_APP_INIT, &arg).i[LOW];
  return retval;
}

_U08 Cp_PREFIX CpUserAppDeInit(_U08 channel);
{
  struct { _U08 channel } arg = { channel };
  retval = rtai_lxrt(ORONUM_CANPIE_LXRT_IDX, SIZARG, CP_USER_APP_DE_INIT, &arg).i[LOW];
  return retval;
}


_U08 Cp_PREFIX CpUserBaudrate(_U08 channel, _U08 baud);
{
  struct { _U08 channel, _U08 baud } arg = { channel,baud };
  retval = rtai_lxrt(ORONUM_CANPIE_LXRT_IDX, SIZARG, CP_USER_BAUDRATE, &arg).i[LOW];
  return retval;
}


_U08 Cp_PREFIX CpUserFifoClear(_U08 channel, _U08 buffer);
{
  struct { _U08 channel, _U08 buffer } arg = { channel, buffer };
  retval = rtai_lxrt(ORONUM_CANPIE_LXRT_IDX, SIZARG, CP_USER_FIFO_CLEAR, &arg).i[LOW];
  return retval;
}


_U08 Cp_PREFIX CpUserFilterAll(_U08 channel, _BIT enable);
{
  struct { _U08 channel, _BIT enable } arg = { channel, enable };
  retval = rtai_lxrt(ORONUM_CANPIE_LXRT_IDX, SIZARG, CP_USER_FILTER_ALL, &arg).i[LOW];
  return retval;
}


_U08 Cp_PREFIX CpUserFilterMsg(_U08 channel, _U16 id, _BIT enable);
{
  struct { _U08 channel, _U16 id, _BIT enable } arg = { channel, id, enable };
  retval = rtai_lxrt(ORONUM_CANPIE_LXRT_IDX, SIZARG, CP_USER_FILTER_MSG, &arg).i[LOW];
  return retval;
}

_U08 Cp_PREFIX CpUserIntFunctions(  _U08 channel,
                                    _U08 (* rx_handler)  (_U08, CpStruct_CAN *),
                                    _U08 (* tx_handler)  (_U08, CpStruct_CAN *),
                                    _U08 (* err_handler) (_U08) )
{
  return !CpErr_OK;
}


DECLARE _U08 Cp_PREFIX CpUserMsgRead(_U08 channel, CpStruct_CAN * msgPtr)
{
  CpStruct_CAN local_msg;
  int retval;
  struct { int channel; CpStruct_CAN * msg; unsigned int cp_size; } arg = 
  { channel, &local_msg , sizeof(CpStruct_CAN) };
  retval = rtai_lxrt(ORONUM_CANPIE_LXRT_IDX, SIZARG, CP_READ_MESSAGE, &arg).i[LOW];
  memcpy(msgPtr, &local_msg, sizeof(CpStruct_CAN) );
  return retval;
}

DECLARE _U08 Cp_PREFIX CpUserMsgWrite(_U08 channel, const CpStruct_CAN * msgPtr)
{
  //CpStruct_CAN local_msg;
  int retval;
  struct { int channel; const CpStruct_CAN * msg; unsigned int cp_size; } arg = 
  { channel, msgPtr , sizeof(CpStruct_CAN) };
  retval = rtai_lxrt(ORONUM_CANPIE_LXRT_IDX, SIZARG, CP_WRITE_MESSAGE, &arg).i[LOW];
  return retval;
}

#undef SIZARG

#endif /* !__KERNEL__ */


#ifdef __cplusplus
}
#endif // "C"

#endif
