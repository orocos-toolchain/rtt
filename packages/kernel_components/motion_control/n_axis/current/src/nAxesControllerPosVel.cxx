#include "kernel_components/nAxesControllerPosVel.hpp"
#include <assert.h>

namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;

  nAxesControllerPosVel::nAxesControllerPosVel(unsigned int num_axes, 
					       std::string name)
    : nAxesControllerPosVel_typedef(name),
      _num_axes(num_axes), 
      _position_meas_local(num_axes),
      _position_desi_local(num_axes),
      _velocity_desi_local(num_axes),
      _velocity_local(num_axes),
      _properties_read(false),
      _controller_gain("K", "Proportional Gain")
  {}


  nAxesControllerPosVel::~nAxesControllerPosVel(){};
  

  void nAxesControllerPosVel::pull()
  {
    // copy Input and Setpoint to local values
    _position_meas_DOI->Get(_position_meas_local);
    _position_desi_DOI->Get(_position_desi_local);
    _velocity_desi_DOI->Get(_velocity_desi_local);
  }



  void nAxesControllerPosVel::calculate()
  {
    for(unsigned int i=0; i<_num_axes; i++)
      _velocity_local[i] = (_controller_gain.value()[i] * (_position_desi_local[i] - _position_meas_local[i])) + _velocity_desi_local[i];
  }

  
  void nAxesControllerPosVel::push()      
  {
    _velocity_DOI->Set(_velocity_local);
  }


  bool nAxesControllerPosVel::componentLoaded()
  {
    // get interface to Output data types
    if ( !nAxesControllerPosVel_typedef::Output::dObj()->Get("Velocity", _velocity_DOI) ){
      cerr << "nAxesControllerPosVel::componentLoaded() DataObjectInterface not found" << endl;
      return false;
    }
    return true;
  }


  bool nAxesControllerPosVel::componentStartup()
  {
    // check if updateProperties has been called
    assert(_properties_read);

    // get interface to Input/Setpoint data types
    if ( !nAxesControllerPosVel_typedef::Input::dObj(   )->Get("Position", _position_meas_DOI) ||
	 !nAxesControllerPosVel_typedef::SetPoint::dObj()->Get("Position", _position_desi_DOI) ||
	 !nAxesControllerPosVel_typedef::SetPoint::dObj()->Get("Velocity", _velocity_desi_DOI) ){
      cerr << "nAxesControllerPosVel::componentStartup() DataObjectInterface not found" << endl;
      return false;
    }
    return true;
  }
  

  bool nAxesControllerPosVel::updateProperties(const PropertyBag& bag)
  {
    // properties have been read
    _properties_read = true;

    // get properties
    if (!composeProperty(bag, _controller_gain) ){
      cerr << "nAxesControllerPosVel::updateProperties() failed" << endl;
      return false;
    }

    // check size of properties
    assert(_controller_gain.value().size() >= _num_axes);

    return true;
  }

  

} // namespace
