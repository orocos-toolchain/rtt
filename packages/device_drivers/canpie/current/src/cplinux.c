


#include <linux/module.h>

#include <linux/config.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/interrupt.h>
#include <asm/io.h>

#include <pkgconf/device_drivers_canpie.h>

MODULE_LICENSE("GPL");
//EXPORT_NO_SYMBOLS;

		
/* init module */
int init_module(void)
{
  printk("Loading CANPie in kernel.\n");
  return(0);
}

/*  cleanup module */
void cleanup_module(void)
{
  printk("Unloading CANPie from kernel.\n");
}


