#include "kernel_components/nAxesSensorPos.hpp"
#include <assert.h>

namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_DeviceInterface;


  nAxesSensorPos::nAxesSensorPos(unsigned int num_axes, 
				 std::vector<AxisInterface*> axes,
				 std::string name)
    : nAxesSensorPos_typedef(name),
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


  nAxesSensorPos::~nAxesSensorPos(){};
  

  void nAxesSensorPos::pull()
  {
    // copy values from position sensors to local variable
    for (unsigned int i=0; i<_num_axes; i++)
      _position_local[i] = _position_sensors[i]->readSensor();
  }



  void nAxesSensorPos::calculate()
  {}


  
  void nAxesSensorPos::push()      
  {
    _position_DOI->Set(_position_local);
  }



  bool nAxesSensorPos::componentLoaded()
  {
    // get interface to Input data types
    if (!Sensor< Writes<nAxesSensorPosInput_pos> >::Input::dObj()->Get("Position", _position_DOI)){
      cerr << "nAxesSensorPos::componentLoaded() DataObjectInterface not found" << endl;
      return false;
    }

    return true;
  }


  bool nAxesSensorPos::componentStartup()
  {
    return true;
  }
  
  

} // end namespace ORO_nAxesControlKernel
