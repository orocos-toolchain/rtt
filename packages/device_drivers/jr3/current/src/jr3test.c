#include <linux/init.h>
#include <linux/module.h>

void JR3DSP_check_sensor_and_DSP( unsigned int dsp );

int __init jr3check_init_module(void)
{
    JR3DSP_check_sensor_and_DSP( 0 );
    JR3DSP_check_sensor_and_DSP( 1 );
    return 0;
}


void __exit jr3check_cleanup_module(void)
{
}


module_init(jr3check_init_module);
module_exit(jr3check_cleanup_module);


