#include "Channel_P_Controller.hpp"

/*****************************************
 * Implementation of controller class Channel_P_Controller
 *****************************************/
template <class Base> 
Channel_P_Controller<Base>::Channel_P_Controller(unsigned int num_axes) 
  : Base("Channel_P_Controller"),
    _num_axes(num_axes),
    _controller_gain("k", "Proportional Gain", 1.0)
{
  _result.resize(_num_axes, 0.0);
  _refPos.resize(_num_axes, 0.0);
  _xyerr.resize(_num_axes, 0.0);
  _measPos.resize(_num_axes, 0.0);
}

template <class Base> 
Channel_P_Controller<Base>::~Channel_P_Controller(){};

template <class Base> 
bool Channel_P_Controller<Base>::componentLoaded()
{
    return Base::Output::dObj()->Get("ChannelValues",outp_dObj);
}

template <class Base> 
bool Channel_P_Controller<Base>::componentStartup()
{
    if ( Base::SetPoint::dObj()->Get("ChannelValues",setp_dObj) &&
         Base::Input::dObj()->Get("ChannelMeasurements", inp_dObj) )
        {
            this->update();
            return true;
        }
    return false;
}



template <class Base>             
void Channel_P_Controller<Base>::pull()      
{
  _refPos    = setp_dObj->Get();
  // No estimator in this kernel: Read directly from SensorInputs
  _measPos   = inp_dObj->Get();
}

template <class Base> 
void Channel_P_Controller<Base>::calculate() 
{
  for (unsigned int i=0; i< _num_axes; i++)
    {
      _xyerr[i] = _refPos[i] - _measPos[i];
      _result[i] = _xyerr[i] * _controller_gain.value();
    }
}

template <class Base>
bool Channel_P_Controller<Base>::updateProperties(const PropertyBag& bag)
{
  composeProperty(bag, _controller_gain);
  cerr << "Channel_P_Controller::updateProperties() : _K = " << _controller_gain.value() << endl;
  return true;
}


template <class Base> 
void Channel_P_Controller<Base>::push()      
{
  outp_dObj->Set(_result);
}
