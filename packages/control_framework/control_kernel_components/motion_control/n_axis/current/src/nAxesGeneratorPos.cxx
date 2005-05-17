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

#include "control_kernel/nAxesGeneratorPos.hpp"
#include <corelib/Logger.hpp>
#include <assert.h>


namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_Geometry;
  using namespace ORO_Execution;
  using namespace ORO_OS;
  

  nAxesGeneratorPos::nAxesGeneratorPos(unsigned int num_axes,  std::string name)
    : nAxesGeneratorPos_typedef(name),
      _num_axes(num_axes), 
      _position_desired(num_axes),
      _traject_position(num_axes),
      _position_meas_local(num_axes),
      _position_local(num_axes),
      _velocity_local(num_axes),
      _properties_read(false),
      _maximum_velocity("max_vel", "Maximum Velocity in Trajectory"),
      _maximum_acceleration("max_acc", "Maximum Acceleration in Trajectory")
  {}
  


  nAxesGeneratorPos::~nAxesGeneratorPos()
  {
    for( unsigned int i=0; i<_num_axes; i++)
      delete _motion_profile[i];
  }
  
  

  void nAxesGeneratorPos::pull()
  {
    // initialize
    if (!_is_initialized){
      _is_initialized = true;
      _position_meas_DOI->Get(_position_desired);
    }

    // end condition
    if (_is_moving){
      _time_passed = TimeService::Instance()->secondsSince(_time_begin);
      if ( _time_passed > _max_duration ){
	// set end position
	for (unsigned int i=0; i<_num_axes; i++)
	  _position_desired[i] = _motion_profile[i]->Pos( _max_duration );

	_is_moving = false;
      }
    }

    // get new setpoint
    if (!_is_moving){
      // try to lock
      ORO_OS::MutexTryLock locker(_my_lock);
      if (locker.isSuccessful() && _new_values){
	_max_duration = 0;
	// get current position/
	_position_meas_DOI->Get(_position_meas_local);
	for (unsigned int i=0; i<_num_axes; i++){
	  // Set motion profiles
	  _motion_profile[i]->SetProfileDuration( _position_meas_local[i], _traject_position[i], _traject_duration );
	  // Find lengthiest trajectory
	  _max_duration = max( _max_duration, _motion_profile[i]->Duration() );
	}
	// Rescale trajectories to maximal duration
	for(unsigned int i = 0; i < _num_axes; i++)
	  _motion_profile[i]->SetProfileDuration( _position_meas_local[i], _traject_position[i], _max_duration );

	_time_begin = TimeService::Instance()->getTicks();
	_time_passed = 0;

	_new_values = false;
	_is_moving = true;
      }
    }

  }
  


  void nAxesGeneratorPos::calculate()
  {
    // is moving: follow trajectory
    if (_is_moving){
      for(unsigned int i=0; i<_num_axes; i++){
	_position_local[i] = _motion_profile[i]->Pos( _time_passed );
	_velocity_local[i] = _motion_profile[i]->Vel( _time_passed );
      }
    }

    // go to desired stop position
    else{
      for(unsigned int i=0; i<_num_axes; i++){
	_position_local[i] = _position_desired[i];
	_velocity_local[i] = 0;
      }
    }
  }


  
  void nAxesGeneratorPos::push()      
  {
    _position_DOI->Set(_position_local);
    _velocity_DOI->Set(_velocity_local);
  }



  bool nAxesGeneratorPos::componentLoaded()
  {
    // get interface to Setpoint data types
    if ( !SetPoint->dObj()->Get("Position_joint", _position_DOI) ||
	 !SetPoint->dObj()->Get("Velocity_joint", _velocity_DOI) ){
      Logger::log() << Logger::Error << "nAxesGeneratorPos::componentLoaded() DataObjectInterface not found" << Logger::endl;
      return false;
    }

    // set empty values
    vector<double> _temp_vector(_num_axes);
    _position_DOI->Set(_temp_vector);
    _velocity_DOI->Set(_temp_vector);

    return true;
  }



  bool nAxesGeneratorPos::componentStartup()
  {
    // check if updateProperties has been called
    if (!_properties_read){
      Logger::log() << Logger::Error << "nAxesGeneratorPos::componentStartup() Properties have not been read" << Logger::endl;
      return false;
    }

    // initialize
    _is_initialized = false;
    _new_values = false;
    _is_moving = false;

    // get interface to Cammand / Model / Input data types
    if ( !Input->dObj()->Get("Position_joint", _position_meas_DOI) ){
      Logger::log() << Logger::Error << "nAxesGeneratorPos::componentStartup() DataObjectInterface not found" << Logger::endl;
      return false;
    }

    _is_moving  = false;
    
    return true;
  }


  
  bool nAxesGeneratorPos::updateProperties(const ORO_ControlKernel::PropertyBag& bag)
  {
    // properties have been read
    _properties_read = true;

    // get properties
    if ( !composeProperty(bag, _maximum_velocity) ||
	 !composeProperty(bag, _maximum_acceleration) ){
      Logger::log() << Logger::Error << "nAxesGeneratorPos::updateProperties() failed" << Logger::endl;
      return false;
    }

    // check size of properties
    assert(_maximum_velocity.value().size() == _num_axes);
    assert(_maximum_acceleration.value().size() == _num_axes);

    // Instantiate Motion Profiles
    for( unsigned int i=0; i<_num_axes; i++)
      _motion_profile.push_back( new VelocityProfile_Trap( _maximum_velocity.value()[i], _maximum_acceleration.value()[i]) );

    return true;
  }
  
  void nAxesGeneratorPos::exportProperties(ORO_CoreLib::PropertyBag& bag)
  {
    bag.add(&_maximum_velocity);
    bag.add(&_maximum_acceleration);
  }
  

  CommandFactoryInterface* nAxesGeneratorPos::createCommandFactory()
  {
    TemplateCommandFactory<nAxesGeneratorPos>* my_commandFactory = newCommandFactory( this );
    my_commandFactory->add( "moveTo", command( &nAxesGeneratorPos::moveTo,
					       &nAxesGeneratorPos::moveFinished,
					       "Set the position setpoint",
					       "setpoint", "joint setpoint for all axes",
					       "time", "minimum time to complete trajectory") );
    return my_commandFactory;
  }

  MethodFactoryInterface* nAxesGeneratorPos::createMethodFactory()
  {
    TemplateMethodFactory<nAxesGeneratorPos>* my_methodFactory = newMethodFactory( this );
    my_methodFactory->add( "reset", method( &nAxesGeneratorPos::reset, "Reset generator" ));

    return my_methodFactory;
  }


  bool nAxesGeneratorPos::moveTo(const std::vector<double>& position, double time)
  {
    MutexLock locker(_my_lock);

    // no new values available
    if (!_new_values){
      _traject_position = position;
      _traject_duration = time;
      _new_values = true;

      assert(_traject_position.size() == _num_axes);
      return true;
    }
    // new values already set
    else
      return false;
  }

  bool nAxesGeneratorPos::moveFinished() const
  {
    MutexLock locker(_my_lock);
    return (!_is_moving && !_new_values);
  }


  void nAxesGeneratorPos::reset()
  {
    _is_initialized = false;
  }


} // namespace

