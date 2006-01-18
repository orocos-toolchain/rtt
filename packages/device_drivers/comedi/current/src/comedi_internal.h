/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  comedi_internal.h 

                        comedi_internal.h -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 

// Some info about this hack :
// For LXRT, we should include <rtai_comedi.h>
// but this file is horribly broken, thus
// we use the signatures of the linux/comedilib.h
// functions but _link_ against libkcomedilib_lxrt.so

#include <pkgconf/system.h>
#ifdef OROPKG_OS_LXRT
    #define __KERNEL__
    extern "C"
    {
#include <linux/comedilib.h>
    }
    #undef __KERNEL__
#else
// userspace gnu/linux:
    #include <comedilib.h>
#endif

