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

#include "control_kernel/nAxesEffectorCartesianVel.hpp"
#include <assert.h>

namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_DeviceInterface;


  nAxesEffectorCartesianVel::nAxesEffectorCartesianVel(unsigned int num_axes, 
						       std::vector<AxisInterface*> axes,
						       ORO_KinDyn::KinematicsComponent* kin,
						       std::string name)
    : nAxesEffectorCartesianVel_typedef(name),
      _num_axes(num_axes), 
      _axes(axes),
      _kinematics(kin),
      _velocity_joint_local(num_axes),
      _position_sensors(num_axes)
  {
    assert(_axes.size() == num_axes);

    // get position sensors from axes
    for (unsigned int i=0; i<_num_axes; i++){
      _position_sensors[i] = _axes[i]->getSensor("Position");
      assert( _position_sensors[i] != NULL );
    }
  }


  nAxesEffectorCartesianVel::~nAxesEffectorCartesianVel(){};
  

  void nAxesEffectorCartesianVel::pull()
  {
    // copy Output to local values
    _velocity_cartesian_DOI->Get(_velocity_cartesian_local);
  }


  void nAxesEffectorCartesianVel::calculate()
  {
    ORO_CoreLib::Double6D q, q_dot;
    for (unsigned int i=0; i<_num_axes; i++)
      q[i] = _position_sensors[i]->readSensor();

    // convert twist
    _kinematics->velocityInverse(q, _velocity_cartesian_local, q_dot);
    for (unsigned int i=0; i<_num_axes; i++)
      _velocity_joint_local[i] = q_dot[i];
  }

  
  void nAxesEffectorCartesianVel::push()      
  {
    for (unsigned int i=0; i<_num_axes; i++)
      _axes[i]->drive(_velocity_joint_local[i]);
  }


  bool nAxesEffectorCartesianVel::componentLoaded()
  {
    return true;
  }


  bool nAxesEffectorCartesianVel::componentStartup()
  {
    // get interface to Output data type
    if ( !Output::dObj(   )->Get("Twist", _velocity_cartesian_DOI) ){
      cerr << "nAxesEffectorCartesianVel::componentStartup() DataObjectInterface not found" << endl;
      return false;
    }
    return true;
  }
  


} // namespace
