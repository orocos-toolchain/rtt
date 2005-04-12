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

#ifndef __N_AXES_SENSOR_KRYPTON_H__
#define __N_AXES_SENSOR_KRYPTON_H__

#include <control_kernel/BaseComponents.hpp>
#include <control_kernel/PropertyExtension.hpp>
#include <control_kernel/ReportingExtension.hpp>
#include <control_kernel/ExtensionComposition.hpp>
#include <corelib/PropertyComposition.hpp>
#include <rtnet/KryptonK600PositionInterfaceThread.hpp>


namespace ORO_ControlKernel
{

  // ---------------
  // -- DATA TYPE --
  // ---------------
  /**
   * @brief An nAxesSensorKrypton can write a vector of doubles.
   */
  class nAxesSensorKryptonInput_leds: public ORO_ControlKernel::ServedTypes<std::vector< ORO_Geometry::Vector> >
  {
  public:
    nAxesSensorKryptonInput_leds()
    {
      this->insert(ORO_ControlKernel::make_pair(0,"KryptonLeds"));
    }
  }; // class




  // ---------------
  // -- COMPONENT --
  // ---------------
  typedef ORO_ControlKernel::Sensor< ORO_ControlKernel::Writes<nAxesSensorKryptonInput_leds>,
				     ORO_ControlKernel::MakeFacet<ORO_ControlKernel::KernelBaseFunction>::Result > nAxesSensorKrypton_typedef;

  /**
   * @brief A SensorKrypton which reads num_led LED positions of a
   * Krypton camera system.
   * 
   * @ingroup kcomps kcomp_naxis kcomp_sensor
   *
   */
  class nAxesSensorKrypton
    : public nAxesSensorKrypton_typedef
  {
  public:
    /**
     * @brief Create a SensorKrypton which provides a vector of
     *  positions of Krypton LEDs.
     */
    nAxesSensorKrypton(unsigned int num_leds, std::string name);

    virtual ~nAxesSensorKrypton();
  
    // Redefining virtual members
    virtual void pull();
    virtual void calculate();
    virtual void push();
    virtual bool componentLoaded();
    virtual bool componentStartup();

  private:
    unsigned int                                                                        _num_leds;
    ORO_DeviceDriver::K600PositionInterface<double>                                     _krypton;
    std::vector<double>                                                                 _kryptondata;

    std::vector<ORO_Geometry::Vector>                                                   _leds_local;
    ORO_ControlKernel::DataObjectInterface< std::vector<ORO_Geometry::Vector> >*        _leds_DOI;

  }; // class

} // end namespace ORO_nAxesControlKernel

#endif // __N_AXES_SENSOR_KRYPTON_H__
