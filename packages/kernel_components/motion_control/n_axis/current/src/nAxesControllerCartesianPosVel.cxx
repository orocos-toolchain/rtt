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

#include "kernel_components/nAxesControllerCartesianPosVel.hpp"
#include <assert.h>
#include <geometry/frames_io.h>


namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;

  nAxesControllerCartesianPosVel::nAxesControllerCartesianPosVel(std::string name)
    : nAxesControllerCartesianPosVel_typedef(name),
      _properties_read(false),
      _controller_gain("K", "Proportional Gain")
  {}


  nAxesControllerCartesianPosVel::~nAxesControllerCartesianPosVel(){};
  

  void nAxesControllerCartesianPosVel::pull()
  {
    // copy Input and Setpoint to local values
    _position_meas_DOI->Get(_position_meas_local);
    _position_desi_DOI->Get(_position_desi_local);
    _velocity_desi_DOI->Get(_velocity_desi_local);
  }



  void nAxesControllerCartesianPosVel::calculate()
  {
    // feedback on position
    _velocity_feedback = diff(_position_meas_local, _position_desi_local);
    for(unsigned int i=0; i<6; i++)
      _velocity_feedback(i) *= _controller_gain.value()[i];

    // feedback + feedforward
    //    cout << "_velocity_desi_local " << _velocity_desi_local << endl;
    //    cout << "_velocity_feedback   " << _velocity_feedback << endl;
    
    _velocity_out_local = _velocity_desi_local + _velocity_feedback;
  }
  
  
  void nAxesControllerCartesianPosVel::push()      
  {
    _velocity_out_DOI->Set(_velocity_out_local);
  }


  bool nAxesControllerCartesianPosVel::componentLoaded()
  {
    // get interface to Output data types
    if ( !nAxesControllerCartesianPosVel_typedef::Output::dObj()->Get("Velocity", _velocity_out_DOI) ){
      cerr << "nAxesControllerCartesianPosVel::componentLoaded() DataObjectInterface not found" << endl;
      return false;
    }
    return true;
  }


  bool nAxesControllerCartesianPosVel::componentStartup()
  {
    // check if updateProperties has been called
    assert(_properties_read);

    // get interface to Input/Setpoint data types
    if ( !nAxesControllerCartesianPosVel_typedef::Input::dObj(   )->Get("Position", _position_meas_DOI) ||
	 !nAxesControllerCartesianPosVel_typedef::SetPoint::dObj()->Get("Position", _position_desi_DOI) ||
	 !nAxesControllerCartesianPosVel_typedef::SetPoint::dObj()->Get("Velocity", _velocity_desi_DOI) ){
      cerr << "nAxesControllerCartesianPosVel::componentStartup() DataObjectInterface not found" << endl;
      return false;
    }
    return true;
  }
  

  bool nAxesControllerCartesianPosVel::updateProperties(const PropertyBag& bag)
  {
    // properties have been read
    _properties_read = true;

    // get properties
    if (!composeProperty(bag, _controller_gain) ){
      cerr << "nAxesVelController::updateProperties() failed" << endl;
      return false;
    }

    // check size of properties
    assert(_controller_gain.value().size() >= 6);

    return true;
  }

  

} // namespace
