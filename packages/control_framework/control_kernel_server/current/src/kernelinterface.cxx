/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:25 CEST 2005  kernelinterface.cxx 

                        kernelinterface.cxx -  description
                           -------------------
    begin                : Tue April 05 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
 
 
#include "kernelinterface.hpp"

namespace ExecutionClient
{
    using namespace ORO_ControlKernel;
  KernelInterface* KernelInterface::sinstance = 0;

  void KernelInterface::setKernelInterfaceInstance( KernelInterface* kint )
  {
    delete sinstance;
    sinstance = kint;
  }

  KernelInterface* KernelInterface::instance()
  {
    return sinstance;
  }

  KernelInterface::~KernelInterface()
  {
  }
}
