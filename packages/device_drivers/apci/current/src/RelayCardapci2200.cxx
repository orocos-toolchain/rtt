/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:32 CEST 2004  RelayCardapci2200.cxx 

                        RelayCardapci2200.cxx -  description
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
#include <pkgconf/system.h>
#include <pkgconf/device_drivers_apci.h>
#if defined(OROINT_DEVICE_DRIVERS_APCI2200) && defined(OROPKG_OS_LXRT)
#include "apci_lxrt.h"
#elif defined (OROINT_DEVICE_DRIVERS_APCI2200_IK)
extern "C"
{
#define new _new
#define class _class
#define LINUX_PCI_H
#include "apci2200/apci2200.h"
#undef class
#undef new
}
#endif

#include <device_drivers/RelayCardapci2200.hpp>

namespace ORO_DeviceDriver
{

    RelayCardapci2200::RelayCardapci2200( const std::string& name )
            : DigitalOutInterface( name ), output_cache(0)
    {
        rtos_printf("APCI2200 Relay init.\n");
#ifdef OROINT_DEVICE_DRIVERS_APCI2200
        if ( ( apci2200 = apci2200_get_device() ) != 0)
        {
            rtos_printf("Acquired apci2200 handle.\n");
            apci2200_set_output_off( apci2200, ~output_cache );
        } else
            rtos_printf("FAILED TO Acquire apci2200 handle.\n");
#endif
    }

    RelayCardapci2200::~RelayCardapci2200()
    {}

    void RelayCardapci2200::switchOn( unsigned int bit )
    {
        output_cache |= (1 << bit);
#ifdef OROINT_DEVICE_DRIVERS_APCI2200
        apci2200_set_output_on( apci2200, output_cache );
#endif
    }

    void RelayCardapci2200::switchOff( unsigned int bit )
    {
        output_cache &= ~(1 << bit);
#ifdef OROINT_DEVICE_DRIVERS_APCI2200
        apci2200_set_output_off( apci2200, ~output_cache );
#endif
    }

    void RelayCardapci2200::setBit( unsigned int bit, bool value )
    {
#ifdef OROINT_DEVICE_DRIVERS_APCI2200
        if (value)
        {
            output_cache |= (1 << bit);
            apci2200_set_output_on( apci2200, output_cache );
        }
        else
        {
            output_cache &= ~(1 << bit);
            apci2200_set_output_off( apci2200, ~output_cache );
        }
#endif
    }

    void RelayCardapci2200::setSequence(unsigned int start_bit, unsigned int stop_bit, unsigned int value)
    {
        unsigned int mask = ( (~0 << (stop_bit + 1)) | (~0 >> (sizeof(unsigned int) * 8 - start_bit)) );
        output_cache &= mask;
        output_cache |= (value << start_bit);
#ifdef OROINT_DEVICE_DRIVERS_APCI2200
        apci2200_set_output_on( apci2200, output_cache );
        apci2200_set_output_off( apci2200, ~output_cache);
#endif
    }

    bool RelayCardapci2200::checkBit(unsigned int n) const
    {
        return (output_cache >> n) & 0x1;
    }

    unsigned int RelayCardapci2200::checkSequence( unsigned int start_bit, unsigned int stop_bit ) const
    {
        unsigned int mask = (( 1 << (stop_bit - start_bit + 1) ) - 1);
        return (output_cache >> start_bit) & mask;
    }

    RelayCardapci2200::DWord RelayCardapci2200::nbOfOutputs() const
    {
#ifdef OROINT_DEVICE_DRIVERS_APCI2200
        return apci2200_get_number_off_digital_outputs();
#else 
        return 0;
#endif
    }

    /*
    RelayCardapci2200::DWord RelayCardapci2200::inputStatusGet()
    {
#ifdef OROINT_DEVICE_DRIVERS_APCI2200
        return apci2200_get_input_status( apci2200 );
#else 
        return 0;
#endif
    }*/

}
