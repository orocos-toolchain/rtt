// $Id: nAxisGeneratorCartesianSin.hpp,v 1.1.1.1 2003/12/02 20:32:06 kgadeyne Exp $
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

#include "control_kernel/nAxesGeneratorCartesianSin.hpp"
#include <assert.h>


namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_Geometry;
  using namespace ORO_Execution;
  using namespace ORO_OS;

  nAxesGeneratorCartesianSin::nAxesGeneratorCartesianSin(std::string name)
    : nAxesGeneratorCartesianSin_typedef(name),
      _pulsation_profile(3),
      _max_alpha("max_alpha", "Maximum rotation acceleration in Trajectory"),
      _max_alphadot("max_alphadot", "Maximum rotation jerk in Trajectory"),
      _properties_read(false),
      _amplitude(3),
      _pulsation(3),
      _phase(3),
      _pulsation_local(3)

    
  {
    for(unsigned int i = 0 ; i < 3 ; i++) {
      _amplitude[i] = 0;
      _pulsation[i] = 0;
      _phase[i] = 0;
    }
  }


  


  nAxesGeneratorCartesianSin::~nAxesGeneratorCartesianSin()
  {
    for( unsigned int i=0; i<3; i++)
      delete _pulsation_profile[i];
  }
  
  

  void nAxesGeneratorCartesianSin::pull()
  {
    // initialize
    if (!_is_initialized){
      _is_initialized = true;
      _position_meas_DOI->Get(_start_frame);
      for(unsigned int i = 0 ; i < 3 ; i++) 
	_start_frame.p(i)=_start_frame.p(i)-_amplitude[i]*sin(_phase[i]);
      _position_out_local.M=_start_frame.M;
    }
    
    _time_passed = TimeService::Instance()->secondsSince(_time_begin);
    _time_omega_passed = TimeService::Instance()->secondsSince(_time_omega_begin);
  }
  
  
  void nAxesGeneratorCartesianSin::calculate()
  {

    // is moving: follow trajectory
    if (_is_moving || _time_passed <= _max_duration || _max_duration==0){
      for(unsigned int i=0; i<3; i++) {
	_pulsation_local[i]=_pulsation_profile[i]->Pos(_time_omega_passed);
	// position
	_position_out_local.p(i) = _start_frame.p(i)+_amplitude[i]*sin(_pulsation_local[i]*_time_passed+_phase[i]);
	// velocity
	_velocity_out_local(i) = _amplitude[i]*_pulsation_local[i]*cos(_pulsation_local[i]*_time_passed+_phase[i]);
      }
    }
    // go to desired stop position
    else {
      std::vector<double> zero(3);
      for(unsigned int i=0; i<3; i++)
	zero[i] = 0.0;
      setPulsation(zero);
    }
  }


  
  void nAxesGeneratorCartesianSin::push()      
  {
    _position_out_DOI->Set(_position_out_local);
    _velocity_out_DOI->Set(_velocity_out_local.RefPoint(-(_position_out_local.p)));
  }



  bool nAxesGeneratorCartesianSin::componentLoaded()
  {
    // get interface to Setpoint data types
    if ( !SetPoint->dObj()->Get("Frame", _position_out_DOI) ||
	 !SetPoint->dObj()->Get("Twist", _velocity_out_DOI) ){
      cerr << "nAxesGeneratorCartesianSin::componentLoaded() DataObjectInterface not found" << endl;
      return false;
    }

    // set empty values
    Frame _temp_frame;
    Twist _temp_twist;
    _position_out_DOI->Set(_temp_frame);
    _velocity_out_DOI->Set(_temp_twist);

    return true;
  }



  bool nAxesGeneratorCartesianSin::componentStartup()
  {
    // check if updateProperties has been called
    if (!_properties_read){
      cerr << "nAxesGeneratorCartesianSin::componentStartup() Properties have not been read." << endl;
      return false;
    }

    // get interface to Cammand / Model / Input data types
    if ( !Input->dObj()->Get("Frame", _position_meas_DOI) ){
      cerr << "nAxesGeneratorCartesianPos::componentStartup() DataObjectInterface not found" << endl;
      return false;
    }


    // initialize
    _is_initialized = false;
    _is_moving = false;

    return true;
  }


  
  bool nAxesGeneratorCartesianSin::updateProperties(const ORO_ControlKernel::PropertyBag& bag)
  {
    // properties have been read
    _properties_read = true;

    // get properties
    if ( !composeProperty(bag, _max_alpha) ||
	 !composeProperty(bag, _max_alphadot) ){
      cerr << "nAxesGeneratorCartesianSin::updateProperties() failed" << endl;
      return false;
    }

    // check size of properties
    assert(_max_alpha.value().size() == 3);
    assert(_max_alphadot.value().size() == 3);

    // Instantiate Motion Profiles
    for( unsigned int i=0; i<3; i++)
      _pulsation_profile[i] = new VelocityProfile_Trap( _max_alpha.value()[i], _max_alphadot.value()[i]);

    return true;
  }
  

  void nAxesGeneratorCartesianSin::exportProperties(ORO_CoreLib::PropertyBag& bag)
  {
    bag.add(&_max_alpha);
    bag.add(&_max_alphadot);
  }


  CommandFactoryInterface* nAxesGeneratorCartesianSin::createCommandFactory()
  {
    TemplateCommandFactory<nAxesGeneratorCartesianSin>* my_commandFactory = newCommandFactory( this );
    my_commandFactory->add( "move", command( &nAxesGeneratorCartesianSin::move,
					       &nAxesGeneratorCartesianSin::moveFinished,
					       "Start the movement",
					       "time", "minimum time to execute trajectory") );
    return my_commandFactory;
  }


  
  MethodFactoryInterface* nAxesGeneratorCartesianSin::createMethodFactory()
  {
    TemplateMethodFactory<nAxesGeneratorCartesianSin>* my_methodFactory = newMethodFactory( this );
    my_methodFactory->add( "reset", method( &nAxesGeneratorCartesianSin::reset, "Reset generator" ));
    my_methodFactory->add( "setAmplitude", method( &nAxesGeneratorCartesianSin::setAmplitude,
						   "Set Amplitude for XYZ",
						   "amplitude","vector of amplitudes to set" ));
    my_methodFactory->add( "setPulsation", method( &nAxesGeneratorCartesianSin::setPulsation,
						   "Set Pulsation for XYZ",
						   "pulsation","vector of pulsations to set" ));
    my_methodFactory->add( "setPhase", method( &nAxesGeneratorCartesianSin::setPhase,
						   "Set Phase for XYZ",
						   "phase","vector of phases to set" ));

    return my_methodFactory;
  }



  bool nAxesGeneratorCartesianSin::move(double time)
  {
    _time_begin = TimeService::Instance()->getTicks();    
    _time_passed = 0;
    _max_duration = time;
    _is_moving = true;
    return true;
  }
  
  bool nAxesGeneratorCartesianSin::moveFinished() const
  {
    return (!_is_moving || _max_duration==0 || _max_duration <= _time_passed);
  }


  void nAxesGeneratorCartesianSin::reset()
  {
    _is_initialized = false;
    _is_moving = false;

    for(unsigned int i = 0 ; i < 3 ; i++) {
      _amplitude[i] = 0;
      _pulsation[i] = 0;
      _phase[i] = 0;
    }
  }

  void nAxesGeneratorCartesianSin::setAmplitude(const std::vector<double>& amplitude)
  {
    assert(amplitude.size()==3);    
    for(unsigned int i = 0 ; i < 3 ; i++)
      _amplitude[i] = amplitude[i];
  }

  void nAxesGeneratorCartesianSin::setPulsation(const std::vector<double>& pulsation)
  {
    _time_omega_begin = TimeService::Instance()->getTicks();    
    _time_omega_passed = 0;
    assert(pulsation.size()==3);    
    for(unsigned int i = 0 ; i < 3 ; i++) { 
      _pulsation_profile[i]->SetProfile(_pulsation[i], pulsation[i]);
      _pulsation[i] = pulsation[i];
    }
  }

  void nAxesGeneratorCartesianSin::setPhase(const std::vector<double>& phase)
  {
    assert(phase.size()==3);    
    for(unsigned int i = 0 ; i < 3 ; i++) 
      _phase[i] = phase[i];
  }

  

} // namespace

