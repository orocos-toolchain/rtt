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


#include "kernel_components/nAxesControllerPos.hpp"
#include <assert.h>

namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_Execution;


  nAxesControllerPos::nAxesControllerPos(unsigned int num_axes, 
					 std::string name)
    : nAxesControllerPos_typedef(name),
      _num_axes(num_axes), 
      _position_meas_local(num_axes),
      _position_meas_old(num_axes),
      _position_desi_local(num_axes),
      _velocity_local(num_axes),
      _offset_measurement(num_axes),
      _properties_read(false),
      _is_measuring(false),
      _controller_gain("K", "Proportional Gain")
  {}


  nAxesControllerPos::~nAxesControllerPos(){};
  

  void nAxesControllerPos::pull()
  {
    // copy Input and Setpoint to local values
    _position_meas_DOI->Get(_position_meas_local);
    _position_desi_DOI->Get(_position_desi_local);
  }


  void nAxesControllerPos::calculate()
  {
    for(unsigned int i=0; i<_num_axes; i++)
      _velocity_local[i] = _controller_gain.value()[i] * (_position_desi_local[i] - _position_meas_local[i]);

    if (_is_initialized && _is_measuring){
      bool is_moving = false;
      for (unsigned int i=0; i<_num_axes; i++)
	if (abs(_position_meas_local[i] - _position_meas_old[i]) > _treshold_moving) is_moving = true;
      if ( !is_moving ){
	for (unsigned int i=0; i<_num_axes; i++)
	  _offset_measurement[i] = _velocity_local[i];
	_is_measuring = false;
      }
    }
  }
  

  
  void nAxesControllerPos::push()      
  {
    // remember old position
    for(unsigned int i=0; i<_num_axes; i++)
      _position_meas_old[i] = _position_meas_local[i];
    _is_initialized = true;

    _velocity_DOI->Set(_velocity_local);
  }


  bool nAxesControllerPos::componentLoaded()
  {
    // get interface to Output data types
    if ( !nAxesControllerPos_typedef::Output::dObj()->Get("Velocity", _velocity_DOI) ){
      cerr << "nAxesControllerPos::componentLoaded() DataObjectInterface not found" << endl;
      return false;
    }

    // set empty values
    vector<double> _temp_vector(_num_axes);
    _velocity_DOI->Set(_temp_vector);

    return true;
  }


  bool nAxesControllerPos::componentStartup()
  {
    // check if updateProperties has been called
    if (!_properties_read){
      cerr << "nAxesControllerPos::componentStartup() Properties have not been read." << endl;
      return false;
    }

    // get interface to Input/Setpoint data types
    if ( !nAxesControllerPos_typedef::Input::dObj(   )->Get("Position", _position_meas_DOI) ||
	 !nAxesControllerPos_typedef::SetPoint::dObj()->Get("Position", _position_desi_DOI) ){
      cerr << "nAxesControllerPos::componentStartup() DataObjectInterface not found" << endl;
      return false;
    }

    _is_initialized = false;
    
    return true;
  }
  

  bool nAxesControllerPos::updateProperties(const PropertyBag& bag)
  {
    // properties have been read
    _properties_read = true;

    // get properties
    if (!composeProperty(bag, _controller_gain) ){
      cerr << "nAxesControllerPos::updateProperties() failed" << endl;
      return false;
    }

    // check size of properties
    assert(_controller_gain.value().size() >= _num_axes);

    return true;
  }


  void nAxesControllerPos::exportProperties(ORO_CoreLib::PropertyBag&)
  {};
  


  CommandFactoryInterface* nAxesControllerPos::createCommandFactory()
  {
    TemplateCommandFactory<nAxesControllerPos>* my_commandFactory = newCommandFactory( this );
    my_commandFactory->add( "offsetMeasure", command( &nAxesControllerPos::startMeasuring,
						      &nAxesControllerPos::finishedMeasuring,
						      "calculate the velocity offset on the axes",
						      "treshold_moving", "treshold to check if axis is moving or not"));
    return my_commandFactory;
  }


  MethodFactoryInterface* nAxesControllerPos::createMethodFactory()
  {
    TemplateMethodFactory<nAxesControllerPos>* my_methodFactory = newMethodFactory( this );
    my_methodFactory->add( "getMeasurement", method( &nAxesControllerPos::getMeasurement, "Get offset measurements"));

    return my_methodFactory;
  }



  bool nAxesControllerPos::startMeasuring(double treshold_moving)
  {
    // don't do anything if still measuring
    if (_is_measuring)
      return false;

    // get new measurement
    else{
      for (unsigned int i=0; i<_num_axes; i++)
	_offset_measurement[i] = 0;
      _treshold_moving = treshold_moving;
      _is_measuring = true;
      return true;
    }
  }

  
  bool nAxesControllerPos::finishedMeasuring() const
  {
    return !_is_measuring;
  }
  

  std::vector<double> nAxesControllerPos::getMeasurement()
  {
    return _offset_measurement;
  }

} // namespace
