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

#include "EncoderPositionInterface.hpp"
#include <os/fosi.h>

namespace ORO_DeviceDriver
{
  using namespace ORO_DeviceInterface;

  // 
  template <class Data>
  EncoderPositionInterface<Data>::EncoderPositionInterface(EncoderInterface * enc_p, 
							   Data scale,Data offset)
  {
    rtos_printf( "Creating EncoderPositionInterface\n" );
    _encoders_p.resize(1);
    _encoders_p[0] = enc_p;
    _scale.resize(1);
    _scale[0] = scale;
    _offset.resize(1);
    _offset[0] = offset;
  }

  template <class Data>
  EncoderPositionInterface<Data>::EncoderPositionInterface(std::vector<EncoderInterface *> enc_p,
							   std::vector<Data> scale, std::vector<Data> offset)
  {
    if ( (enc_p.size() == scale.size()) && (enc_p.size() == offset.size()) )
      {    
	rtos_printf( "Creating EncoderPositionInterface\n" );
	_encoders_p = enc_p;
	_scale = scale;
	_offset = offset;
      }
    else 
      {
	rtos_printf( "EPI: Sizes do not match!!\n");
      }
  }
  
  template <class Data>
  EncoderPositionInterface<Data>::~EncoderPositionInterface(){};

  template <class Data>
  int EncoderPositionInterface<Data>::PositionGet(std::vector<Data> & p)
  {
    int ret;
    unsigned int size = _encoders_p.size();

    if ( size != p.size())
      {
	rtos_printf("EncoderPositionInterface::PositionGet() WARNING : size of arg does not correspond to num Encoders BYE BYE REALTIME\n");
	p.resize(size);
      }
    for (unsigned int i = 0; i < size; i++)
      {
	// Read encoders.  TODO: Is this correct (upcounting)?
	p[i] = static_cast<double>((_encoders_p[i])->positionGet() + 
	  ( (_encoders_p[i])->turnGet() * (_encoders_p[i])->resolution() ));
	// Take into account scale and offset
	p[i] *= _scale[i];
	p[i] += _offset[i];
      }
    ret = 0;
    return ret;
  }

} // End namespace
