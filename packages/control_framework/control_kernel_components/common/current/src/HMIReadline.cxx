/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:05 CEST 2004  HMIReadline.cxx 

                        HMIReadline.cxx -  description
                           -------------------
    begin                : Thu July 15 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens at mech.kuleuven.ac.be
 
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

#pragma implementation
#include "control_kernel/HMIReadline.hpp"

namespace ORO_ControlKernel
{
#if 1
    std::vector<std::string> HMIReadline::completes;
    std::vector<std::string>::iterator HMIReadline::complete_iter;
    std::string HMIReadline::component;
    std::string HMIReadline::method;
    std::string HMIReadline::datasource;
    std::string HMIReadline::text;
    ExecutionExtension* HMIReadline::ee = 0;
#ifdef OROPKG_CONTROL_KERNEL_EXTENSIONS_EXECUTION
    TaskContext* HMIReadline::taskcontext = 0;
#endif
#endif
}
