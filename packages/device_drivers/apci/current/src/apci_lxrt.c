

/**
 * This file is intended to be compiled as a kernel module and export all the
 * device driver kernel functions to LXRT. One has to include devices_lxrt.h
 * in his lxrt program to interact with the device drivers.
 *
 * I will use LXRT index 15 and concentrate all device acces through this file.
 * Comedi is already supported in LXRT by the default rtai (from 2.4.11 on) package.
 *
 * Feb 2003 Peter Soetens
 */

//#include "apci1710/apci1710.h"
//#include "apci2200/apci2200.h"
//#include "apci1032/apci1032.h"


#include <linux/module.h>

#include <rtai_config.h>
#include "apci_lxrt.h"
#include <rtai.h>
#include <rtai_lxrt.h>


MODULE_LICENSE("GPL");
//EXPORT_NO_SYMBOLS;

/**
 * The first parameter is the bitmask and denotes what marshalling must
 * happen and if the call is realtime (bit 0), the second parameter is the function
 * to be called.
 */
static struct rt_fun_entry rt_apci_fun[] = {
#ifdef OROINT_DEVICE_DRIVERS_APCI1032
    [ APCI1032_READ1 ] = { 0,i_APCI1032_Read1DigitalInput },
    [ APCI1032_READ8  ] = { 0,i_APCI1032_Read8DigitalInput },
    [ APCI1032_READ16 ] = { 0,i_APCI1032_Read16DigitalInput },
    [ APCI1032_READ32 ] = { 0,i_APCI1032_Read32DigitalInput },
#endif
#ifdef OROINT_DEVICE_DRIVERS_APCI2200
    [ APCI2200_GETDEVICE	] = { 0,apci2200_get_device },
    [ APCI2200_GETINPUTSTATUS	] = { 0,apci2200_get_input_status },
    [ APCI2200_GETOUTPUTSTATUS	] = { 0,apci2200_get_output_status },
    [ APCI2200_SETOUTPUTON	] = { 0,apci2200_set_output_on },
    [ APCI2200_SETOUTPUTOFF	] = { 0,apci2200_set_output_off },
    /*
    [ APCI2200_WATCHDOGINIT	] = { 0,apci2200_watchdog_init },
    [ APCI2200_WATCHDOGSTART	] = { 0,apci2200_watchdog_start },
    [ APCI2200_WATCHDOGSTOP	] = { 0,apci2200_watchdog_stop },
    [ APCI2200_WATCHDOGGETSTATUS] = { 0,apci2200_watchdog_get_status },
    [ APCI2200_WATCHDOGTRIGGER	] = { 0,apci2200_watchdog_trigger },
    */
    [ APCI2200_GETNUMBERDO       ] = { 0,apci2200_get_number_off_digital_outputs },
#endif
#ifdef OROINT_DEVICE_DRIVERS_APCI1710
    [ APCI1710_GETDEVICE         ] = { 0,apci1710_get_device },
    [ APCI1710_SSI_INIT          ] = { 0,apci1710_ssi_init },
    [ APCI1710_SSI_READ          ] = { UW1(4,0),apci1710_ssi_read },
//    [ APCI1710_SSI_READALL       ] = { 0,apci1710_ssi_read_all },
    [ APCI1710_SSI_READALL       ] = { UW1(3,4),apci1710_ssi_read_all },
    [ APCI1710_SSI_REFRESH       ] = { 0,apci1710_ssi_refresh },
    [ APCI1710_SSI_READLATEST    ] = { 0,apci1710_ssi_read_latest },
//    [ APCI1710_SSI_READLATESTALL ] = { 0,apci1710_ssi_read_latest_all },
    [ APCI1710_SSI_READLATESTALL ] = { UW1(3,4),apci1710_ssi_read_latest_all },

    [ APCI1710_INCR_INIT                ] = { 0, apci1710_incr_init},
    [ APCI1710_INCR_COUNTER_AUTOTEST    ] = { 0, apci1710_incr_counter_auto_test},
    [ APCI1710_INCR_CLEAR_COUNTER       ] = { 0, apci1710_incr_clear_counter_value},
    [ APCI1710_INCR_SET_INPFILTER       ] = { 0, apci1710_incr_set_input_filter},
    [ APCI1710_INCR_LATCH_COUNTER       ] = { 0, apci1710_incr_latch_counter},
    [ APCI1710_INCR_READ_LATCH_REGSTATUS] = { 0, apci1710_incr_read_latch_register_status},
    [ APCI1710_INCR_READ_LATCH_REGVALUE ] = { 0, apci1710_incr_read_latch_register_value},
    [ APCI1710_INCR_READ_16_COUNT_VALUE ] = { 0, apci1710_incr_read_16bit_counter_value},
    [ APCI1710_INCR_ENABLE_LATCH_INT    ] = { 0, apci1710_incr_enable_latch_interrupt},
    [ APCI1710_INCR_DISABLE_LATCH_INT   ] = { 0, apci1710_incr_disable_latch_interrupt},
    [ APCI1710_INCR_READ_32_COUNT_VALUE ] = { 0, apci1710_incr_read_32bit_counter_value},
    [ APCI1710_INCR_WRITE_16_COUNT_VALUE] = { 0, apci1710_incr_write_16bit_counter_value},
    [ APCI1710_INCR_WRITE_32_COUNT_VALUE] = { 0, apci1710_incr_write_32bit_counter_value},
    [ APCI1710_INCR_INIT_INDEX          ] = { 0, apci1710_incr_init_index},
    [ APCI1710_INCR_ENABLE_INDEX        ] = { 0, apci1710_incr_enable_index},
    [ APCI1710_INCR_DISABLE_INDEX       ] = { 0, apci1710_incr_disable_index},
    [ APCI1710_INCR_GET_INDEX_STATUS    ] = { 0, apci1710_incr_get_index_status},
    [ APCI1710_INCR_SET_DIG_CHANNEL_ON  ] = { 0, apci1710_incr_set_digital_channel_on},
    [ APCI1710_INCR_SET_DIG_CHANNEL_OFF ] = { 0, apci1710_incr_set_digital_channel_off},
    [ APCI1710_INCR_CREATE ] = { 0, apci1710_incr_create_module},
    [ APCI1710_INCR_CLEANUP ] = { 0, apci1710_incr_cleanup_module},
#endif
};



/* init module */
int init_module(void)
{

	if( set_rt_fun_ext_index(rt_apci_fun, MYIDX)) {
		printk("Recompile your module with a different index\n");
		return -EACCES;
	}

	return(0);
}

/*  cleanup module */
void cleanup_module(void)
{
	reset_rt_fun_ext_index(rt_apci_fun, MYIDX);
}


