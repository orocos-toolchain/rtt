// $Id: nAxesSensorCartesianPos.hpp,v 1.1.1.1 2003/12/02 20:32:06 kgadeyne Exp $
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

#ifndef __N_AXES_SENSOR_CARTESIAN_POS_H__
#define __N_AXES_SENSOR_CARTESIAN_POS_H__

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
  class nAxesSensorCartesianPosInput_pos: public ORO_ControlKernel::ServedTypes< ORO_Geometry::Frame >
  {
  public:
    nAxesSensorCartesianPosInput_pos()
    {
      this->insert(ORO_ControlKernel::make_pair(0,"Position"));
    }
  }; // class




  // ---------------
  // -- COMPONENT --
  // ---------------
  typedef ORO_ControlKernel::Sensor< ORO_ControlKernel::Writes<nAxesSensorCartesianPosInput_pos>,
				     ORO_ControlKernel::MakeAspect<ORO_ControlKernel::PropertyExtension,
								   ORO_ControlKernel::KernelBaseFunction>::Result > nAxesSensorCartesianPos_typedef;

  class nAxesSensorCartesianPos
    : public nAxesSensorCartesianPos_typedef
  {
  public:
    nAxesSensorCartesianPos(unsigned int num_axes,
			    std::vector<ORO_DeviceInterface::AxisInterface*> axes,
			    ORO_KinDyn::KinematicsComponent* kin,
			    std::string name);
    virtual ~nAxesSensorCartesianPos();
  
    // Redefining virtual members
    virtual void pull();
    virtual void calculate();
    virtual void push();
    virtual bool componentLoaded();
    virtual bool componentStartup();

  private:
    unsigned int                                                          _num_axes;
    std::vector<ORO_DeviceInterface::AxisInterface*>                      _axes;
    ORO_KinDyn::KinematicsComponent*                                      _kinematics;
    std::vector<double>                                                   _position_joint;

    ORO_Geometry::Frame                                                   _position_out_local;
    ORO_ControlKernel::DataObjectInterface< ORO_Geometry::Frame >*        _position_out_DOI;
    std::vector< const ORO_DeviceInterface::SensorInterface<double>* >    _position_sensors;

  }; // class

} // end namespace ORO_nAxesControlKernel

#endif // __N_AXES_SENSOR_CARTESIAN_POS_H__
