#include "kernel_components/nAxesEffectorVel.hpp"
#include <assert.h>

namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;
  using namespace ORO_DeviceInterface;


  nAxesEffectorVel::nAxesEffectorVel(unsigned int num_axes, 
					 std::vector<AxisInterface*> axes,
					 std::string name)
    : nAxesEffectorVel_typedef(name),
      _num_axes(num_axes), 
      _axes(axes),
      _velocity_local(num_axes)
  {
    assert(_axes.size() == num_axes);
  }


  nAxesEffectorVel::~nAxesEffectorVel(){};
  

  void nAxesEffectorVel::pull()
  {
    // copy Output to local values
    _velocity_DOI->Get(_velocity_local);
  }


  void nAxesEffectorVel::calculate()
  {}

  
  void nAxesEffectorVel::push()      
  {
    for (unsigned int i=0; i<_num_axes; i++)
      _axes[i]->drive(_velocity_local[i]);
  }


  bool nAxesEffectorVel::componentLoaded()
  {
    return true;
  }


  bool nAxesEffectorVel::componentStartup()
  {
    // get interface to Output data type
    if ( !nAxesEffectorVel_typedef::Output::dObj(   )->Get("Velocity", _velocity_DOI) ){
      cerr << "nAxesEffectorVel::componentStartup() DataObjectInterface not found" << endl;
      return false;
    }
    return true;
  }
  

  bool nAxesEffectorVel::updateProperties(const PropertyBag& bag)
  {
    cerr << "(Effector)  updateProperties()" << endl;
    return true;
  }


} // namespace
