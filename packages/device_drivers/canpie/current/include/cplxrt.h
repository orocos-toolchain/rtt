
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


_U32 Cp_PREFIX CpUserAppInit(_U32 channel, _U32 rcvFifoSize, _U32 trmFifoSize, _U32 timeout)
{
  _U32 retval;
  struct { _U32 channel; _U32 rcvFifoSize; _U32 trmFifoSize; _U32 timeout; } arg = { channel, rcvFifoSize, trmFifoSize,  timeout};
  printf("RCV : %d, TRNS: %d\n", arg.rcvFifoSize, arg.trmFifoSize);
  retval = rtai_lxrt(ORONUM_CANPIE_LXRT_IDX, SIZARG, CP_USER_APP_INIT, &arg).i[LOW];
  return retval;
}

_U32 Cp_PREFIX CpUserAppDeInit(_U32 channel)
{
  _U32 retval;
  struct { _U32 channel ; } arg = { channel };
  retval = rtai_lxrt(ORONUM_CANPIE_LXRT_IDX, SIZARG, CP_USER_APP_DE_INIT, &arg).i[LOW];
  return retval;
}


_U32 Cp_PREFIX CpUserBaudrate(_U32 channel, _U32 baud)
{
  _U32 retval;
  struct { _U32 channel; _U32 baud ; } arg = { channel,baud };
  retval = rtai_lxrt(ORONUM_CANPIE_LXRT_IDX, SIZARG, CP_USER_BAUDRATE, &arg).i[LOW];
  return retval;
}


_U32 Cp_PREFIX CpUserFifoClear(_U32 channel, _U32 buffer)
{
  _U32 retval;
  struct { _U32 channel; _U32 buffer; } arg = { channel, buffer };
  retval = rtai_lxrt(ORONUM_CANPIE_LXRT_IDX, SIZARG, CP_USER_FIFO_CLEAR, &arg).i[LOW];
  return retval;
}


_U32 Cp_PREFIX CpUserFilterAll(_U32 channel, _U32 enable)
{
  _U32 retval;
  struct { _U32 channel; _U32 enable; } arg = { channel, enable };
  retval = rtai_lxrt(ORONUM_CANPIE_LXRT_IDX, SIZARG, CP_USER_FILTER_ALL, &arg).i[LOW];
  return retval;
}


_U32 Cp_PREFIX CpUserFilterMsg(_U32 channel, _U32 id, _U32 enable)
{
  _U32 retval;
  struct { _U32 channel; _U32 id; _U32 enable; } arg = { channel, id, enable };
  retval = rtai_lxrt(ORONUM_CANPIE_LXRT_IDX, SIZARG, CP_USER_FILTER_MSG, &arg).i[LOW];
  return retval;
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
  int retval;
  struct { int channel; CpStruct_CAN * msg; unsigned int cp_size; } arg = 
      { channel, msgPtr , sizeof(CpStruct_CAN) };
  return rtai_lxrt(ORONUM_CANPIE_LXRT_IDX, SIZARG, CP_USER_MESSAGE_READ, &arg).i[LOW];
}

 _U32 Cp_PREFIX CpUserMsgWrite(_U32 channel, const CpStruct_CAN * msgPtr)
{
  //CpStruct_CAN local_msg;
  int retval;
  struct { int channel; const CpStruct_CAN* msg; unsigned int cp_size; } arg = { channel, msgPtr , sizeof(CpStruct_CAN) };
  return rtai_lxrt(ORONUM_CANPIE_LXRT_IDX, SIZARG, CP_USER_MESSAGE_WRITE, &arg).i[LOW];
}

#undef SIZARG

#endif /* !__KERNEL__ */


#ifdef __cplusplus
}
#endif // "C"

#endif
