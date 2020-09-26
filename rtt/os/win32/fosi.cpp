/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:15 CET 2005  fosi.c

                        fosi.c -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#include "rtt/os/fosi.h"
//linux specific
//#include <unistd.h>
#include <stdarg.h>

#undef rtos_printf

#ifdef __cplusplus
extern "C"
{
#endif

unsigned int sleep(unsigned int s)
{
  Sleep(s*1000);
  return 0;
}

#if __GNUC__ != 4
int usleep(unsigned int us)
{
    return win32_nanosleep(us * 1000);
}
#endif

int setenv(const char *name, const char *value, int overwrite)
{
  if (overwrite == 0)
  {
    char c;
    if (GetEnvironmentVariable(name, &c, sizeof (c)) > 0) return 0;
  }

  if (SetEnvironmentVariable(name, value) != 0) return 0;
  return -1;
}

#ifdef __cplusplus
}
#endif
