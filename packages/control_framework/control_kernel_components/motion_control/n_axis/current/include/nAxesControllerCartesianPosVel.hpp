// $Id: nAxisPosVelController.hpp,v 1.1.1.1 2003/12/02 20:32:06 kgadeyne Exp $
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

#ifndef __N_AXES_CARTESIAN_POS_VEL_CONTROLLER_H__
#define __N_AXES_CARTESIAN_POS_VEL_CONTROLLER_H__

#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/ReportingExtension.hpp>
#include <control_kernel/PropertyExtension.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <corelib/PropertyComposition.hpp>
#include <geometry/frames.h>


namespace ORO_ControlKernel
{

  // ---------------
  // -- DATA TYPE --
  // ---------------
  class nAxesControllerCartesianPosVelInput_pos: public ORO_ControlKernel::ServedTypes< ORO_Geometry::Frame >
  {
  public:
    nAxesControllerCartesianPosVelInput_pos() {}
  }; // class


  class nAxesControllerCartesianPosVelSetpoint_pos_vel: public ORO_ControlKernel::ServedTypes< ORO_Geometry::Frame, ORO_Geometry::Twist >
  {
  public:
    nAxesControllerCartesianPosVelSetpoint_pos_vel() {}
  }; // class


  class nAxesControllerCartesianPosVelOutput_vel: public ORO_ControlKernel::ServedTypes< ORO_Geometry::Twist >
  {
  public:
    nAxesControllerCartesianPosVelOutput_vel()
    {
      this->insert(ORO_ControlKernel::make_pair(0,"Twist"));
      this->insert(ORO_ControlKernel::make_pair(0,"TrackingError"));
    }
  }; // class




  // ---------------
  // -- COMPONENT --
  // ---------------
  typedef ORO_ControlKernel::Controller< ORO_ControlKernel::Expects<nAxesControllerCartesianPosVelInput_pos>,
					 ORO_ControlKernel::Expects<ORO_ControlKernel::NoModel>,
					 ORO_ControlKernel::Expects<nAxesControllerCartesianPosVelSetpoint_pos_vel>,
					 ORO_ControlKernel::Writes<nAxesControllerCartesianPosVelOutput_vel>,
					 ORO_ControlKernel::MakeFacet<ORO_ControlKernel::PropertyExtension,
								       ORO_ControlKernel::KernelBaseFunction>::Result > nAxesControllerCartesianPosVel_typedef;

  class nAxesControllerCartesianPosVel
    : public nAxesControllerCartesianPosVel_typedef
  {
  public:
    nAxesControllerCartesianPosVel(std::string name);
    virtual ~nAxesControllerCartesianPosVel();

    virtual bool componentLoaded();
    virtual bool componentStartup();
    virtual bool updateProperties(const ORO_ControlKernel::PropertyBag& bag);
    virtual void exportProperties(ORO_CoreLib::PropertyBag&);

    virtual void pull();
    virtual void calculate();
    virtual void push();

  private:
    ORO_Geometry::Frame                                                   _position_meas_local, _position_desi_local;
    ORO_Geometry::Twist                                                   _velocity_out_local, _velocity_desi_local, _velocity_feedback;
    
    ORO_ControlKernel::DataObjectInterface< ORO_Geometry::Frame >         *_position_meas_DOI,  *_position_desi_DOI;
    ORO_ControlKernel::DataObjectInterface< ORO_Geometry::Twist >         *_velocity_desi_DOI, *_velocity_out_DOI, *_tracking_error_DOI;

    bool                                                                  _properties_read;
    ORO_ControlKernel::Property< std::vector<double> >                    _controller_gain;

  }; // class

} // namespace

#endif // __N_AXES_CARTESIAN_POS_VEL_CONTROLLER_H__
