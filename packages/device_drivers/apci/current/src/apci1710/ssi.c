/**
* Copyright (c) 2001 Jef de Geeter
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "ssi.h"
#include "apci1710.h"
#include <asm/io.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/kernel.h>

//apci1710_ssi ssi;

int init_module( void )
{

    return 0;
}

void cleanup_module( void )
{
}

int apci1710_ssi_init( apci1710_device* dev, int moduleNb, int ssiProfile, int positionBitLength, int turnBitLength, int countingMode, unsigned int ssiOutputClock )
{

    unsigned int timervalue;

    // check if the module is a SSI counter
    printk("Mnr: %d, prof: %d, posbit: %d, turnbit: %d, cMode: %d, clock: %d", moduleNb, ssiProfile, positionBitLength, turnBitLength, countingMode, ssiOutputClock );
    if ( dev == 0 )
        return -3;

    if ( ( dev->moduleConf[ moduleNb ] & APCI_SSI_COUNTER ) != APCI_SSI_COUNTER )
        return -4;

    // calculation of timervalue (old buggy code)
    //timervalue = ssiOutputClock / 2;

    // from dos code
    if (ssiOutputClock > 251 && ssiOutputClock <= 5000000UL)
      timervalue = APCI1710_33MHZ * 500000UL / ssiOutputClock;
    else 
      return -1;

    //rtos_printf("Writing in register %d\n",timervalue);

    // ssi profile
    // ssi.ssiProfile[ moduleNb ] = ssiProfile;

    if ( countingMode != COUNTINGMODE_BINARY && countingMode != COUNTINGMODE_GRAY )
        return -2;

    if ( countingMode == COUNTINGMODE_BINARY )
        outl( ssiProfile + 1, dev->ioaddr[ 2 ] + 4 + 64 * moduleNb );

    if ( countingMode == COUNTINGMODE_GRAY )
        outl( ssiProfile, dev->ioaddr[ 2 ] + 4 + 64 * moduleNb );

    // timervalue
    outl( timervalue, dev->ioaddr[ 2 ] + 64 * moduleNb );

    // counting mode, gray or binary
    // 7 = 0111
    outb( 7 * countingMode, dev->ioaddr[ 2 ] + 12 + 64 * moduleNb );

    printk( "apci1710 module %d initialized with ssi profile %d, output clock is %d Hz\n", moduleNb, ssiProfile, ssiOutputClock );

    return 0;
}

int apci1710_ssi_read( apci1710_device* dev, int moduleNb, int selectedSSI, unsigned int* result )
{
    if (selectedSSI < 0 || selectedSSI > 2)
        return -1;
    // checks if the module is a SSI counter
    if ( ( dev->moduleConf[ moduleNb ] & APCI_SSI_COUNTER ) != APCI_SSI_COUNTER )
        return -1;

    apci1710_ssi_refresh(dev, moduleNb );
    apci1710_ssi_read_latest(dev,moduleNb,selectedSSI, result);
    return 0;
}

int apci1710_ssi_read_all( apci1710_device* dev, int moduleNb, unsigned int* result )
{
    // checks if the module is a SSI counter
    if ( ( dev->moduleConf[ moduleNb ] & APCI_SSI_COUNTER ) != APCI_SSI_COUNTER )
        return -1;

    apci1710_ssi_refresh(dev, moduleNb );
    apci1710_ssi_read_latest_all(dev, moduleNb, result);
    return 0;
}

int apci1710_ssi_refresh(apci1710_device* dev, int moduleNb )
{
    // start the reading
    outb( 0x1, dev->ioaddr[ 2 ] + 8 + 64 * moduleNb );
    return 0;
}

int apci1710_ssi_read_latest(apci1710_device* dev, int moduleNb, int selectedSSI, unsigned int* result )
{
    if (selectedSSI < 0 || selectedSSI > 2)
        return -1;

    do
    {}
    while ( inb( dev->ioaddr[ 2 ] + 64 * moduleNb ) & 0x1 );

    // data is available
    *result = inl( dev->ioaddr[ 2 ] + 4 * ( selectedSSI + 1 ) + 64 * moduleNb );

    return 0;
}

int apci1710_ssi_read_latest_all( apci1710_device* dev, int moduleNb, unsigned int* result )
{
    int i;
    do
    {}
    while ( inb( dev->ioaddr[ 2 ] + 64 * moduleNb ) & 0x1 );

    // data is available
    for (i = 0; i < 3; ++i )
    {
        result[ i ] = inl( dev->ioaddr[ 2 ] + 4 * ( i + 1 ) + 64 * moduleNb );
    }
    return 0;
}
