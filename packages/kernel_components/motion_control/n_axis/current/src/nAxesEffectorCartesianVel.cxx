#include "kernel_components/nAxesEffectorCartesianVel.hpp"
#include <assert.h>

namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_DeviceInterface;


  nAxesEffectorCartesianVel::nAxesEffectorCartesianVel(unsigned int num_axes, 
						       std::vector<AxisInterface*> axes,
						       ORO_KinDyn::KinematicsComponent* kin,
						       std::string name)
    : nAxesEffectorCartesianVel_typedef(name),
      _num_axes(num_axes), 
      _axes(axes),
      _kinematics(kin),
      _velocity_joint_local(num_axes),
      _position_sensors(num_axes)
  {
    assert(_axes.size() == num_axes);

    // get position sensors from axes
    for (unsigned int i=0; i<_num_axes; i++){
      _position_sensors[i] = _axes[i]->getSensor("Position");
      assert( _position_sensors[i] != NULL );
    }
  }


  nAxesEffectorCartesianVel::~nAxesEffectorCartesianVel(){};
  

  void nAxesEffectorCartesianVel::pull()
  {
    // copy Output to local values
    _velocity_cartesian_DOI->Get(_velocity_cartesian_local);
  }


  void nAxesEffectorCartesianVel::calculate()
  {
    ORO_CoreLib::Double6D q, q_dot;
    for (unsigned int i=0; i<_num_axes; i++)
      q[i] = _position_sensors[i]->readSensor();

    // convert twist
    _kinematics->positionForward(q, _position_cartesian );
    _kinematics->velocityInverse(q, _velocity_cartesian_local.RefPoint(_position_cartesian.p * -1), q_dot);
    for (unsigned int i=0; i<_num_axes; i++)
      _velocity_joint_local[i] = q_dot[i];
  }

  
  void nAxesEffectorCartesianVel::push()      
  {
    for (unsigned int i=0; i<_num_axes; i++)
      _axes[i]->drive(_velocity_joint_local[i]);
  }


  bool nAxesEffectorCartesianVel::componentLoaded()
  {
    return true;
  }


  bool nAxesEffectorCartesianVel::componentStartup()
  {
    // get interface to Output data type
    if ( !nAxesEffectorCartesianVel_typedef::Output::dObj(   )->Get("Velocity", _velocity_cartesian_DOI) ){
      cerr << "nAxesEffectorCartesianVel::componentStartup() DataObjectInterface not found" << endl;
      return false;
    }
    return true;
  }
  

  bool nAxesEffectorCartesianVel::updateProperties(const PropertyBag& bag)
  {
    cerr << "(Effector)  updateProperties()" << endl;
    return true;
  }


} // namespace
