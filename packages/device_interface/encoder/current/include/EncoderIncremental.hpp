// $Id: EncoderIncremental.hpp,v 1.18 2003/08/08 12:38:41 kgadeyne Exp $
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

#ifndef __ENCODERINCREMENTAL_HPP
#define __ENCODERINCREMENTAL_HPP


#include <device_interface/SensorInterface.hpp>
#include <device_interface/EncoderIncrementalInterface.hpp>
#include <string>

namespace ORO_DeviceInterface
{

  /**
   * An Interface for reading an incremental encoder, reading
   * the counter or reading the position using conversions.  The
   * default Position Unittype is a double<
   */
  template<class UnitType=double> class EncoderIncremental :
    public EncoderIncrementalInterface
  {

  public:
    /**
     * Create a nameserved encoder.
     *
     * @param name  The name of the encoder.
     * @param Scale The scale factor for reading the position.
     * @param Offset The offset (in physical units) for reading the position.
     */
    EncoderIncremental(const std::string& name, UnitType Scale=1.0, UnitType Offset=0.0 );

    /**
     * Create an encoder.
     *
     * @param Scale The scale factor for reading the position.
     * @param Offset The offset (in physical units) for reading the position.
     */
    EncoderIncremental(UnitType Scale=1.0, UnitType Offset=0.0 );

    virtual ~EncoderIncremental();

    virtual int readCounter( int& p) = 0;

    /**
     * Reads a converted position, according to Scale and Offset parameters
     * in the constructor.
     */
    virtual int readPosition( UnitType& p );

    virtual void reset( int value=0 ) = 0;

  protected:
    UnitType scale;
    UnitType offset;
  };

};

// Template instantiation
#include "EncoderIncremental.inc"

#endif

