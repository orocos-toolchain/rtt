
#pragma implementation
#include "kernel_components/P_Controller.hpp"

namespace ORO_ControlKernel {

/*****************************************
 * Implementation of controller class P_Controller
 *****************************************/
P_Controller::P_Controller(unsigned int num_axes) 
  : Base("P_Controller"),
    _num_axes(num_axes),
    _controller_gain("k", "Proportional Gain", 1.0)
{
  _result.resize(_num_axes, 0.0);
  _refPos.resize(_num_axes, 0.0);
  _xyerr.resize(_num_axes, 0.0);
  _measPos.resize(_num_axes, 0.0);
}

P_Controller::~P_Controller(){};

bool P_Controller::componentLoaded()
{
    return Base::Output::dObj()->Get("ChannelValues",outp_dObj);
}

bool P_Controller::componentStartup()
{
    if ( Base::SetPoint::dObj()->Get("ChannelValues",setp_dObj) &&
         Base::Input::dObj()->Get("ChannelMeasurements", inp_dObj) )
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
      _result[i] = _xyerr[i] * _controller_gain.value();
    }
}

bool P_Controller::updateProperties(const PropertyBag& bag)
{
  composeProperty(bag, _controller_gain);
  std::cerr << "P_Controller::updateProperties() : _K = " << _controller_gain.value() << std::endl;
  return true;
}


void P_Controller::push()      
{
  outp_dObj->Set(_result);
}

}
