// $Id: $
// Copyright (C) 2004 Klaas Gadeyne <klaas.gadeyne@mech.kuleuven.ac.be>
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

#ifndef __ENCODER_POSITION_INTERFACE_HPP__
#define __ENCODER_POSITION_INTERFACE_HPP__

#include "PositionInterface.hpp"
#include "EncoderInterface.hpp"
#include <vector>

namespace RTT
{
  
  /**

  */
  template <class PositionData>
  class EncoderPositionInterface :
    public PositionInterface<PositionData>
  {
  protected:
    typedef PositionData Data;

    std::vector<Data> _data;
    std::vector<EncoderInterface *> _encoders_p;
    std::vector<Data> _scale;
    std::vector<Data> _offset;
    
  public:
    /// Constructor with 1 encoder
    EncoderPositionInterface(EncoderInterface * enc_p,
			     Data scale, Data offset);

    /// Constructor with vector of encoders
    EncoderPositionInterface(std::vector<EncoderInterface *> enc_p,
			     std::vector<Data> scale, std::vector<Data> offset);
    /// Destructor
    virtual ~EncoderPositionInterface();
   
    // Redefine Pure virtuals
    virtual int PositionGet( std::vector<Data>& p );
    // Not sure if these belong in PositionInterface...
    // virtual std::vector<Data> maxMeasurement(){};
    // virtual std::vector<Data> minMeasurement(){};
    // virtual std::vector<Data> zeroMeasurement(){};
  };

} // End namespace

// Template instantiation
#include "EncoderPositionInterface.inc"

#endif // __ENCODER_POSITION_INTERFACE_HPP__
