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

#include "kernel_components/nAxesGeneratorSin.hpp"
#include <assert.h>


namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_Geometry;
  

  nAxesGeneratorSin::nAxesGeneratorSin(unsigned int num_axes,  std::string name)
    : nAxesGeneratorSin_typedef(name),
      _num_axes(num_axes), 
      _position_desired(num_axes),
      _position_meas_local(num_axes),
      _position_initial(num_axes),
      _position_local(num_axes),
      _velocity_local(num_axes),
      _properties_read(false),
      _sin_frequency("frequency", "Frequency of sin"),
      _sin_amplitude("amplitude", "Amplitude of sin"),
      _sin_phase("phase", "Phase of sin")
  {}
  


  nAxesGeneratorSin::~nAxesGeneratorSin()
  {}
  
  

  void nAxesGeneratorSin::pull()
  {
    // initialize
    if (!_is_initialized){
      _is_initialized = true;
      _position_meas_DOI->Get(_position_meas_local);
      for (unsigned int i=0; i<_num_axes; i++)
	_position_initial[i] = _position_meas_local[i] - ( _sin_amplitude.value()[i] * sin(_sin_phase.value()[i]) );

      _time_begin = HeartBeatGenerator::Instance()->ticksGet();
    }
  }


  void nAxesGeneratorSin::calculate()
  {
    _time_passed = HeartBeatGenerator::Instance()->secondsSince(_time_begin);
    for (unsigned int i=0; i<_num_axes; i++){
      _position_local[i] = _position_initial[i] + (_sin_amplitude.value()[i] * sin( (_sin_frequency.value()[i] * _time_passed) + _sin_phase.value()[i] ));
      _velocity_local[i] = _sin_amplitude.value()[i] * _sin_frequency.value()[i] * cos( (_sin_frequency.value()[i] * _time_passed) + _sin_phase.value()[i] );
    }
  }
  

  
  void nAxesGeneratorSin::push()      
  {
    _position_DOI->Set(_position_local);
    _velocity_DOI->Set(_velocity_local);
  }



  bool nAxesGeneratorSin::componentLoaded()
  {
    // get interface to Setpoint data types
    if ( !nAxesGeneratorSin_typedef::SetPoint::dObj()->Get("Position", _position_DOI) ||
	 !nAxesGeneratorSin_typedef::SetPoint::dObj()->Get("Velocity", _velocity_DOI) ){
      cerr << "nAxesGeneratorSin::componentLoaded() DataObjectInterface not found" << endl;
      return false;
    }
    
    // set empty values
    vector<double> _temp_vector(_num_axes);
    _position_DOI->Set(_temp_vector);
    _velocity_DOI->Set(_temp_vector);

    return true;
  }



  bool nAxesGeneratorSin::componentStartup()
  {
    // check if updateProperties has been called
    if (!_properties_read){
      cerr << "nAxesGeneratorSin::componentStartup() Properties have not been read." << endl;
      return false;
    }

    // initialize
    _is_initialized = false;

    // get interface to Cammand / Model / Input data types
    if ( !nAxesGeneratorSin_typedef::Input::dObj()->Get("Position", _position_meas_DOI) ){
      cerr << "nAxesGeneratorSin::componentStartup() DataObjectInterface not found" << endl;
      return false;
    }

    return true;
  }


  
  bool nAxesGeneratorSin::updateProperties(const ORO_ControlKernel::PropertyBag& bag)
  {
    // properties have been read
    _properties_read = true;

    // get properties
    if ( !composeProperty(bag, _sin_amplitude) ||
	 !composeProperty(bag, _sin_frequency) ||
	 !composeProperty(bag, _sin_phase) ){
      cerr << "nAxesGeneratorSin::updateProperties() failed" << endl;
      return false;
    }

    // check size of properties
    assert(_sin_amplitude.value().size() == _num_axes);
    assert(_sin_frequency.value().size() == _num_axes);
    assert(_sin_phase.value().size()     == _num_axes);

    return true;
  }
  

} // namespace

