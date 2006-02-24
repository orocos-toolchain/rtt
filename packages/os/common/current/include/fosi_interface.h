/**
 * @file fosi_interface.h
 * This file defines the FOSI (Framework Operating System Interface) from
 * orocos calls to native RTOS calls  
 *
 * Rename to 'fosi.h'
 */

#ifndef ORO_FOSI_INTERFACE_H
#define ORO_FOSI_INTERFACE_H

#error "This file is a template for developers, do not include directly."

#ifdef __cplusplus
extern "C"
{
#endif

	/**
	 * 'Task' and 'Thread' should merge into same type.
	 */
    struct TaskType;
	struct ThreadType;
    typedef struct TaskTask RTOS_TASK;
	typedef struct ThreadType RTOS_THREAD;

	// include 'atomic' functions,
	// depends on processor target, not OS.
#include "oro_atomic.h"
#include "oro_bitops.h"

	/**
	 * Time functions.
	 */

    typedef long long NANO_TIME;
    typedef long long TICK_TIME;

    typedef struct timespec TIME_SPEC;

    NANO_TIME rtos_get_time_ns( void );

    /**
     * This function should return ticks.
     */
    TICK_TIME systemTimeGet();

    /**
     * This function should return nano seconds
     */
    NANO_TIME systemNSecsTimeGet();

    /**
     * Time conversions.
     */
    TICK_TIME nano2ticks( NANO_TIME nano );

    NANO_TIME ticks2nano( TICK_TIME count );

	// Semaphore functions

	typedef struct sem_struct rt_sem_t;

	int rtos_sem_init(rt_sem_t* m, int value );
	int rtos_sem_destroy(rt_sem_t* m );
	int rtos_sem_signal(rt_sem_t* m );
	int rtos_sem_wait(rt_sem_t* m );
	int rtos_sem_trywait(rt_sem_t* m );
	int rtos_sem_wait_timed(rt_sem_t* m, NANO_TIME delay );
    int rtos_sem_value(rt_sem_t* m );

    // Mutex functions
	// The 'mutexattr' part must disapear, it is nowhere used.

    typedef struct mutex_struct rt_mutex_t;
    typedef struct mutex_attr_struct rt_mutex_attr_t;

    int rtos_mutex_init(rt_mutex_t* m, const rt_mutex_attr_t *mutexattr );
    int rtos_mutex_destroy(rt_mutex_t* m );
    int rtos_mutex_rec_init(rt_mutex_t* m, const rt_mutex_attr_t *mutexattr);
    int rtos_mutex_rec_destroy(rt_mutex_t* m );
    int rtos_mutex_lock( rt_mutex_t* m);
    int rtos_mutex_trylock( rt_mutex_t* m);
    int rtos_mutex_unlock( rt_mutex_t* m);

	// 'real-time' print functions

	int rtos_printf( const char *fmt, ... );

#ifdef __cplusplus
}
#endif

#endif // ORO_FOSI_INTERFACE_H
