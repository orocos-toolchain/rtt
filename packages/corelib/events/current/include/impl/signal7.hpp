/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:38 CET 2006  signal7.hpp 

                        signal7.hpp -  description
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
 
 

#ifndef OROCOS_SIGNAL_SIGNAL7_HEADER
#define OROCOS_SIGNAL_SIGNAL7_HEADER

#define OROCOS_SIGNAL_NUM_ARGS 7
#define OROCOS_SIGNAL_TEMPLATE_PARMS typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7
#define OROCOS_SIGNAL_TEMPLATE_ARGS T1, T2, T3, T4, T5, T6, T7
#define OROCOS_SIGNAL_PARMS arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7
#define OROCOS_SIGNAL_ARGS a1, a2, a3, a4, a5, a6, a7
#define OROCOS_SIGNAL_ARG_TYPES typedef T1 arg1_type;typedef T2 arg2_type;typedef T3 arg3_type;typedef T4 arg4_type;typedef T5 arg5_type;typedef T6 arg6_type;typedef T7 arg7_type;
#define OROCOS_SIGNAL_CONNECTION_TYPEDEFS typedef typename slot_function::arg1_type arg1_type;typedef typename slot_function::arg2_type arg2_type;typedef typename slot_function::arg3_type arg3_type;typedef typename slot_function::arg4_type arg4_type;typedef typename slot_function::arg5_type arg5_type;typedef typename slot_function::arg6_type arg6_type;typedef typename slot_function::arg7_type arg7_type;

#include "signal_template.hpp"

#undef OROCOS_SIGNAL_CONNECTION_TYPEDEFS
#undef OROCOS_SIGNAL_ARG_TYPES
#undef OROCOS_SIGNAL_ARGS
#undef OROCOS_SIGNAL_PARMS
#undef OROCOS_SIGNAL_TEMPLATE_ARGS
#undef OROCOS_SIGNAL_TEMPLATE_PARMS
#undef OROCOS_SIGNAL_NUM_ARGS

#endif // OROCOS_SIGNAL_SIGNAL7_HEADER
