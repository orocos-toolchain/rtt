/*
 * Two Levels Segregate Fit memory allocator (TLSF)
 * Version 2.4.4
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

#ifndef _TLSF_H_
#define _TLSF_H_

/* We avoid name clashes with other projects and
 * only make the required function available
 */
#include "rtt/os/fosi.h"
#define tlsf_malloc oro_rt_malloc
#define tlsf_free oro_rt_free
#define tlsf_realloc oro_rt_realloc
#define tlsf_calloc oro_rt_calloc

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef ORO_MEMORY_POOL
extern size_t init_memory_pool(size_t, void *);
extern size_t get_used_size(void *);
extern size_t get_used_size_mp();
extern size_t get_max_size(void *);
extern size_t get_max_size_mp();
extern void destroy_memory_pool(void *);
extern size_t add_new_area(void *, size_t, void *);
extern void *malloc_ex(size_t, void *);
extern void free_ex(void *, void *);
extern void *realloc_ex(void *, size_t, void *);
extern void *calloc_ex(size_t, size_t, void *);
#endif

extern void *tlsf_malloc(size_t size);
extern void tlsf_free(void *ptr);
extern void *tlsf_realloc(void *ptr, size_t size);
extern void *tlsf_calloc(size_t nelem, size_t elem_size);

#ifdef	__cplusplus
}
#endif

#endif
