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


#include "control_kernel/nAxesControllerPos.hpp"
#include <assert.h>

namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_Execution;


  nAxesControllerPos::nAxesControllerPos(unsigned int num_axes, 
					 std::string name)
    : nAxesControllerPos_typedef(name),
      _num_axes(num_axes), 
      _position_meas(num_axes),
      _position_desi(num_axes),
      _velocity_out(num_axes),
      _offset_measurement(num_axes),
      _properties_read(false),
      _controller_gain("K", "Proportional Gain")
  {}


  nAxesControllerPos::~nAxesControllerPos(){};
  

  void nAxesControllerPos::pull()
  {
    // copy Input and Setpoint to local values
    _position_meas_DOI->Get(_position_meas);
    _position_desi_DOI->Get(_position_desi);
  }


  void nAxesControllerPos::calculate()
  {
    // position feedback
    for(unsigned int i=0; i<_num_axes; i++)
      _velocity_out[i] = _controller_gain.value()[i] * (_position_desi[i] - _position_meas[i]);


    // measure offsets
    if (_is_measuring && TimeService::Instance()->secondsSince(_time_begin) > _time_sleep){
      for (unsigned int i=0; i<_num_axes; i++)
	_offset_measurement[i] += _velocity_out[i] / _num_samples;
      _num_samples_taken++;
      if (_num_samples_taken == _num_samples)  _is_measuring = false;
    }
  }

  
  void nAxesControllerPos::push()      
  {
    _velocity_DOI->Set(_velocity_out);
  }


  bool nAxesControllerPos::componentLoaded()
  {
    // get interface to Output data types
    if ( !Output::dObj()->Get("Velocity", _velocity_DOI) ){
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
    if ( !Input::dObj(   )->Get("Position", _position_meas_DOI) ||
	 !SetPoint::dObj()->Get("Position", _position_desi_DOI) ){
      cerr << "nAxesControllerPos::componentStartup() DataObjectInterface not found" << endl;
      return false;
    }

    // initialize
    _is_measuring = false;
    
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
    assert(_controller_gain.value().size() == _num_axes);

    return true;
  }


  void nAxesControllerPos::exportProperties(ORO_CoreLib::PropertyBag& bag)
  {
    bag.add(&_controller_gain);
  }


  CommandFactoryInterface* nAxesControllerPos::createCommandFactory()
  {
    TemplateCommandFactory<nAxesControllerPos>* my_commandFactory = newCommandFactory( this );
    my_commandFactory->add( "measureOffset", command( &nAxesControllerPos::startMeasuringOffsets,
						      &nAxesControllerPos::finishedMeasuringOffsets,
						      "calculate the velocity offset on the axes",
						      "time_sleep", "time to wait before starting measurement",
						      "num_samples", "number of samples to take"));
    return my_commandFactory;
  }


  MethodFactoryInterface* nAxesControllerPos::createMethodFactory()
  {
    TemplateMethodFactory<nAxesControllerPos>* my_methodFactory = newMethodFactory( this );
    my_methodFactory->add( "getOffset", method( &nAxesControllerPos::getMeasurementOffsets, "Get offset measurements"));

    return my_methodFactory;
  }



  bool nAxesControllerPos::startMeasuringOffsets(double time_sleep, int num_samples)
  {
    // don't do anything if still measuring
    if (_is_measuring)
      return false;

    // get new measurement
    else{
      for (unsigned int i=0; i<_num_axes; i++){
	_offset_measurement[i] = 0;
      }
      _time_sleep        = max(1.0, time_sleep);  // min 1 sec
      _time_begin        = TimeService::Instance()->getTicks();
      _num_samples       = max(1,num_samples);    // min 1 sample
      _num_samples_taken = 0;
      _is_measuring      = true;
      return true;
    }
  }

  
  bool nAxesControllerPos::finishedMeasuringOffsets() const
  {
    return !_is_measuring;
  }
  

  const std::vector<double>& nAxesControllerPos::getMeasurementOffsets()
  {
    return _offset_measurement;
  }

} // namespace
