/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:32 CEST 2004  ComediChannel.cxx 

                        ComediChannel.cxx -  description
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
#include "corelib/ComediChannel.hpp"

/***********************/
/* con- & destructors  */
/***********************/

ComediChannel::ComediChannel( ComediDevice* comedidevice, unsigned int subdevicenumber, unsigned int channum )
{
    device = comedidevice;
    subdevice = subdevicenumber;
    chan = channum;
}

ComediChannel::~ComediChannel()
{}

/***********************/
/*   channel queries   */
/***********************/

lsampl_t ComediChannel::get_maxdata()
{
    return device->get_maxdata( subdevice, chan );
}

int ComediChannel::get_n_ranges()
{
    return device->get_n_ranges( subdevice, chan );
}

/*comedi_range* ComediChannel::get_range(int range)
{
        return device->get_range(subdevice, chan, range);
}
 
int ComediChannel::find_range(unsigned int unit, double min, double max)
{
        return device->find_range(subdevice, chan, unit, min, max);
}*/

/**************************/
/*   synchronous stuff    */
/**************************/

int ComediChannel::data_read( lsampl_t* data )
{
    return device->data_read( subdevice, chan, data );
}

/*int ComediChannel::data_read_n(lsampl_t* data, unsigned int n)
{
        return device->data_read_n(subdevice, chan, data, n); 
}*/

/*int ComediChannel::data_read_hint()
{
        return device->data_read_hint(subdevice, chan);
}*/

/*int ComediChannel::data_read_delayed(lsampl_t* data, unsigned int nano_sec)
{
        return device->data_read_delayed(subdevice, chan, data, nano_sec);
}*/

int ComediChannel::data_write( lsampl_t data )
{
    return device->data_write( subdevice, chan, data );
}

int ComediChannel::dio_config( unsigned int dir )
{
    return device->dio_config( subdevice, chan, dir );
}

int ComediChannel::dio_read( unsigned int* bit )
{
    return device->dio_read( subdevice, chan, bit );
}

int ComediChannel::dio_write( unsigned int bit )
{
    return device->dio_write( subdevice, chan, bit );
}

/**********************/
/*   extra functions  */
/**********************/

int ComediChannel::set_rangetype( unsigned int rangetype )
{
    return device->set_rangetype( subdevice, chan, rangetype );
}

int ComediChannel::set_aref( unsigned int aref )
{
    return device->set_aref( subdevice, chan, aref );
}

unsigned int ComediChannel::get_rangetype()
{
    return device->get_rangetype( subdevice, chan );
}

unsigned int ComediChannel::get_aref()
{
    return device->get_aref( subdevice, chan );
}

ComediDevice* ComediChannel::get_comedidevice()
{
    return device;
}

ComediSubdevice* ComediChannel::get_comedisubdevice()
{
    return 0; //device->get_comedisubdevice(subdevice);
}

