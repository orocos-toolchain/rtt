#include "kernel_components/nAxesControllerCartesianPos.hpp"
#include <assert.h>
#include <geometry/frames_io.h>


namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;

  nAxesControllerCartesianPos::nAxesControllerCartesianPos(std::string name)
    : nAxesControllerCartesianPos_typedef(name),
      _properties_read(false),
      _controller_gain("K", "Proportional Gain")
  {}


  nAxesControllerCartesianPos::~nAxesControllerCartesianPos(){};
  

  void nAxesControllerCartesianPos::pull()
  {
    // copy Input and Setpoint to local values
    _position_meas_DOI->Get(_position_meas_local);
    _position_desi_DOI->Get(_position_desi_local);
  }



  void nAxesControllerCartesianPos::calculate()
  {
    // feedback on position
    _velocity_out_local = diff(_position_meas_local, _position_desi_local);
    
    for(unsigned int i=0; i<6; i++)
      _velocity_out_local(i) *= _controller_gain.value()[i];
  }
  
  
  void nAxesControllerCartesianPos::push()      
  {
    _velocity_out_DOI->Set(_velocity_out_local);
  }


  bool nAxesControllerCartesianPos::componentLoaded()
  {
    // get interface to Output data types
    if ( !nAxesControllerCartesianPos_typedef::Output::dObj()->Get("Velocity", _velocity_out_DOI) ){
      cerr << "nAxesControllerCartesianPos::componentLoaded() DataObjectInterface not found" << endl;
      return false;
    }
    return true;
  }


  bool nAxesControllerCartesianPos::componentStartup()
  {
    // check if updateProperties has been called
    assert(_properties_read);

    // get interface to Input/Setpoint data types
    if ( !nAxesControllerCartesianPos_typedef::Input::dObj(   )->Get("Position", _position_meas_DOI) ||
	 !nAxesControllerCartesianPos_typedef::SetPoint::dObj()->Get("Position", _position_desi_DOI) ){
      cerr << "nAxesControllerCartesianPos::componentStartup() DataObjectInterface not found" << endl;
      return false;
    }
    return true;
  }
  

  bool nAxesControllerCartesianPos::updateProperties(const PropertyBag& bag)
  {
    // properties have been read
    _properties_read = true;

    // get properties
    if (!composeProperty(bag, _controller_gain) ){
      cerr << "nAxesVelController::updateProperties() failed" << endl;
      return false;
    }

    // check size of properties
    assert(_controller_gain.value().size() >= 6);

    return true;
  }

  

} // namespace
