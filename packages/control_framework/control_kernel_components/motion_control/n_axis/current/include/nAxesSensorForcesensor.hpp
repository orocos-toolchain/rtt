// $Id: nAxesSensorForcesensor.hpp,v 1.1.1.1 2003/12/02 20:32:06 kgadeyne Exp $
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

#ifndef __N_AXES_SENSOR_FORCE_H__
#define __N_AXES_SENSOR_FORCE_H__

#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/ReportingExtension.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <control_kernel/ExecutionExtension.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <execution/TemplateCommandFactory.hpp>
#include <execution/TemplateMethodFactory.hpp>
#include <corelib/PropertyComposition.hpp>
#include <device_interface/AxisInterface.hpp>
#include <kindyn/KinematicsComponent.hpp>

namespace ORO_ControlKernel
{

  // ---------------
  // -- DATA TYPE --
  // ---------------
  class nAxesSensorForcesensorInput_pos_force: public ORO_ControlKernel::ServedTypes< ORO_Geometry::Frame, ORO_Geometry::Wrench, std::vector<double> >
  {
  public:
    nAxesSensorForcesensorInput_pos_force()
    {
      this->insert(ORO_ControlKernel::make_pair(0,"Position_EE"));
      this->insert(ORO_ControlKernel::make_pair(1,"ForcesensorForce"));
      this->insert(ORO_ControlKernel::make_pair(2,"Position_joint"));
    }
  }; // class




  // ---------------
  // -- COMPONENT --
  // ---------------
  typedef ORO_ControlKernel::Sensor< ORO_ControlKernel::Writes<nAxesSensorForcesensorInput_pos_force>,
				     ORO_ControlKernel::MakeFacet< ORO_ControlKernel::KernelBaseFunction,
								   ORO_ControlKernel::ExecutionExtension>::Result > nAxesSensorForcesensor_typedef;
  

  /// Component returns force measurement and frame of froce sensor
  /** This component reads the force sensor
  */
  class nAxesSensorForcesensor
    : public nAxesSensorForcesensor_typedef
  {
  public:
    nAxesSensorForcesensor(unsigned int num_axes,
			   std::vector<ORO_DeviceInterface::AxisInterface*> axes,
			   ORO_DeviceInterface::SensorInterface<ORO_Geometry::Wrench>* forcesensor,
			   ORO_KinDyn::KinematicsComponent* kin,
			   const ORO_Geometry::Frame& MP_FS,
			   std::string name);
    virtual ~nAxesSensorForcesensor();
  
    // Redefining virtual members
    virtual bool componentLoaded();
    virtual bool componentStartup();


    virtual void pull();
    virtual void calculate();
    virtual void push();

    virtual CommandFactoryInterface* createCommandFactory();
    virtual MethodFactoryInterface*  createMethodFactory();
    bool startMeasuring(int num_meas);
    bool finishedMeasuring() const;
    ORO_Geometry::Wrench getMeasurement() const;
    

  private:
    unsigned int                                                          _num_axes, _num_measurements, _counter_measurements;
    std::vector<ORO_DeviceInterface::AxisInterface*>                      _axes;
    std::vector< const ORO_DeviceInterface::SensorInterface<double>* >    _position_sensors;
    ORO_DeviceInterface::SensorInterface<ORO_Geometry::Wrench>*           _forcesensor;
    ORO_KinDyn::KinematicsComponent*                                      _kinematics;
    std::vector<double>                                                   _position_joint;
    bool                                                                  _is_measuring;
    ORO_Geometry::Wrench                                                  _average_measurement;
    

    ORO_Geometry::Frame                                                   _MP_FS, _world_MP;
    ORO_Geometry::Wrench                                                  _force;
    ORO_ControlKernel::DataObjectInterface< ORO_Geometry::Frame >*        _position_EE_DOI;
    ORO_ControlKernel::DataObjectInterface< std::vector<double> >*        _position_joint_DOI;
    ORO_ControlKernel::DataObjectInterface< ORO_Geometry::Wrench >*       _force_out_DOI;


  }; // class

} // end namespace ORO_nAxesControlKernel

#endif // __N_AXES_SENSOR_FORCE_H__
