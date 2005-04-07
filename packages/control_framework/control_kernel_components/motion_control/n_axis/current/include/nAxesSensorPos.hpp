// $Id: nAxesSensorPos.hpp,v 1.1.1.1 2003/12/02 20:32:06 kgadeyne Exp $
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

/**
 * @defgroup kcomp_naxis All Available Control Kernel nAxis Components.
 */

#ifndef __N_AXES_SENSOR_POS_H__
#define __N_AXES_SENSOR_POS_H__

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
  /**
   * @brief An nAxesSensorPos can write a vector of doubles.
   */
  class nAxesSensorPosInput_pos: public ORO_ControlKernel::ServedTypes<std::vector<double> >
  {
  public:
    nAxesSensorPosInput_pos()
    {
      this->insert(ORO_ControlKernel::make_pair(0,"Position"));
    }
  }; // class




  // ---------------
  // -- COMPONENT --
  // ---------------
  typedef ORO_ControlKernel::Sensor< ORO_ControlKernel::Writes<nAxesSensorPosInput_pos>,
				     ORO_ControlKernel::MakeFacet<ORO_ControlKernel::KernelBaseFunction>::Result > nAxesSensorPos_typedef;

  /**
   * @brief A Joint space SensorPos which reads the joint positions
   * from a vector of AxisInterfaces.
   * 
   * @ingroup kcomps kcomp_naxis kcomp_sensor
   *
   */
  class nAxesSensorPos
    : public nAxesSensorPos_typedef
  {
  public:
    /**
     * @brief Create a SensorPos with num_axes axis and a vector of
     * AxisInterfaces to access Position encoders.
     */
    nAxesSensorPos(unsigned int num_axes, std::vector<ORO_DeviceInterface::AxisInterface*> axes, std::string name);

    virtual ~nAxesSensorPos();
  
    // Redefining virtual members
    virtual void pull();
    virtual void calculate();
    virtual void push();
    virtual bool componentLoaded();
    virtual bool componentStartup();

  private:
    unsigned int                                                          _num_axes;
    std::vector<ORO_DeviceInterface::AxisInterface*>                      _axes;

    std::vector<double>                                                   _position_local;
    ORO_ControlKernel::DataObjectInterface< std::vector<double> >*        _position_DOI;
    std::vector< const ORO_DeviceInterface::SensorInterface<double>* >    _position_sensors;

  }; // class

} // end namespace ORO_nAxesControlKernel

#endif // __N_AXES_SENSOR_POS_H__
