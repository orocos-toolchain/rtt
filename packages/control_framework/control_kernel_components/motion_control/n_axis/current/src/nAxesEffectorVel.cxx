// $Id: nAxisGeneratorCartesianPos.hpp,v 1.1.1.1 2003/12/02 20:32:06 kgadeyne Exp $
// Copyright (C) 2003 Klaas Gadeyne <klaas.gadeyne@mech.kuleuven.ac.be>
//                    Wim Meeussen  <wim.meeussen@mech.kuleuven.ac.be>
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  

#include "control_kernel/nAxesEffectorVel.hpp"
#include <corelib/Logger.hpp>
#include <assert.h>

namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_DeviceInterface;
  using namespace ORO_CoreLib;


  nAxesEffectorVel::nAxesEffectorVel(unsigned int num_axes, 
					 std::vector<AxisInterface*> axes,
					 std::string name)
    : nAxesEffectorVel_typedef(name),
      _num_axes(num_axes), 
      _axes(axes),
      _velocity_local(num_axes)
  {
    assert(_axes.size() == num_axes);
  }


  nAxesEffectorVel::~nAxesEffectorVel(){};
  

  void nAxesEffectorVel::pull()
  {
    // copy Output to local values
    _velocity_DOI->Get(_velocity_local);
  }


  void nAxesEffectorVel::calculate()
  {}

  
  void nAxesEffectorVel::push()      
  {
    for (unsigned int i=0; i<_num_axes; i++)
      _axes[i]->drive(_velocity_local[i]);
  }


  bool nAxesEffectorVel::componentLoaded()
  {
    return true;
  }


  bool nAxesEffectorVel::componentStartup()
  {
    // get interface to Output data type
    if ( !Output->dObj(   )->Get("Velocity_joint", _velocity_DOI) ){
      Logger::log() << Logger::Error << "nAxesEffectorVel::componentStartup() DataObjectInterface not found" << Logger::endl;
      return false;
    }
    return true;
  }
  

  bool nAxesEffectorVel::updateProperties(const PropertyBag& bag)
  {
    return true;
  }


} // namespace
