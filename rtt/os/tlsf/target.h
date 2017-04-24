/*
 * Two Levels Segregate Fit memory allocator (TLSF)
 * Version 2.4.6
 *
 * Written by Miguel Masmano Tello <mimastel@doctor.upv.es>
 *
 * Thanks to Ismael Ripoll for his suggestions and reviews
 *
 * Copyright (C) 2008, 2007, 2006, 2005, 2004
 *
 * This code is released using a dual license strategy: GPL/LGPL
 * You can choose the licence that better fits your requirements.
 *
 * Released under the terms of the GNU General Public License Version 2.0
 * Released under the terms of the GNU Lesser General Public License Version 2.1
 *
 */
#ifndef _TLSF_TARGET_H_
#define _TLSF_TARGET_H_

#include "../fosi.h"

#define TLSF_MLOCK_T            rt_mutex_t
#define TLSF_CREATE_LOCK(l)     rtos_mutex_init (l)
#define TLSF_DESTROY_LOCK(l)    rtos_mutex_destroy(l)
#define TLSF_ACQUIRE_LOCK(l)    rtos_mutex_lock(l)
#define TLSF_RELEASE_LOCK(l)    rtos_mutex_unlock(l)

#endif
