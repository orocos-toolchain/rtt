#include "kernel_components/nAxesControllerVel.hpp"
#include <assert.h>

namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;


  nAxesControllerVel::nAxesControllerVel(unsigned int num_axes, 
					 std::string name)
    : nAxesControllerVel_typedef(name),
      _num_axes(num_axes), 
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
    assert(_properties_read);

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

  

} // namespace
