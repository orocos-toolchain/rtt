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

#ifndef __N_AXES_GENERATOR_CARTESIAN_SIN_H__
#define __N_AXES_GENERATOR_CARTESIAN_SIN_H__

#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/ReportingExtension.hpp>
#include <control_kernel/PropertyExtension.hpp>
#include <control_kernel/ExecutionExtension.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <execution/TemplateCommandFactory.hpp>
#include <execution/TemplateMethodFactory.hpp>
#include <corelib/PropertyComposition.hpp>
#include <geometry/velocityprofile_traphalf.h>
#include <corelib/TimeService.hpp>
#include <os/MutexLock.hpp>
#include <geometry/frames.h>

namespace ORO_ControlKernel
{

  // ---------------
  // -- DATA TYPE --
  // ---------------
  class nAxesGeneratorCartesianSinSetpoint_pos_vel: public ORO_ControlKernel::ServedTypes< ORO_Geometry::Frame, ORO_Geometry::Twist >
  {
  public:
    nAxesGeneratorCartesianSinSetpoint_pos_vel() 
    {
      this->insert(ORO_ControlKernel::make_pair(0,"Frame"));
      this->insert(ORO_ControlKernel::make_pair(1,"Twist"));
    }
  }; // class


  class nAxesGeneratorCartesianSinInput_pos: public ORO_ControlKernel::ServedTypes< ORO_Geometry::Frame >
  {
  public:
    nAxesGeneratorCartesianSinInput_pos() {}
  }; // class




  // ---------------
  // -- COMPONENT --
  // ---------------
  typedef ORO_ControlKernel::Generator< ORO_ControlKernel::Expects<ORO_ControlKernel::NoInput>,
					ORO_ControlKernel::Expects<ORO_ControlKernel::NoModel>,
					ORO_ControlKernel::Expects<ORO_ControlKernel::NoCommand>,
					ORO_ControlKernel::Writes<nAxesGeneratorCartesianSinSetpoint_pos_vel>,
					ORO_ControlKernel::MakeFacet<ORO_ControlKernel::PropertyExtension,
								      ORO_ControlKernel::KernelBaseFunction,
								      ORO_ControlKernel::ExecutionExtension>::Result > nAxesGeneratorCartesianSin_typedef;  

  class nAxesGeneratorCartesianSin
    : public nAxesGeneratorCartesianSin_typedef
  {
  public:
    nAxesGeneratorCartesianSin(std::string name);
    virtual ~nAxesGeneratorCartesianSin();

    virtual bool componentLoaded();
    virtual bool componentStartup();
    virtual bool updateProperties(const ORO_ControlKernel::PropertyBag& bag);
    virtual void exportProperties(ORO_CoreLib::PropertyBag&);

    virtual void pull();
    virtual void calculate();
    virtual void push();

    virtual CommandFactoryInterface* createCommandFactory();
    virtual MethodFactoryInterface*  createMethodFactory();
    bool move(double time=0);
    bool moveFinished() const;
    void reset();
    void setAmplitude(std::vector<double> amplitude);
    void setPulsation(std::vector<double> pulsation);
    void setPhase(std::vector<double> phase);

  private:
    ORO_Geometry::Frame                                                   _start_frame;
    ORO_Geometry::Frame                                                   _position_desired, _position_out_local;
    ORO_Geometry::Twist                                                   _velocity_out_local, _velocity_begin_end, _velocity_delta;
    ORO_ControlKernel::DataObjectInterface< ORO_Geometry::Frame >         *_position_meas_DOI, *_position_out_DOI;
    ORO_ControlKernel::DataObjectInterface< ORO_Geometry::Twist >         *_velocity_out_DOI;

    std::vector<ORO_Geometry::VelocityProfile_TrapHalf*>                  _motion_profile;
    ORO_CoreLib::TimeService::ticks                                _time_begin;
    ORO_CoreLib::TimeService::Seconds                              _time_passed;
    mutable ORO_OS::Mutex                                                 _my_lock;
    double                                                                _max_duration, _traject_duration;
    
    bool                                                                  _properties_read, _is_moving, _new_values, _is_initialized;
    ORO_ControlKernel::Property< std::vector<double> >                    _maximum_velocity, _maximum_acceleration;
    std::vector<double>                                                   _amplitude;
    std::vector<double>                                                   _pulsation;
    std::vector<double>                                                   _phase;

    

  }; // class

} // namespace

#endif // __N_AXES_GENERATOR_CARTESIAN_SIN_H__
