#include "kernel_components/nAxesGeneratorCartesianPos.hpp"
#include <assert.h>
#include <geometry/frames_io.h>


namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_Geometry;
  

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
      _time_passed = HeartBeatGenerator::Instance()->secondsSince(_time_begin);
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
	_twist_begin_end = diff(_traject_begin, _traject_end);

	// Set motion profiles
	for (unsigned int i=0; i<6; i++){
	  _motion_profile[i]->SetProfileDuration( 0, _twist_begin_end(i), _traject_duration );
	  _max_duration = max( _max_duration, _motion_profile[i]->Duration() );
	}

	// Rescale trajectories to maximal duration
	for (unsigned int i=0; i<6; i++)
	  _motion_profile[i]->SetProfileDuration( 0, _twist_begin_end(i), _max_duration );

	_time_begin = HeartBeatGenerator::Instance()->ticksGet();
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
      Vector translation_now(_motion_profile[0]->Pos(_time_passed),_motion_profile[1]->Pos(_time_passed),_motion_profile[2]->Pos(_time_passed));
      Vector rotation_now   (_motion_profile[3]->Pos(_time_passed),_motion_profile[4]->Pos(_time_passed),_motion_profile[5]->Pos(_time_passed));
      _position_out_local = Frame( _traject_begin.M * Rot( _traject_begin.M.Inverse( rotation_now ) ), _traject_begin.p + translation_now);

      // velocity
      for(unsigned int i=0; i<6; i++)
	_velocity_out_local(i) = _motion_profile[i]->Vel( _time_passed );
    }

    // go to desired stop position
    else{
      _position_out_local = _position_desired;
      _velocity_out_local = _velocity_out_local * 0;
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
    if ( !nAxesGeneratorCartesianPos_typedef::SetPoint::dObj()->Get("Position", _position_out_DOI) ||
	 !nAxesGeneratorCartesianPos_typedef::SetPoint::dObj()->Get("Velocity", _velocity_out_DOI) ){
      cerr << "nAxesGeneratorCartesianPos::componentLoaded() DataObjectInterface not found" << endl;
      return false;
    }
    return true;
  }



  bool nAxesGeneratorCartesianPos::componentStartup()
  {
    // check if updateProperties has been called
    assert(_properties_read);

    // initialize
    _is_initialized = false;

    // get interface to Cammand / Model / Input data types
    if ( !nAxesGeneratorCartesianPos_typedef::Input::dObj()->Get("Position", _position_meas_DOI) ){
      cerr << "nAxesGeneratorCartesianPos::componentStartup() DataObjectInterface not found" << endl;
      return false;
    }

    _is_moving  = false;
    
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
  

  
  bool nAxesGeneratorCartesianPos::moveTo(const ORO_Geometry::Frame& frame, double time)
  {
    MutexLock locker(_my_lock);

    // set desired position and time
    _traject_end      = frame;
    _traject_duration = time;

    _new_values = true;
    return true;
  }
  
  


} // namespace

