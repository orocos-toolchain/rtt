// $Id: nAxisEffectorCartesianVel.hpp,v 1.1.1.1 2003/12/02 20:32:06 kgadeyne Exp $
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

#ifndef __N_AXES_EFFECTOR_CARTESIAN_VEL_H__
#define __N_AXES_EFFECTOR_CARTESIAN_VEL_H__

#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/PropertyExtension.hpp>
#include <control_kernel/ReportingExtension.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <corelib/PropertyComposition.hpp>
#include <device_interface/AxisInterface.hpp>
#include <geometry/frames.h>
#include <kindyn/KinematicsComponent.hpp>

namespace ORO_ControlKernel
{

  // ---------------
  // -- DATA TYPE --
  // ---------------
  /**
   * @brief An nAxesEffectorCartesianVel can read a Twist.
   */
  class nAxesEffectorCartesianVelOutput_vel: public ORO_ControlKernel::ServedTypes< ORO_Geometry::Twist >
  {
  public:
    nAxesEffectorCartesianVelOutput_vel() {}
  }; // class




  // ---------------
  // -- COMPONENT --
  // ---------------
  typedef ORO_ControlKernel::Effector< ORO_ControlKernel::Expects<NoInput>,
				       ORO_ControlKernel::Expects<NoModel>,
				       ORO_ControlKernel::Expects<nAxesEffectorCartesianVelOutput_vel>,
				       ORO_ControlKernel::MakeFacet<ORO_ControlKernel::KernelBaseFunction>::Result > nAxesEffectorCartesianVel_typedef;

  /**
   * @brief A Cartesian EffectorVel which sends a desired velocity
   * to each of its AxisInterfaces, using the inverse kinematics.
   * 
   * @ingroup kcomps kcomp_naxis kcomp_effector
   */
  class nAxesEffectorCartesianVel
    : public nAxesEffectorCartesianVel_typedef
  {
  public:
    /**
     * @brief Create an EffectorVel with num_axes axis, a
     * kinematicsComponent to get the inverse kinematics, and a vector of
     * AxisInterfaces to access analog drives.
     */
    nAxesEffectorCartesianVel(unsigned int num_axes, 
			      std::vector<ORO_DeviceInterface::AxisInterface*> axes,
			      ORO_KinDyn::KinematicsComponent* kin,
			      std::string name);
    virtual ~nAxesEffectorCartesianVel();

    virtual bool componentLoaded();
    virtual bool componentStartup();

    virtual void pull();
    virtual void calculate();
    virtual void push();


  private:
    unsigned int                                                          _num_axes;
    std::vector<ORO_DeviceInterface::AxisInterface*>                      _axes;
    ORO_KinDyn::KinematicsComponent*                                      _kinematics;

    std::vector<double>                                                   _velocity_joint_local;
    ORO_Geometry::Twist                                                   _velocity_cartesian_local;
    ORO_Geometry::Frame                                                   _position_cartesian;
    ORO_ControlKernel::DataObjectInterface< ORO_Geometry::Twist >         *_velocity_cartesian_DOI;
    std::vector< const ORO_DeviceInterface::SensorInterface<double>* >    _position_sensors;

  }; // class

} // namespace

#endif // __N_AXES_EFFECTOR_CARTESIAN_VEL_H__
