/***************************************************************************
  tag: 

                        ecos_rec_mutex.c -  description
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

#include <rtt/os/ecos_rec_mutex.h>
 
void cyg_recursive_mutex_init( cyg_recursive_mutex_t *mx )
{
  cyg_mutex_init(&(mx->mutex));
  mx->owner=0;
  mx->count = 0;
}

bool cyg_recursive_mutex_lock( cyg_recursive_mutex_t *mx )
{
  bool result = false;
  cyg_scheduler_lock();
  {
    if( cyg_thread_self() == mx->owner )
      {
          mx->count++;
          result = true;
      }
    else
      {
          result = cyg_mutex_lock( &mx->mutex );
          mx->count = 1;
          mx->owner = cyg_thread_self();
      }
  }
  cyg_scheduler_unlock();
  return result;
}

bool cyg_recursive_mutex_trylock( cyg_recursive_mutex_t *mx )
{
    bool result=false;
    cyg_scheduler_lock();
    {
        if( cyg_thread_self() == mx->owner ) {
            mx->count++;
            result = true;
        } else {
            result = cyg_mutex_trylock( &mx->mutex );
            if (result == true) {
                mx->count = 1;
                mx->owner = cyg_thread_self();
            }
        }
    }
  cyg_scheduler_unlock();
  return result;
}

void cyg_recursive_mutex_unlock( cyg_recursive_mutex_t *mx )
{
  cyg_scheduler_lock();
  {
    if( cyg_thread_self() == mx->owner )
      {
	// Only do something if mutex was locked
	if (mx->count >= 1)
	  {
	    mx->count--;
	    if (mx->count == 0)
	      {
		cyg_mutex_unlock( &mx->mutex );
	      }
	  }
	else
	  diag_printf("recursive mutex: not locked\n");
      }
    else
      diag_printf("Error unlocking recursive mutex: you're not the owner!\n");
  }
  cyg_scheduler_unlock();
}

void cyg_recursive_mutex_destroy( cyg_recursive_mutex_t *mx )
{
  // Fixme: need check here to verify if owner??
  cyg_mutex_release(&(mx->mutex));
  cyg_mutex_destroy(&(mx->mutex));
}
