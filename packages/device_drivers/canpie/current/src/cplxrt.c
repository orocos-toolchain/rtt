


#include <linux/module.h>

#include <linux/config.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/interrupt.h>
#include <asm/io.h>

#include "can/cplxrt.h"
#include "can/cpcore.h"
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

extern unsigned int cp_recv_int;
extern unsigned int cp_emcy_int;
extern unsigned int cp_trns_int;

#define STACK_SIZE 2000
#define CHANNEL 0
/**
 * Transmit task.
 */
RT_TASK cp_tx_task;
/**
 * Sem that signals a messages should be
 * transmitted (signalled from Send or interrupt)
 */
SEM cp_tx_sem;
/**
 * Receive task.
 */
RT_TASK cp_rx_task;
/**
 * Sem that signals a messages should be
 * received (signalled from Send or interrupt)
 */
SEM cp_rx_sem;

/**
 * The transmit function.
 */
void cp_tx_function(int channel)
{
	while(1)
		{
			rt_sem_wait( &cp_tx_sem );
			CpCoreMsgTransmit(channel);
		}
}
		
/**
 * The receive function.
 */
void cp_rx_function(int channel)
{
	while(1)
		{
			rt_sem_wait( &cp_rx_sem );
			CpCoreMsgReceive(channel);
		}
}
		


/* init module */
int init_module(void)
{
  if( set_rt_fun_ext_index(rt_apci_fun, ORONUM_CANPIE_LXRT_IDX)) {
    printk("LXRT index %d already in use !\n", ORONUM_CANPIE_LXRT_IDX);
    printk("Recompile canpie with a different index\n");
    return -EACCES;
  }
  cp_recv_int = cp_emcy_int = cp_trns_int = 0;
  rt_task_init(&cp_tx_task, cp_tx_function, CHANNEL, STACK_SIZE, 0, 0, 0);
  rt_sem_init( &cp_tx_sem, 0);
  rt_task_init(&cp_rx_task, cp_rx_function, CHANNEL, STACK_SIZE, 0, 0, 0);
  rt_sem_init( &cp_rx_sem, 0);
  printk("CANPie Loaded\n");
  return(0);
}

/*  cleanup module */
void cleanup_module(void)
{
  reset_rt_fun_ext_index(rt_apci_fun, ORONUM_CANPIE_LXRT_IDX);
  rt_task_delete(&cp_tx_task);
  rt_sem_delete(&cp_tx_sem);
  rt_task_delete(&cp_rx_task);
  rt_sem_delete(&cp_rx_sem);
  printk("CANPie Interrupt Statistics : #RECV: %d, #TRNS: %d, #EMCY: %d\n",cp_recv_int, cp_trns_int, cp_emcy_int);
  printk("CANPie Unloaded\n");
}


