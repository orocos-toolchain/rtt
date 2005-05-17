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


#include <control_kernel/nAxesSensorCartesianPos.hpp>
#include <corelib/Logger.hpp>
#include <assert.h>

namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_DeviceInterface;
  using namespace ORO_CoreLib;


  nAxesSensorCartesianPos::nAxesSensorCartesianPos(unsigned int num_axes, 
						   std::vector<AxisInterface*> axes,
						   ORO_KinDyn::KinematicsComponent* kin,
						   const ORO_Geometry::Frame& offset,
						   std::string name)
    : nAxesSensorCartesianPos_typedef(name),
      _num_axes(num_axes), 
      _axes(axes),
      _kinematics(kin),
      _position_joint(num_axes),
      _offset(offset),
      _position_sensors(num_axes)
  {
    assert(_axes.size() == num_axes);

    // get position sensors from axes
    for (unsigned int i=0; i<_num_axes; i++){
      _position_sensors[i] = _axes[i]->getSensor("Position");
      assert( _position_sensors[i] != NULL );
    }
  }


  nAxesSensorCartesianPos::~nAxesSensorCartesianPos(){};
  

  void nAxesSensorCartesianPos::pull()
  {
    // copy values from position sensors to local variable
    for (unsigned int i=0; i<_num_axes; i++)
      _position_joint[i] = _position_sensors[i]->readSensor();
  }



  void nAxesSensorCartesianPos::calculate()
  {
    // forwarard kinematics
    ORO_CoreLib::Double6D temp;
    for (unsigned int i=0; i<_num_axes; i++)
      temp[i] = _position_joint[i];

    _kinematics->positionForward(temp, _position_cart );
  }


  
  void nAxesSensorCartesianPos::push()      
  {
    _position_cart_DOI->Set(_position_cart * _offset);
    _position_joint_DOI->Set(_position_joint);
  }



  bool nAxesSensorCartesianPos::componentLoaded()
  {
    // get interface to Input data types
    if (!Input->dObj()->Get("Position_EE", _position_cart_DOI) ||
	!Input->dObj()->Get("Position_joint", _position_joint_DOI)){
      Logger::log() << Logger::Error << "nAxesSensorCartesianPos::componentLoaded() DataObjectInterface not found" << Logger::endl;
      return false;
    }

    // set empty values
    _position_cart_DOI->Set(_position_cart);
    _position_joint_DOI->Set(_position_joint);

    return true;
  }


  bool nAxesSensorCartesianPos::componentStartup()
  {
    return true;
  }
  
  

} // end namespace ORO_nAxesControlKernel
