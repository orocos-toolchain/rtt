#include "kernel_components/nAxesSensorPos.hpp"
#include <assert.h>

namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_DeviceInterface;


  nAxesPosSensor::nAxesPosSensor(unsigned int num_axes, 
				 std::vector<AxisInterface*> axes,
				 std::string name)
    : nAxesPosSensor_typedef(name),
      _num_axes(num_axes), 
      _axes(axes),
      _position_local(num_axes),
      _position_sensors(num_axes)
  {
    assert(_axes.size() == num_axes);

    // get position sensors from axes
    for (unsigned int i=0; i<_num_axes; i++){
      _position_sensors[i] = _axes[i]->getSensor("Position");
      assert( _position_sensors[i] != NULL );
    }
  }


  nAxesPosSensor::~nAxesPosSensor(){};
  

  void nAxesPosSensor::pull()
  {
    // copy values from position sensors to local variable
    for (unsigned int i=0; i<_num_axes; i++)
      _position_local[i] = _position_sensors[i]->readSensor();
  }



  void nAxesPosSensor::calculate()
  {}


  
  void nAxesPosSensor::push()      
  {
    _position_DOI->Set(_position_local);
  }



  bool nAxesPosSensor::componentLoaded()
  {
    // get interface to Input data types
    if (!Sensor< Writes<nAxesPosSensorInput_pos> >::Input::dObj()->Get("Position", _position_DOI)){
      cerr << "nAxesPosSensor::componentLoaded() DataObjectInterface not found" << endl;
      return false;
    }

    return true;
  }


  bool nAxesPosSensor::componentStartup()
  {
    return true;
  }
  
  

} // end namespace ORO_nAxesControlKernel
