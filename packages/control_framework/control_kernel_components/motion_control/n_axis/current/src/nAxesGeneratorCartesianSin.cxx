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
  using namespace std;  

  nAxesGeneratorCartesianSin::nAxesGeneratorCartesianSin(std::string name)
    : nAxesGeneratorCartesianSin_typedef(name),
      _properties_read(false),
      _new_values(false),
      _maximum_velocity("max_vel", "Maximum Velocity in Trajectory"),
      _maximum_acceleration("max_acc", "Maximum Acceleration in Trajectory"),
      _amplitude(3),
      _pulsation(3),
      _phase(3)

    
  {
    for(unsigned int i = 0 ; i < 3 ; i++) {
      _amplitude[i] = 0;
      _pulsation[i] = 0;
      _phase[i] = 0;
    }
  }

}
  


  nAxesGeneratorCartesianSin::~nAxesGeneratorCartesianSin()
  {
    for( unsigned int i=0; i<6; i++)
      delete _motion_profile[i];
  }
  
  

  void nAxesGeneratorCartesianSin::pull()
  {
    // initialize
    if (!_is_initialized){
      _is_initialized = true;
      _position_meas_DOI->Get(_position_desired);
      _start_frame = _position_desired;
    }

    // is moving: do we have to stop?
    if (_is_moving){
      _time_passed = TimeService::Instance()->secondsSince(_time_begin);
      if ( _time_passed > _max_duration ){
	// set end position
	_position_desired = _start_frame;
	_is_moving = false;
      }
    }
    /*
    // is not moving: can create new trajectory?
    if (!_is_moving){
      // try to lock
      ORO_OS::MutexTryLock locker(_my_lock);
      if (locker.isSuccessful() && _new_values){
	_max_duration = 0;

	// get current position
	_position_meas_DOI->Get(_traject_begin);
	_velocity_begin_end = diff(_traject_begin, _traject_end);

	// Set motion profiles
	for (unsigned int i=0; i<6; i++){
	  _motion_profile[i]->SetProfileDuration( 0, _velocity_begin_end(i), _traject_duration );
	  _max_duration = max( _max_duration, _motion_profile[i]->Duration() );
	}

	// Rescale trajectories to maximal duration
	for (unsigned int i=0; i<6; i++)
	  _motion_profile[i]->SetProfileDuration( 0, _velocity_begin_end(i), _max_duration );

	_time_begin = TimeService::Instance()->getTicks();
	_time_passed = 0;

	_new_values = false;
	_is_moving = true;
      }
    
    }
    */
  }
  


  void nAxesGeneratorCartesianSin::calculate()
  {
    // is moving: follow trajectory
    if (_is_moving){
      // position
      _position_out_local = _start_frame*Vector(_amplitude[0]*sin(_pulsation[0]*_time_passed+_phase[0]),_amplitude[1]*sin(_pulsation[1]*_time_passed+_phase[1]),_amplitude[2]*sin(_pulsation[2]*_time_passed+_phase[2]));

      // velocity
      for(unsigned int i=0; i<3; i++)
	_velocity_out_local(i) = _amplitude[i]*_pulsation[i]*cos(_pulsation[i]*_time_passed+_phase[i]);
      //      _velocity_out_local.RefPoint( _position_out_local.p * -1 );
    }

    // go to desired stop position
    else{
      _position_out_local = _position_desired;
      SetToZero(_velocity_out_local);
    }
  }


  
  void nAxesGeneratorCartesianSin::push()      
  {
    _position_out_DOI->Set(_position_out_local);
    _velocity_out_DOI->Set(_velocity_out_local);
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

    // initialize
    _is_initialized = false;
    _new_values = false;
    _is_moving = false;

    // get interface to Cammand / Model / Input data types
    if ( !Input->dObj()->Get("Frame", _position_meas_DOI) ){
      cerr << "nAxesGeneratorCartesianSin::componentStartup() DataObjectInterface not found" << endl;
      return false;
    }

    return true;
  }


  
  bool nAxesGeneratorCartesianSin::updateProperties(const ORO_ControlKernel::PropertyBag& bag)
  {
    // properties have been read
    _properties_read = true;

    // get properties
    if ( !composeProperty(bag, _maximum_velocity) ||
	 !composeProperty(bag, _maximum_acceleration) ){
      cerr << "nAxesGeneratorCartesianSin::updateProperties() failed" << endl;
      return false;
    }

    // check size of properties
    assert(_maximum_velocity.value().size() == 6);
    assert(_maximum_acceleration.value().size() == 6);

    // Instantiate Motion Profiles
    for( unsigned int i=0; i<6; i++)
      _motion_profile.push_back( new VelocityProfile_Trap( _maximum_velocity.value()[i], _maximum_acceleration.value()[i]) );

    return true;
  }
  

  void nAxesGeneratorCartesianSin::exportProperties(ORO_CoreLib::PropertyBag& bag)
  {
    bag.add(&_maximum_velocity);
    bag.add(&_maximum_acceleration);
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
    
    _traject_duration = time;
    _is_moving = true;
    return true;
  }
  
  bool nAxesGeneratorCartesianSin::moveFinished() const
  {
    return (!_is_moving);
  }


  void nAxesGeneratorCartesianSin::reset()
  {
    _is_initialized = false;
    _new_values = false;
    _is_moving = false;

    for(unsigned int i = 0 ; i < 3 ; i++) {
      _amplitude[i] = 0;
      _pulsation[i] = 0;
      _phase[i] = 0;
    }
  }

  void nAxesGeneratorCartesianSin::setAmplitude(vector<double> amplitude)
  {
    assert(amplitude.size()==3);    
    for(unsigned int i = 0 ; i < 3 ; i++) 
      _amplitude[i] = amplitude[i];
  }

  void nAxesGeneratorCartesianSin::setPulsation(vector<double> pulsation)
  {
    assert(pulsation.size()==3);    
    for(unsigned int i = 0 ; i < 3 ; i++) 
      _pulsation[i] = pulsation[i];
  }

  void nAxesGeneratorCartesianSin::setPhase(vector<double> phase)
  {
    assert(phase.size()==3);    
    for(unsigned int i = 0 ; i < 3 ; i++) 
      _phase[i] = phase[i];
  }

  

} // namespace

