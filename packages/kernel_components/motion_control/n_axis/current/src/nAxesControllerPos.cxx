#include "kernel_components/nAxesControllerPos.hpp"
#include <assert.h>

namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;


  nAxesControllerPos::nAxesControllerPos(unsigned int num_axes, 
					 std::string name)
    : nAxesControllerPos_typedef(name),
      _num_axes(num_axes), 
      _velocity_local(num_axes),
      _properties_read(false),
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
  }

  
  void nAxesControllerPos::push()      
  {
    _velocity_DOI->Set(_velocity_local);
  }


  bool nAxesControllerPos::componentLoaded()
  {
    // get interface to Output data types
    if ( !nAxesControllerPos_typedef::Output::dObj()->Get("Velocity", _velocity_DOI) ){
      cerr << "nAxesControllerPos::componentLoaded() DataObjectInterface not found" << endl;
      return false;
    }
    return true;
  }


  bool nAxesControllerPos::componentStartup()
  {
    // check if updateProperties has been called
    assert(_properties_read);

    // get interface to Input/Setpoint data types
    if ( !nAxesControllerPos_typedef::Input::dObj(   )->Get("Position", _position_meas_DOI) ||
	 !nAxesControllerPos_typedef::SetPoint::dObj()->Get("Position", _position_desi_DOI) ){
      cerr << "nAxesControllerPos::componentStartup() DataObjectInterface not found" << endl;
      return false;
    }
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

  

} // namespace
