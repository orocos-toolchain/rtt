#include "kernel_components/nAxesEffectorVel.hpp"
#include <assert.h>

namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_DeviceInterface;


  nAxesVelEffector::nAxesVelEffector(unsigned int num_axes, 
					 std::vector<AxisInterface*> axes,
					 std::string name)
    : nAxesVelEffector_typedef(name),
      _num_axes(num_axes), 
      _axes(axes),
      _velocity_local(num_axes)
  {
    assert(_axes.size() == num_axes);
  }


  nAxesVelEffector::~nAxesVelEffector(){};
  

  void nAxesVelEffector::pull()
  {
    // copy Output to local values
    _velocity_DOI->Get(_velocity_local);
  }


  void nAxesVelEffector::calculate()
  {}

  
  void nAxesVelEffector::push()      
  {
    for (unsigned int i=0; i<_num_axes; i++)
      _axes[i]->drive(_velocity_local[i]);
  }


  bool nAxesVelEffector::componentLoaded()
  {
    return true;
  }


  bool nAxesVelEffector::componentStartup()
  {
    // get interface to Output data type
    if ( !nAxesVelEffector_typedef::Output::dObj(   )->Get("Velocity", _velocity_DOI) ){
      cerr << "nAxesVelEffector::componentStartup() DataObjectInterface not found" << endl;
      return false;
    }
    return true;
  }
  

  bool nAxesVelEffector::updateProperties(const PropertyBag& bag)
  {
    cerr << "(Effector)  updateProperties()" << endl;
    return true;
  }


} // namespace
