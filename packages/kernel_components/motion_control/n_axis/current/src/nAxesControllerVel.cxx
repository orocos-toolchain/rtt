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

#include "kernel_components/nAxesControllerVel.hpp"
#include <assert.h>

namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;


  nAxesControllerVel::nAxesControllerVel(unsigned int num_axes, 
					 std::string name)
    : nAxesControllerVel_typedef(name),
      _num_axes(num_axes), 
      _position_meas_local(num_axes),
      _velocity_desi_local(num_axes),
      _velocity_local(num_axes),
      _position_desired(num_axes),
      _properties_read(false),
      _controller_gain("K", "Proportional Gain")
  {}


  nAxesControllerVel::~nAxesControllerVel(){};
  

  void nAxesControllerVel::pull()
  {
    // copy Input and Setpoint to local values
    _position_meas_DOI->Get(_position_meas_local);
    _velocity_desi_DOI->Get(_velocity_desi_local);
  }



  void nAxesControllerVel::calculate()
  {
    // initialize
    if (!_is_initialized){
      _is_initialized = true;
      for (unsigned int i=0; i<_num_axes; i++)
	_position_desired[i] = _position_meas_local[i];
      _time_begin = HeartBeatGenerator::Instance()->ticksGet();
    }

    // position feedback on integrated velocity
    double time_difference = HeartBeatGenerator::Instance()->secondsSince(_time_begin);
    for(unsigned int i=0; i<_num_axes; i++){
      _position_desired[i] += _velocity_desi_local[i] * time_difference;
      _velocity_local[i] = (_controller_gain.value()[i] * (_position_desired[i] - _position_meas_local[i])) + _velocity_desi_local[i];
    }
    _time_begin = HeartBeatGenerator::Instance()->ticksGet();
  }

  
  void nAxesControllerVel::push()      
  {
    _velocity_DOI->Set(_velocity_local);
  }


  bool nAxesControllerVel::componentLoaded()
  {
    // get interface to Output data types
    if ( !nAxesControllerVel_typedef::Output::dObj()->Get("Velocity", _velocity_DOI) ){
      cerr << "nAxesControllerVel::componentLoaded() DataObjectInterface not found" << endl;
      return false;
    }
    return true;
  }


  bool nAxesControllerVel::componentStartup()
  {
    // check if updateProperties has been called
    if (!_properties_read){
      cerr << "nAxesControllerVel::componentStartup() Properties have not been read." << endl;
      return false;
    }

    // reset integrator
    _is_initialized = false;

    // get interface to Input/Setpoint data types
    if ( !nAxesControllerVel_typedef::Input::dObj(   )->Get("Position", _position_meas_DOI) ||
	 !nAxesControllerVel_typedef::SetPoint::dObj()->Get("Velocity", _velocity_desi_DOI) ){
      cerr << "nAxesControllerVel::componentStartup() DataObjectInterface not found" << endl;
      return false;
    }
    return true;
  }
  

  bool nAxesControllerVel::updateProperties(const PropertyBag& bag)
  {
    // properties have been read
    _properties_read = true;

    // get properties
    if (!composeProperty(bag, _controller_gain) ){
      cerr << "nAxesControllerVel::updateProperties() failed" << endl;
      return false;
    }

    // check size of properties
    assert(_controller_gain.value().size() >= _num_axes);

    return true;
  }

  
  void nAxesControllerVel::exportProperties(ORO_CoreLib::PropertyBag&)
  {};


} // namespace
