// $Id: nAxisGeneratorVel.hpp,v 1.1.1.1 2003/12/02 20:32:06 kgadeyne Exp $
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

#ifndef __N_AXES_GENERATOR_VEL_H__
#define __N_AXES_GENERATOR_VEL_H__

#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/ReportingExtension.hpp>
#include <control_kernel/PropertyExtension.hpp>
#include <control_kernel/ExecutionExtension.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <execution/TemplateCommandFactory.hpp>
#include <execution/TemplateMethodFactory.hpp>
#include <execution/TemplateMethodFactory.hpp>
#include <corelib/PropertyComposition.hpp>
#include <geometry/velocityprofile_trap.h>
#include <corelib/TimeService.hpp>
#include <os/MutexLock.hpp>

namespace ORO_ControlKernel
{

  // ---------------
  // -- DATA TYPE --
  // ---------------
  
  class nAxesGeneratorVelSetpoint_vel: public ORO_ControlKernel::ServedTypes<std::vector<double> >
  {
  public:
    nAxesGeneratorVelSetpoint_vel() 
    {
      this->insert(ORO_ControlKernel::make_pair(0,"Velocity"));
    }
  }; // class

  


  // ---------------
  // -- COMPONENT --
  // ---------------
  typedef ORO_ControlKernel::Generator< ORO_ControlKernel::Expects<ORO_ControlKernel::NoInput>,
					ORO_ControlKernel::Expects<ORO_ControlKernel::NoModel>,
					ORO_ControlKernel::Expects<ORO_ControlKernel::NoCommand>,
					ORO_ControlKernel::Writes<nAxesGeneratorVelSetpoint_vel>,
					ORO_ControlKernel::MakeAspect<ORO_ControlKernel::PropertyExtension,
								      ORO_ControlKernel::KernelBaseFunction,
								      ORO_ControlKernel::ExecutionExtension>::Result > nAxesGeneratorVel_typedef;
  

  class nAxesGeneratorVel
    : public nAxesGeneratorVel_typedef
  {
  public:
    nAxesGeneratorVel(unsigned int num_axes, std::string name);
    virtual ~nAxesGeneratorVel();

    virtual bool componentLoaded();
    virtual bool componentStartup();
    virtual bool updateProperties(const ORO_ControlKernel::PropertyBag& bag);
    virtual void exportProperties(ORO_CoreLib::PropertyBag&);

    virtual void pull();
    virtual void calculate();
    virtual void push();

    virtual CommandFactoryInterface* createCommandFactory();
    virtual MethodFactoryInterface*  createMethodFactory();

    bool setInitVelocity(const int axis, const double velocity);
    bool setInitVelocities(const std::vector<double>& velocity);
    bool velocityFinished(const int axis) const;
    bool velocitiesFinished() const;
    bool applyVelocity(const int axis, const double velocity, double duration=0);
    bool applyVelocities(const std::vector<double>& velocity, const std::vector<double>& duration);
    bool gotoVelocity(const int axis, const double velocity, double duration=0);
    bool gotoVelocities(const std::vector<double>& velocity, const std::vector<double>& duration);


  private:
    unsigned int                                                          _num_axes;
    std::vector<double>                                                   _duration_desired, _duration_trajectory, _velocity_local;
    std::vector<bool>                                                     _maintain_velocity;
    ORO_ControlKernel::DataObjectInterface< std::vector<double> >         *_velocity_DOI;
    std::vector<ORO_CoreLib::TimeService::ticks>                          _time_begin;
    std::vector<ORO_CoreLib::TimeService::Seconds>                        _time_passed;
    std::vector<ORO_Geometry::VelocityProfile_Trap*>                      _vel_profile;
    bool                                                                  _properties_read;
    ORO_ControlKernel::Property< std::vector<double> >                    _max_acc, _max_jerk;

  }; // class

} // namespace

#endif // __N_AXES_GENERATOR_VEL_H__
