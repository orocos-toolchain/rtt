/***************************************************************************
  tag: 

                        ecos_rec_mutex.h -  description
                           -------------------
    begin                : Jan 21 2006
    copyright            : (C) 2006 Klaas Gadeyne
    email                : firstname lastname at fmtc be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef ORO_OS_ECOS_REC_MUTEX_HPP
#define ORO_OS_ECOS_REC_MUTEX_HPP

#include <cyg/kernel/kapi.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct 
{
  cyg_mutex_t     mutex;
  cyg_handle_t    owner;
  cyg_uint32      count;
}
cyg_recursive_mutex_t;

/// Initialize recursive mutex
void cyg_recursive_mutex_init( cyg_recursive_mutex_t *mx );

/// Lock recursive mutex
bool cyg_recursive_mutex_lock( cyg_recursive_mutex_t *mx );

/// Trylock
bool cyg_recursive_mutex_trylock( cyg_recursive_mutex_t *mx );

/// Unlock
void cyg_recursive_mutex_unlock( cyg_recursive_mutex_t *mx );

/// Destroy
void cyg_recursive_mutex_destroy( cyg_recursive_mutex_t *mx );

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
