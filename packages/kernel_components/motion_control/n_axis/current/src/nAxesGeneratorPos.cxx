#include "kernel_components/nAxesGeneratorPos.hpp"
#include <assert.h>


namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_Geometry;
  

  nAxesPosGenerator::nAxesPosGenerator(unsigned int num_axes,  std::string name)
    : nAxesPosGenerator_typedef(name),
      _num_axes(num_axes), 
      _position_desired(num_axes),
      _position_meas_local(num_axes),
      _position_local(num_axes),
      _velocity_local(num_axes),
      _properties_read(false),
      _new_values(false),
      _maximum_velocity("max_vel", "Maximum Velocity in Trajectory"),
      _maximum_acceleration("max_acc", "Maximum Acceleration in Trajectory")
  {}
  


  nAxesPosGenerator::~nAxesPosGenerator()
  {
    for( unsigned int i=0; i<_num_axes; i++)
      delete _motion_profile[i];
  }
  
  

  void nAxesPosGenerator::pull()
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
	for (unsigned int i=0; i<_num_axes; i++)
	  _position_desired[i] = _motion_profile[i]->Pos( _max_duration );

	_is_moving = false;
      }
    }

    // is not moving: can create new trajectory?
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

	_time_begin = HeartBeatGenerator::Instance()->ticksGet();
	_time_passed = 0;

	_new_values = false;
	_is_moving = true;
      }
    }

  }
  


  void nAxesPosGenerator::calculate()
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


  
  void nAxesPosGenerator::push()      
  {
    _position_DOI->Set(_position_local);
    _velocity_DOI->Set(_velocity_local);
  }



  bool nAxesPosGenerator::componentLoaded()
  {
    // get interface to Setpoint data types
    if ( !nAxesPosGenerator_typedef::SetPoint::dObj()->Get("Position", _position_DOI) ||
	 !nAxesPosGenerator_typedef::SetPoint::dObj()->Get("Velocity", _velocity_DOI) ){
      cerr << "nAxesPosGenerator::componentLoaded() DataObjectInterface not found" << endl;
      return false;
    }
    return true;
  }



  bool nAxesPosGenerator::componentStartup()
  {
    // check if updateProperties has been called
    assert(_properties_read);

    // initialize
    _is_initialized = false;

    // get interface to Cammand / Model / Input data types
    if ( !nAxesPosGenerator_typedef::Input::dObj()->Get("Position", _position_meas_DOI) ){
      cerr << "nAxesPosGenerator::componentStartup() DataObjectInterface not found" << endl;
      return false;
    }

    _is_moving  = false;
    
    return true;
  }


  
  bool nAxesPosGenerator::updateProperties(const ORO_ControlKernel::PropertyBag& bag)
  {
    // properties have been read
    _properties_read = true;

    // get properties
    if ( !composeProperty(bag, _maximum_velocity) ||
	 !composeProperty(bag, _maximum_acceleration) ){
      cerr << "nAxesPosGenerator::updateProperties() failed" << endl;
      return false;
    }

    // check size of properties
    assert(_maximum_velocity.value().size() >= _num_axes);
    assert(_maximum_acceleration.value().size() >= _num_axes);

    // Instantiate Motion Profiles
    for( unsigned int i=0; i<_num_axes; i++)
      _motion_profile.push_back( new VelocityProfile_Trap( _maximum_velocity.value()[i], _maximum_acceleration.value()[i]) );

    return true;
  }
  

  
  bool nAxesPosGenerator::moveTo(std::vector<double> position, double time)
  {
    cerr << "(Generator)  moveTo" << endl;

    MutexLock locker(_my_lock);

    // set desired position and time
    _traject_position = position;
    _traject_duration = time;

    assert(_traject_position.size() == _num_axes);
    _new_values = true;
    return true;
  }
  
  


} // namespace

