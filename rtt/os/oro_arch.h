/***************************************************************************
  tag: Philippe Hamelin Wed Apr 13 13:02:01 2011 -0400 oro_arch.h

                        oro_arch.h -  description
                           -------------------
    begin                : Wed Apr 13 2011
    copyright            : (C) 2005 Peter Soetens
    copyright            : (C) 2011 Philippe Hamelin
    email                : philippe.hamelin@gmail.com

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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#include "rtt/rtt-config.h"

# if defined(OROBLD_OS_NO_ASM)
#include "rtt/os/oro_noasm/oro_arch.h"
# else
#  if defined(_MSC_VER)
#include "rtt/os/oro_msvc/oro_arch.h"
#  elif defined(__GNUC__)
#    if ( OROBLD_GCC_VERSION < 40100 || defined(__clang__) )
#      if defined( OROBLD_OS_ARCH_i386 )
#include "rtt/os/oro_i386/oro_arch.h"
#      elif defined( OROBLD_OS_ARCH_x86_64 )
#include "rtt/os/oro_x86_64/oro_arch.h"
#      elif defined( OROBLD_OS_ARCH_ppc )
#include "rtt/os/oro_powerpc/oro_arch.h"
#      else
#         error "Unsupported architecture for GCC < 4.1.0 or clang++"
#      endif
#    else // GCC >= 4.1.0 has intrinsics. Clang++'s 2.9 intrinsics are broken in C++ code
#include "rtt/os/oro_gcc/oro_arch.h"
#    endif
#  else
#   error "Unsupported architecture or compiler"
#  endif
# endif
