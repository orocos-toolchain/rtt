// $Id: nAxisEffectorVel.hpp,v 1.1.1.1 2003/12/02 20:32:06 kgadeyne Exp $
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

#ifndef __N_AXES_EFFECTOR_VEL_H__
#define __N_AXES_EFFECTOR_VEL_H__

#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/PropertyExtension.hpp>
#include <control_kernel/ReportingExtension.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <corelib/PropertyComposition.hpp>
#include <device_interface/AxisInterface.hpp>

namespace ORO_ControlKernel
{

  // ---------------
  // -- DATA TYPE --
  // ---------------
  class nAxesEffectorVelOutput_vel: public ORO_ControlKernel::ServedTypes<std::vector<double> >
  {
  public:
    nAxesEffectorVelOutput_vel() {}
  }; // class




  // ---------------
  // -- COMPONENT --
  // ---------------
  typedef ORO_ControlKernel::Effector< ORO_ControlKernel::Expects<nAxesEffectorVelOutput_vel>,
				       ORO_ControlKernel::MakeAspect<ORO_ControlKernel::PropertyExtension,
								     ORO_ControlKernel::KernelBaseFunction>::Result > nAxesEffectorVel_typedef;

  class nAxesEffectorVel
    : public nAxesEffectorVel_typedef
  {
  public:
    nAxesEffectorVel(unsigned int num_axes, std::vector<ORO_DeviceInterface::AxisInterface*> axes, std::string name);
    virtual ~nAxesEffectorVel();

    virtual bool componentLoaded();
    virtual bool componentStartup();
    virtual bool updateProperties(const ORO_ControlKernel::PropertyBag& bag);

    virtual void pull();
    virtual void calculate();
    virtual void push();


  private:
    unsigned int                                                          _num_axes;
    std::vector<ORO_DeviceInterface::AxisInterface*>                      _axes;

    std::vector<double>                                                   _velocity_local;
    ORO_ControlKernel::DataObjectInterface< std::vector<double> >         *_velocity_DOI;

  }; // class

} // namespace

#endif // __N_AXES_EFFECTOR_VEL_H__
