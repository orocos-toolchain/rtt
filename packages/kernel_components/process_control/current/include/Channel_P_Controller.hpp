// Copyright (C) 2003 Klaas Gadeyne <klaas.gadeyne@mech.kuleuven.ac.be>
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  

#ifndef __CHANNEL_P_CONTROLLER_H__
#define __CHANNEL_P_CONTROLLER_H__

#include <control_kernel/DataServer.hpp>

namespace ORO_ControlKernel
{

    /**
     * The output of the Channel_P_Controller.
     */
    struct Channel_P_Controller_Output
        : ServedTypes< std::vector<double> > 
    {
        Channel_P_Controller_Output() {
            this->insert(std::make_pair(0, "ChannelValues"));
        }
    };

  /**
     A controller for XY[Z]* table, with reporting and updateProperties
     - In: Position Setpoints and Model
     - Out: Velocity sendpoints to send to the effector
  */
  template <class Base> class Channel_P_Controller
    : public Base
  {
  protected:
      typedef std::vector<double> ChannelType;
      DataObjectInterface<ChannelType>* inp_dObj;
      DataObjectInterface<ChannelType>* outp_dObj;
      DataObjectInterface<ChannelType>* setp_dObj;
      ChannelType   _result;
      ChannelType   _refPos;
      ChannelType   _measPos;

      double _K;
      unsigned int _num_axes;

      ChannelType _xyerr;

      Property<double> _controller_gain;

  public:
    Channel_P_Controller(unsigned int _num_axes);
    virtual ~Channel_P_Controller();

    // Redefining virtual members
    virtual bool updateProperties(const PropertyBag& bag);
      virtual bool componentLoaded();
      virtual bool componentStartup();

    virtual void pull();
    virtual void calculate();
    virtual void push();

  };


#include "Channel_P_Controller.inc"

}

#endif // __N_AXES_P_CONTROLLER_H__
