// $Id: nAxisGeneratorCartesianPosForce.hpp,v 1.1.1.1 2003/12/02 20:32:06 kgadeyne Exp $
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


#include "kernel_components/nAxesSensorCartesianPosForce.hpp"
#include <assert.h>

#define GRAVITY_CONSTANT    9.81

namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_DeviceInterface;


  nAxesSensorCartesianPosForce::nAxesSensorCartesianPosForce(unsigned int num_axes, 
							     std::vector<AxisInterface*> axes,
							     SensorInterface<ORO_Geometry::Wrench>* forcesensor,
							     ORO_KinDyn::KinematicsComponent* kin,
							     const Frame& MP_EE,
							     const Frame& MP_FS,
							     std::string name)
    : nAxesSensorCartesianPosForce_typedef(name),
      _num_axes(num_axes), 
      _axes(axes),
      _position_sensors(num_axes),
      _forcesensor(forcesensor),
      _kinematics(kin),
      _position_joint(num_axes),
      _MP_EE(MP_EE),
      _MP_FS(MP_FS),
      _properties_read(false),
      _mass("mass", "Mass attached to force sensor"),
      _center_gravity("center_gravity", "Center of gravity from mass attached to force sensor"),
      _offset("offset", "Offset on measured values of force sensor")
  {
    assert(_axes.size() == num_axes);

    // get position sensors from axes
    for (unsigned int i=0; i<_num_axes; i++){
      _position_sensors[i] = _axes[i]->getSensor("Position");
      assert( _position_sensors[i] != NULL );
    }
  }


  nAxesSensorCartesianPosForce::~nAxesSensorCartesianPosForce(){};
  

  void nAxesSensorCartesianPosForce::pull()
  {
    // copy values from position sensors to local variable
    for (unsigned int i=0; i<_num_axes; i++)
      _position_joint[i] = _position_sensors[i]->readSensor();

    // copy values from force sensor to local variable
    _forcesensor->readSensor(_force);
  }


  void nAxesSensorCartesianPosForce::calculate()
  {
    // forwarard kinematics
    ORO_CoreLib::Double6D temp;
    for (unsigned int i=0; i<_num_axes; i++)
      temp[i] = _position_joint[i];
    _kinematics->positionForward(temp, _world_MP );

    // mass compensation and offset for force sensor
    _world_FS = _world_MP * _MP_FS;
    _gravity.torque = (_world_FS * _center_gravity) * _gravity.force;
    _force = ( _world_FS * (_force - _offset) ) - _gravity;
  }


  
  void nAxesSensorCartesianPosForce::push()      
  {
    _position_out_DOI->Set(_world_MP * _MP_EE);
    _force_out_DOI->Set(_force);
  }



  bool nAxesSensorCartesianPosForce::componentLoaded()
  {
    // get interface to Input data types
    if (!Sensor< Writes<nAxesSensorCartesianPosForceInput_pos_force> >::Input::dObj()->Get("Frame", _position_out_DOI) ||
	!Sensor< Writes<nAxesSensorCartesianPosForceInput_pos_force> >::Input::dObj()->Get("Wrench", _force_out_DOI)){
      cerr << "nAxesSensorCartesianPosForce::componentLoaded() DataObjectInterface not found" << endl;
      return false;
    }

    // set empty values
    Frame _temp_frame;
    Wrench _temp_wrench;
    _position_out_DOI->Set(_temp_frame);
    _force_out_DOI->Set(_temp_wrench);

    return true;
  }


  bool nAxesSensorCartesianPosForce::componentStartup()
  {
    // check if updateProperties has been called
    if (!_properties_read){
      cerr << "nAxesSensorCartesianPosForce::componentStartup() Properties have not been read." << endl;
      return false;
    }

    return true;
  }
  


  bool nAxesSensorCartesianPosForce::updateProperties(const PropertyBag& bag)
  {
    // properties have been read
    _properties_read = true;

    // get properties
    if (!composeProperty(bag, _mass) ||
	!composeProperty(bag, _center_gravity) ||
        !composeProperty(bag, _offset)){
	cerr << "nAxesSensorCartesianPosForce::updateProperties() failed" << endl;
      return false;
    }

    // foce component of gravity
    _gravity.force  = Vector(0, 0, (-1 * _mass * GRAVITY_CONSTANT));

    return true;
  }

  void nAxesSensorCartesianPosForce::exportProperties(ORO_CoreLib::PropertyBag& bag)
  {};


  

} // end namespace ORO_nAxesControlKernel
