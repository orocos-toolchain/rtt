/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:05 CEST 2004  P_Controller.cxx 

                        P_Controller.cxx -  description
                           -------------------
    begin                : Thu July 15 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens at mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#pragma implementation
#include "kernel_components/P_Controller.hpp"

namespace ORO_ControlKernel {

/*****************************************
 * Implementation of controller class P_Controller
 *****************************************/
P_Controller::P_Controller(unsigned int num_axes , const std::string& name) 
  : Base( name ),
    _num_axes(num_axes),
    _xyerr("Track_Err", "Tracking error", std::vector<double>( num_axes, 0) ),
    _controller_gain("K", "Proportional Gain", std::vector<double>( num_axes, 1.0))
{
  _result.resize(_num_axes, 0.0);
  _refPos.resize(_num_axes, 0.0);
  _measPos.resize(_num_axes, 0.0);
}

P_Controller::~P_Controller(){};

bool P_Controller::componentLoaded()
{
    return Output::dObj()->Get("ChannelValues",outp_dObj);
}

bool P_Controller::componentStartup()
{
    if ( SetPoint::dObj()->Get("ChannelValues",setp_dObj) &&
         Input::dObj()->Get("ChannelValues", inp_dObj) )
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
      _xyerr.set()[i] = _refPos[i] - _measPos[i];
      _result[i] = _xyerr.get()[i] * _controller_gain.get()[i];
    }
}

bool P_Controller::updateProperties(const PropertyBag& bag)
{
  return composeProperty(bag, _controller_gain);
}

void P_Controller::exportProperties(PropertyBag& bag)
{
    bag.add( &_controller_gain );
}

void P_Controller::exportReports(PropertyBag& bag)
{
    bag.add( &_xyerr );
}


void P_Controller::push()      
{
  outp_dObj->Set(_result);
}

}
