


#include <linux/module.h>

#include <linux/config.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/interrupt.h>
#include <asm/io.h>

#include "can/cplxrt.h"
#include <rtai_lxrt.h>
#include <pkgconf/device_drivers_canpie.h>

MODULE_LICENSE("GPL");
//EXPORT_NO_SYMBOLS;

/**
 * The first parameter is the bitmask and denotes what marshalling must
 * happen and if the call is realtime (bit 0), the second parameter is the function
 * to be called.
 */
static struct rt_fun_entry rt_apci_fun[] = {
    [ CP_USER_MESSAGE_READ  ] = { UW1(2,3), CpUserMsgRead },
    [ CP_USER_MESSAGE_WRITE ] = { UR1(2,3), CpUserMsgWrite }, // UR1(2,3)
    [ CP_USER_APP_INIT      ] = { 0, CpUserAppInit },
    [ CP_USER_APP_DE_INIT   ] = { 0, CpUserAppDeInit },
    [ CP_USER_BAUDRATE      ] = { 0, CpUserBaudrate },
    [ CP_USER_FIFO_CLEAR    ] = { 0, CpUserFifoClear },
    [ CP_USER_FILTER_ALL    ] = { 0, CpUserFilterAll },
    [ CP_USER_FILTER_MSG    ] = { 0, CpUserFilterMsg },
    [ CP_USER_INT_FUNCTIONS ] = { 0, CpUserIntFunctions }
};


		
/* init module */
int init_module(void)
{
  if( set_rt_fun_ext_index(rt_apci_fun, ORONUM_CANPIE_LXRT_IDX)) {
    printk("LXRT index %d already in use !\n", ORONUM_CANPIE_LXRT_IDX);
    printk("Recompile canpie with a different index\n");
    return -EACCES;
  }
  printk("CANPie Loaded\n");
  return(0);
}

/*  cleanup module */
void cleanup_module(void)
{
  reset_rt_fun_ext_index(rt_apci_fun, ORONUM_CANPIE_LXRT_IDX);
  printk("CANPie Unloaded\n");
}


