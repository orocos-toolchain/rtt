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

#include "control_kernel/nAxesGeneratorCartesianPos.hpp"
#include <assert.h>


namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_Geometry;
  using namespace ORO_Execution;
  using namespace ORO_OS;
  

  nAxesGeneratorCartesianPos::nAxesGeneratorCartesianPos(std::string name)
    : nAxesGeneratorCartesianPos_typedef(name),
      _properties_read(false),
      _new_values(false),
      _maximum_velocity("max_vel", "Maximum Velocity in Trajectory"),
      _maximum_acceleration("max_acc", "Maximum Acceleration in Trajectory")
  {}
  


  nAxesGeneratorCartesianPos::~nAxesGeneratorCartesianPos()
  {
    for( unsigned int i=0; i<6; i++)
      delete _motion_profile[i];
  }
  
  

  void nAxesGeneratorCartesianPos::pull()
  {
    // initialize
    if (!_is_initialized){
      _is_initialized = true;
      _position_meas_DOI->Get(_position_desired);
    }

    // is moving: do we have to stop?
    if (_is_moving){
      _time_passed = TimeService::Instance()->secondsSince(_time_begin);
      if ( _time_passed > _max_duration ){
	// set end position
	_position_desired = _traject_end;
	_is_moving = false;
      }
    }

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

  }
  


  void nAxesGeneratorCartesianPos::calculate()
  {
    // is moving: follow trajectory
    if (_is_moving){
      // position
      _velocity_delta = Twist(Vector(_motion_profile[0]->Pos(_time_passed),_motion_profile[1]->Pos(_time_passed),_motion_profile[2]->Pos(_time_passed)),
			      Vector(_motion_profile[3]->Pos(_time_passed),_motion_profile[4]->Pos(_time_passed),_motion_profile[5]->Pos(_time_passed)) );
      _position_out_local = Frame( _traject_begin.M * Rot( _traject_begin.M.Inverse( _velocity_delta.rot ) ), _traject_begin.p + _velocity_delta.vel);

      // velocity
      for(unsigned int i=0; i<6; i++)
	_velocity_out_local(i) = _motion_profile[i]->Vel( _time_passed );
      _velocity_out_local.RefPoint( _position_out_local.p * -1 );
    }

    // go to desired stop position
    else{
      _position_out_local = _position_desired;
      SetToZero(_velocity_out_local);
    }
  }


  
  void nAxesGeneratorCartesianPos::push()      
  {
    _position_out_DOI->Set(_position_out_local);
    _velocity_out_DOI->Set(_velocity_out_local);
  }



  bool nAxesGeneratorCartesianPos::componentLoaded()
  {
    // get interface to Setpoint data types
    if ( !SetPoint->dObj()->Get("Frame", _position_out_DOI) ||
	 !SetPoint->dObj()->Get("Twist", _velocity_out_DOI) ){
      cerr << "nAxesGeneratorCartesianPos::componentLoaded() DataObjectInterface not found" << endl;
      return false;
    }

    // set empty values
    Frame _temp_frame;
    Twist _temp_twist;
    _position_out_DOI->Set(_temp_frame);
    _velocity_out_DOI->Set(_temp_twist);

    return true;
  }



  bool nAxesGeneratorCartesianPos::componentStartup()
  {
    // check if updateProperties has been called
    if (!_properties_read){
      cerr << "nAxesGeneratorCartesianPos::componentStartup() Properties have not been read." << endl;
      return false;
    }

    // initialize
    _is_initialized = false;
    _new_values = false;
    _is_moving = false;

    // get interface to Cammand / Model / Input data types
    if ( !Input->dObj()->Get("Frame", _position_meas_DOI) ){
      cerr << "nAxesGeneratorCartesianPos::componentStartup() DataObjectInterface not found" << endl;
      return false;
    }

    return true;
  }


  
  bool nAxesGeneratorCartesianPos::updateProperties(const ORO_ControlKernel::PropertyBag& bag)
  {
    // properties have been read
    _properties_read = true;

    // get properties
    if ( !composeProperty(bag, _maximum_velocity) ||
	 !composeProperty(bag, _maximum_acceleration) ){
      cerr << "nAxesGeneratorCartesianPos::updateProperties() failed" << endl;
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
  

  void nAxesGeneratorCartesianPos::exportProperties(ORO_CoreLib::PropertyBag& bag)
  {
    bag.add(&_maximum_velocity);
    bag.add(&_maximum_acceleration);
  }


  CommandFactoryInterface* nAxesGeneratorCartesianPos::createCommandFactory()
  {
    TemplateCommandFactory<nAxesGeneratorCartesianPos>* my_commandFactory = newCommandFactory( this );
    my_commandFactory->add( "moveTo", command( &nAxesGeneratorCartesianPos::moveTo,
					       &nAxesGeneratorCartesianPos::moveFinished,
					       "Set the position setpoint",
					       "setpoint", "position setpoint for end effector",
					       "time", "minimum time to execute trajectory") );
    return my_commandFactory;
  }


  
  MethodFactoryInterface* nAxesGeneratorCartesianPos::createMethodFactory()
  {
    TemplateMethodFactory<nAxesGeneratorCartesianPos>* my_methodFactory = newMethodFactory( this );
    my_methodFactory->add( "reset", method( &nAxesGeneratorCartesianPos::reset, "Reset generator" ));

    return my_methodFactory;
  }



  bool nAxesGeneratorCartesianPos::moveTo(ORO_Geometry::Frame frame, double time)
  {
    MutexLock locker(_my_lock);

    // no new values available
    if (!_new_values){
      _traject_end = frame;
      _traject_duration = time;
      _new_values = true;

      return true;
    }
    // new values already set
    else{
      cerr << "(nAxesGeneratorCartesianPos)  cannot set new moveto setpoint: already have setpoint" << endl;
      return false;
    }
  }


  bool nAxesGeneratorCartesianPos::moveFinished() const
  {
    MutexLock locker(_my_lock);
    return (!_is_moving && !_new_values);
  }


  void nAxesGeneratorCartesianPos::reset()
  {
    _is_initialized = false;
    _new_values = false;
    _is_moving = false;
  }
  

} // namespace

