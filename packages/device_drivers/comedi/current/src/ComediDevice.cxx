/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:32 CEST 2004  ComediDevice.cxx 

                        ComediDevice.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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

/* Klaas Gadeyne, August 14, 2003  
   Major rewrite of original file by Peter Soetens
*/

#include "comedi/ComediDevice.hpp"
#include <os/fosi.h>

#include "comedi_internal.h"

extern "C"
{
#include <cerrno>
}

namespace ORO_DeviceDriver
{
  typedef unsigned int Data;

  ComediDevice::ComediDevice( unsigned int minor )
    : devminor( minor ), error( 0 )
  {
    it = 0;

    char devString[ 15 ];
    sprintf( devString, "/dev/comedi%d", devminor );

    it = comedi_open( devString );

    rtos_printf( "Trying to open %s\n", devString );

    if ( it == 0 )
      {
	rtos_printf( "comedi_open failed\n" );
	error = -EINVAL;
      }
  }

  ComediDevice::~ComediDevice()
  {
    if (comedi_close(it) < 0)
      {
	rtos_printf( "comedi_close failed\n" );
      }
  }

  // KG MaxData only depending on subdevice?
  // Always take channel 0;
  Data ComediDevice::getMaxData(unsigned int subd)
  {
    unsigned int channel = 0;
    return (Data) comedi_get_maxdata( it, subd, channel );
  }

  int ComediDevice::getSubDeviceType(unsigned int subd)
  {
    return comedi_get_subdevice_type( it, subd );
  }

  int ComediDevice::read( unsigned int subd, unsigned int chanNr,
			  unsigned int range, unsigned int aref,
			  ComediDevice::Data& value )
  {
    value = 0;
    if ( error )
      return -1;

    comedi_data_read( it, subd, chanNr, range, aref, 
		      ( lsampl_t* ) & value );

    return 0;
  }

  int ComediDevice::write( unsigned int subd, unsigned int chanNr,
			   unsigned int range, unsigned int aref,
			   const ComediDevice::Data& value)
  {
    if ( error )
      return -1;

    Data output = value; 

    comedi_data_write( it, subd, chanNr, range, aref, output );

    return 0;
  }

  comedi_t* ComediDevice::getDevice() 
  { 
    return it; 
  }

}
