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

#include "EncoderIncremental.hpp"

namespace ORO_DeviceInterface
{

  template <class UnitType>
  EncoderIncremental<UnitType>::EncoderIncremental(const std::string& name , 
						   UnitType Scale, 
						   UnitType Offset)
    : EncoderIncrementalInterface( name ), scale(Scale),offset(Offset){};
  

  template <class UnitType>
  EncoderIncremental<UnitType>::EncoderIncremental(UnitType Scale, 
						   UnitType Offset)
    : scale(Scale),offset(Offset){};
  

  template <class UnitType>
  EncoderIncremental<UnitType>::~EncoderIncremental(){};

  template <class UnitType>
  int EncoderIncremental<UnitType>::readPosition( UnitType& position )
  {
    int tmppos=0;
    readCounter(tmppos);
    // rtos_printf("Comedi Counter : reading ... %d\n",tmppos);
    position = tmppos * scale + offset;
    return 0;
  }
  
}
