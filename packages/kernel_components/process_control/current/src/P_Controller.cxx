
#pragma implementation
#include "kernel_components/P_Controller.hpp"

namespace ORO_ControlKernel {

/*****************************************
 * Implementation of controller class P_Controller
 *****************************************/
P_Controller::P_Controller(unsigned int num_axes) 
  : Base("P_Controller"),
    _num_axes(num_axes),
    _controller_gain("K", "Proportional Gain")
{
  _result.resize(_num_axes, 0.0);
  _refPos.resize(_num_axes, 0.0);
  _xyerr.resize(_num_axes, 0.0);
  _measPos.resize(_num_axes, 0.0);
  _controller_gain.set().resize(num_axes, 1.0); // set gain to one
}

P_Controller::~P_Controller(){};

bool P_Controller::componentLoaded()
{
    return Base::Output::dObj()->Get("ChannelValues",outp_dObj);
}

bool P_Controller::componentStartup()
{
    if ( Base::SetPoint::dObj()->Get("ChannelValues",setp_dObj) &&
         Base::Input::dObj()->Get("ChannelValues", inp_dObj) )
        {
            this->update();
            return true;
        }
    return false;
}



void P_Controller::pull()      
{
  _refPos    = setp_dObj->Get();
  // No estimator in this kernel: Read directly from SensorInputs
  _measPos   = inp_dObj->Get();
}


void P_Controller::calculate() 
{
  for (unsigned int i=0; i< _num_axes; i++)
    {
      _xyerr[i] = _refPos[i] - _measPos[i];
      _result[i] = _xyerr[i] * _controller_gain.get()[i];
    }
}

bool P_Controller::updateProperties(const PropertyBag& bag)
{
  std::cerr << "P_Controller::updateProperties()" << std::endl;
  return composeProperty(bag, _controller_gain);
}


void P_Controller::push()      
{
  outp_dObj->Set(_result);
}

}
