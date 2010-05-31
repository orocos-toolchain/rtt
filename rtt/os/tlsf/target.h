#ifndef _TLSF_TARGET_H_
#define _TLSF_TARGET_H_

#include "../fosi.h"

#define TLSF_MLOCK_T            rt_mutex_t
#define TLSF_CREATE_LOCK(l)     rtos_mutex_init (l)
#define TLSF_DESTROY_LOCK(l)    rtos_mutex_destroy(l)
#define TLSF_ACQUIRE_LOCK(l)    rtos_mutex_lock(l)
#define TLSF_RELEASE_LOCK(l)    rtos_mutex_unlock(l)

#endif
