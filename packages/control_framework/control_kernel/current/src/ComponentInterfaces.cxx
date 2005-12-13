/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:07 CEST 2004  ComponentInterfaces.cxx 

                        ComponentInterfaces.cxx -  description
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

#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "control_kernel/ComponentInterfaces.hpp"
#include "control_kernel/KernelInterfaces.hpp"

using namespace ORO_ControlKernel;

bool ComponentBaseInterface::enableFacet(KernelBaseFunction* e)
{
    kern = e;
    return kern->addComponent(this);
}

void ComponentBaseInterface::disableFacet()
{
    if ( inKernel() )
        {
            kern->removeComponent(this);
            kern = 0;
        }
}

bool ComponentBaseInterface::startup()
{
    return kern->startup(this);
}

bool ComponentBaseInterface::restart()
{
    return kern->restart(this);
}

bool ComponentBaseInterface::shutdown()
{
    return kern->shutdown(this);
}

bool ComponentBaseInterface::select()
{
    return kern->select(this);
}
