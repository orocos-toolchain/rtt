// $Id: nAxesSensorCartesianPosForce.hpp,v 1.1.1.1 2003/12/02 20:32:06 kgadeyne Exp $
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

#ifndef __N_AXES_SENSOR_CARTESIAN_POS_FORCE_H__
#define __N_AXES_SENSOR_CARTESIAN_POS_FORCE_H__

#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/PropertyExtension.hpp>
#include <control_kernel/ReportingExtension.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <corelib/PropertyComposition.hpp>
#include <device_interface/AxisInterface.hpp>
#include <kindyn/KinematicsComponent.hpp>

namespace ORO_ControlKernel
{

  // ---------------
  // -- DATA TYPE --
  // ---------------
  class nAxesSensorCartesianPosForceInput_pos_force: public ORO_ControlKernel::ServedTypes< ORO_Geometry::Frame, ORO_Geometry::Wrench>
  {
  public:
    nAxesSensorCartesianPosForceInput_pos_force()
    {
      this->insert(ORO_ControlKernel::make_pair(0,"Frame"));
      this->insert(ORO_ControlKernel::make_pair(1,"Wrench"));
    }
  }; // class




  // ---------------
  // -- COMPONENT --
  // ---------------
  typedef ORO_ControlKernel::Sensor< ORO_ControlKernel::Writes<nAxesSensorCartesianPosForceInput_pos_force>,
				     ORO_ControlKernel::MakeAspect<ORO_ControlKernel::PropertyExtension,
								   ORO_ControlKernel::KernelBaseFunction>::Result > nAxesSensorCartesianPosForce_typedef;
  
  class nAxesSensorCartesianPosForce
    : public nAxesSensorCartesianPosForce_typedef
  {
  public:
    nAxesSensorCartesianPosForce(unsigned int num_axes,
				 std::vector<ORO_DeviceInterface::AxisInterface*> axes,
				 ORO_DeviceInterface::SensorInterface<ORO_Geometry::Wrench>* forcesensor,
				 ORO_KinDyn::KinematicsComponent* kin,
				 const ORO_Geometry::Frame& MP_EE,
				 const ORO_Geometry::Frame& MP_FS,
				 std::string name);
    virtual ~nAxesSensorCartesianPosForce();
  
    // Redefining virtual members
    virtual bool componentLoaded();
    virtual bool componentStartup();
    virtual bool updateProperties(const ORO_ControlKernel::PropertyBag& bag);
    virtual void exportProperties(ORO_CoreLib::PropertyBag& bag);

    virtual void pull();
    virtual void calculate();
    virtual void push();

  private:
    unsigned int                                                          _num_axes;
    std::vector<ORO_DeviceInterface::AxisInterface*>                      _axes;
    std::vector< const ORO_DeviceInterface::SensorInterface<double>* >    _position_sensors;
    ORO_DeviceInterface::SensorInterface<ORO_Geometry::Wrench>*           _forcesensor;
    ORO_KinDyn::KinematicsComponent*                                      _kinematics;
    std::vector<double>                                                   _position_joint;

    ORO_Geometry::Frame                                                   _MP_EE, _MP_FS, _world_MP, _world_FS;
    ORO_Geometry::Wrench                                                  _force, _gravity;
    ORO_ControlKernel::DataObjectInterface< ORO_Geometry::Frame >*        _position_out_DOI;
    ORO_ControlKernel::DataObjectInterface< ORO_Geometry::Wrench >*       _force_out_DOI;

    bool                                                                  _properties_read;
    ORO_ControlKernel::Property< double >                                 _mass;    
    ORO_ControlKernel::Property< ORO_Geometry::Vector >                   _center_gravity;
    ORO_ControlKernel::Property< ORO_Geometry::Wrench >                   _offset;

  }; // class

} // end namespace ORO_nAxesControlKernel

#endif // __N_AXES_SENSOR_CARTESIAN_POS_FORCE_H__
