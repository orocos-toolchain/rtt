// $Id: nAxisPosGenerator.hpp,v 1.1.1.1 2003/12/02 20:32:06 kgadeyne Exp $
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

#ifndef __N_AXES_GENERATOR_SIN_H__
#define __N_AXES_GENERATOR_SIN_H__

#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/ReportingExtension.hpp>
#include <control_kernel/PropertyExtension.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <corelib/PropertyComposition.hpp>
#include <geometry/velocityprofile_trap.h>
#include <corelib/HeartBeatGenerator.hpp>


namespace ORO_ControlKernel
{

  // ---------------
  // -- DATA TYPE --
  // ---------------
  class nAxesGeneratorSinSetpoint_pos_vel: public ORO_ControlKernel::ServedTypes<std::vector<double> >
  {
  public:
    nAxesGeneratorSinSetpoint_pos_vel() 
    {
      this->insert(ORO_ControlKernel::make_pair(0,"Position"));
      this->insert(ORO_ControlKernel::make_pair(0,"Velocity"));
    }
  }; // class


  class nAxesGeneratorSinInput_pos: public ORO_ControlKernel::ServedTypes<std::vector<double>, double >
  {
  public:
    nAxesGeneratorSinInput_pos() {}
  }; // class




  // ---------------
  // -- COMPONENT --
  // ---------------
  typedef ORO_ControlKernel::Generator< ORO_ControlKernel::Expects<nAxesGeneratorSinInput_pos>,
					ORO_ControlKernel::Expects<ORO_ControlKernel::NoModel>,
					ORO_ControlKernel::Expects<ORO_ControlKernel::NoCommand>,
					ORO_ControlKernel::Writes<nAxesGeneratorSinSetpoint_pos_vel>,
					ORO_ControlKernel::MakeAspect<ORO_ControlKernel::PropertyExtension,
								      ORO_ControlKernel::KernelBaseFunction>::Result > nAxesGeneratorSin_typedef;
  

  class nAxesGeneratorSin
    : public nAxesGeneratorSin_typedef
  {
  public:
    nAxesGeneratorSin(unsigned int num_axes, std::string name);
    virtual ~nAxesGeneratorSin();
    
    virtual bool componentLoaded();
    virtual bool componentStartup();
    virtual bool updateProperties(const ORO_ControlKernel::PropertyBag& bag);

    virtual void pull();
    virtual void calculate();
    virtual void push();

  private:
    unsigned int                                                          _num_axes;

    std::vector<double>                                                   _position_desired, _position_meas_local, _position_initial,  _position_local, _velocity_local;
    ORO_ControlKernel::DataObjectInterface< std::vector<double> >         *_position_meas_DOI, *_position_DOI, *_velocity_DOI;

    ORO_CoreLib::HeartBeatGenerator::ticks                                _time_begin;
    ORO_CoreLib::HeartBeatGenerator::Seconds                              _time_passed;
    
    bool                                                                  _properties_read, _is_initialized;
    ORO_ControlKernel::Property< std::vector<double> >                    _sin_frequency, _sin_amplitude, _sin_phase;

    

  }; // class

} // namespace

#endif // __N_AXES_GENERATOR_SIN_H__
