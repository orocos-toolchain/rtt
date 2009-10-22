/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  corba.h

                        corba.h -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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


#ifndef ORO_CORBA_SUPPORT_HH
#define ORO_CORBA_SUPPORT_HH

#include "rtt-corba-config.h"
#if CORBA_IS_TAO
#include <ace/SString.h>
#include <tao/corba.h>
#include <tao/PortableServer/PS_ForwardC.h>
#define CORBA_SERVANT(f) f ## "S.h"
#define CORBA_EXCEPTION_INFO(x) x._info().c_str()
#else
#include <omniORB4/CORBA.h>
#include <omniORB4/POA.h>
namespace corba {
    typedef Any* Any_ptr;
}
#define ACE_THROW_SPEC(x) throw x
#define CORBA_EXCEPTION_INFO(x) x._name()
#endif

#endif
