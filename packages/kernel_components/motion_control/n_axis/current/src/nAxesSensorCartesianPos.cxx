#include "kernel_components/nAxesSensorCartesianPos.hpp"
#include <assert.h>

namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_DeviceInterface;


  nAxesSensorCartesianPos::nAxesSensorCartesianPos(unsigned int num_axes, 
						   std::vector<AxisInterface*> axes,
						   ORO_KinDyn::KinematicsComponent* kin,
						   std::string name)
    : nAxesSensorCartesianPos_typedef(name),
      _num_axes(num_axes), 
      _axes(axes),
      _kinematics(kin),
      _position_joint(num_axes),
      _position_sensors(num_axes)
  {
    assert(_axes.size() == num_axes);

    // get position sensors from axes
    for (unsigned int i=0; i<_num_axes; i++){
      _position_sensors[i] = _axes[i]->getSensor("Position");
      assert( _position_sensors[i] != NULL );
    }
  }


  nAxesSensorCartesianPos::~nAxesSensorCartesianPos(){};
  

  void nAxesSensorCartesianPos::pull()
  {
    // copy values from position sensors to local variable
    for (unsigned int i=0; i<_num_axes; i++)
      _position_joint[i] = _position_sensors[i]->readSensor();
  }



  void nAxesSensorCartesianPos::calculate()
  {
    // forwarard kinematics
    ORO_CoreLib::Double6D temp;
    for (unsigned int i=0; i<_num_axes; i++)
      temp[i] = _position_joint[i];
    
    _kinematics->positionForward(temp, _position_out_local );
  }


  
  void nAxesSensorCartesianPos::push()      
  {
    _position_out_DOI->Set(_position_out_local);
  }



  bool nAxesSensorCartesianPos::componentLoaded()
  {
    // get interface to Input data types
    if (!Sensor< Writes<nAxesSensorCartesianPosInput_pos> >::Input::dObj()->Get("Position", _position_out_DOI)){
      cerr << "nAxesSensorCartesianPos::componentLoaded() DataObjectInterface not found" << endl;
      return false;
    }

    return true;
  }


  bool nAxesSensorCartesianPos::componentStartup()
  {
    return true;
  }
  
  

} // end namespace ORO_nAxesControlKernel
