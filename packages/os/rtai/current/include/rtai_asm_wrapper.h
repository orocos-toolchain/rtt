/*
COPYRIGHT (C) 2000  Paolo Mantegazza (mantegazza@aero.polimi.it)

This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


/*
ACKNOWLEDGMENTS:
4/12/01, Jan Kiszka (Jan.Kiszka@web.de) for adding support to non 0/1 apics.
*/

/*
 * Grabbed from rtai-24.h for wrapping to C++ code in rtai_cpp
 * by Peter Soetens
 */

struct desc_struct
{
    void *fun;
};

struct apic_timer_setup_data
{
    int mode, count;
};

extern unsigned volatile int *locked_cpus;

extern void send_ipi_shorthand( unsigned int shorthand, int irq );
extern void send_ipi_logical( unsigned long dest, int irq );
extern int rt_assign_irq_to_cpu( int irq, unsigned long cpus_mask );
extern int rt_reset_irq_to_sym_mode( int irq );
extern int rt_request_global_irq( unsigned int irq, void ( *handler ) ( void ) );
extern int rt_request_global_irq_ext( unsigned int irq, void ( *handler ) ( void ), unsigned long data );
extern void rt_set_global_irq_ext( unsigned int irq, int ext, unsigned long data );
extern int rt_free_global_irq( unsigned int irq );
extern unsigned int rt_startup_irq( unsigned int irq );
extern void rt_shutdown_irq( unsigned int irq );
extern void rt_enable_irq( unsigned int irq );
extern void rt_disable_irq( unsigned int irq );
extern void rt_mask_and_ack_irq( unsigned int irq );
extern void rt_ack_irq( unsigned int irq );
extern void rt_unmask_irq( unsigned int irq );

extern int rt_request_linux_irq( unsigned int irq,

                                     void ( *linux_handler ) ( int irq, void *dev_id, struct pt_regs *regs ),
                                     char *linux_handler_id, void *dev_id );

extern int rt_free_linux_irq( unsigned int irq, void *dev_id );
extern void rt_pend_linux_irq( unsigned int irq );
extern int rt_request_srq( unsigned int label, void ( *rtai_handler ) ( void ), long long ( *user_handler ) ( unsigned int whatever ) );
extern int rt_free_srq( unsigned int srq );

extern struct desc_struct rt_set_full_intr_vect( unsigned int vector, int type, int dpl, void ( *handler ) ( void ) );

extern void rt_reset_full_intr_vect( unsigned int vector, struct desc_struct idt_element );
extern void *rt_set_intr_handler( unsigned int vector, void ( *handler ) ( void ) );
extern void rt_reset_intr_handler( unsigned int vector, void ( *handler ) ( void ) );
extern void rt_do_irq( int vector );
extern void rt_pend_linux_srq( unsigned int srq );
extern int rt_request_cpu_own_irq( unsigned int irq, void ( *handler ) ( void ) );
extern int rt_free_cpu_own_irq( unsigned int irq );
extern int rt_request_timer( void ( *handler ) ( void ), unsigned int tick, int apic );
extern void rt_request_timer_cpuid( void ( *handler ) ( void ), unsigned int tick, int cpuid );
extern void rt_free_timer( void );

extern void rt_request_apic_timers( void ( *handler ) ( void ), struct apic_timer_setup_data *apic_timer_data );
extern void rt_free_apic_timers( void );
extern void rt_mount_rtai( void );
extern void rt_umount_rtai( void );
extern int rt_printk( const char *format, ... );
extern void ll2a( long long ll, char *s );

extern int rtai_print_to_screen( const char *format, ... );
extern int rt_is_lxrt( void );

// The next 3 should probably go inside INTERFACE_TO_LINUX
// PC: Can someone explain why we have INTERFACE_TO_LINUX ?
extern int rt_is_linux( void );
extern RT_TRAP_HANDLER rt_set_rtai_trap_handler( RT_TRAP_HANDLER handler );

extern struct task_struct *rt_whoislinux( int cpuid );


#ifdef INTERFACE_TO_LINUX
extern void rt_switch_to_linux( int cpuid );
extern void rt_switch_to_real_time( int cpuid );
#endif

// TODO : Find a way to call the inline functions in rtai-XX.h
//
