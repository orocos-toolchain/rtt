/***************************************************************************
  tag: FMTC  do nov 2 13:05:58 CET 2006  signature3.hpp 

                        signature3.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
    email                : peter.soetens@fmtc.be
 
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
 
 
#define OROCOS_SIGNATURE_NUM_ARGS 3
#define OROCOS_SIGNATURE_TEMPLATE_PARMS typename T1, typename T2, typename T3
#define OROCOS_SIGNATURE_TEMPLATE_ARGS T1, T2, T3
#define OROCOS_SIGNATURE_PARMS arg1_type a1, arg2_type a2, arg3_type a3
#define OROCOS_SIGNATURE_ARGS a1, a2, a3
#define OROCOS_SIGNATURE_ARG_TYPES typedef T1 arg1_type;typedef T2 arg2_type;typedef T3 arg3_type;
#define OROCOS_SIGNATURE_TYPEDEFS typedef typename function_type::arg1_type arg1_type;typedef typename function_type::arg2_type arg2_type;typedef typename function_type::arg3_type arg3_type;

#include "signature_template.hpp"

#undef OROCOS_SIGNATURE_TYPEDEFS
#undef OROCOS_SIGNATURE_ARG_TYPES
#undef OROCOS_SIGNATURE_ARGS
#undef OROCOS_SIGNATURE_PARMS
#undef OROCOS_SIGNATURE_TEMPLATE_ARGS
#undef OROCOS_SIGNATURE_TEMPLATE_PARMS
#undef OROCOS_SIGNATURE_NUM_ARGS
