/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:44 CEST 2002  ComediChannel.hpp 

                        ComediChannel.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef COMEDICHANNEL_HPP
#define COMEDICHANNEL_HPP

#include "ComediDevice.hpp"
#include "ComediSubdevice.hpp"

class ComediChannel
{

    public:
        /**
         * Create a channel given a ComediDevice, device number and channel nummber
         * 
         *  @param device : the device which contains the requested channel
         *  @param subdevicenumber : the number of the subdevice which contains the requested channel
         *  @param chan : the requested channel
         */
        ComediChannel( ComediDevice* device, unsigned int subdevicenumber, unsigned int chan );

        /*
         * delete this channel
         */
        ~ComediChannel();

        //channel queries
        lsampl_t get_maxdata();
        int get_n_ranges();
        //        comedi_range* get_range(int range);
        //        int find_range(unsigned int unit, double min, double max);

        //synchronous stuff
        int data_read( lsampl_t* data );
        //int data_read_n(lsampl_t* data, unsigned int n);
        //int data_read_hint();
        //int data_read_delayed(lsampl_t* data, unsigned int nano_sec);
        int data_write( lsampl_t data );
        int dio_config( unsigned int dir );
        int dio_read( unsigned int* bit );
        int dio_write( unsigned int bit );

        //extra functions
        int set_rangetype( unsigned int rangetype );
        int set_aref( unsigned int aref );
        ComediDevice* get_comedidevice();
        ComediSubdevice* get_comedisubdevice();

        unsigned int get_rangetype();
        unsigned int get_aref();

    protected:
        ComediDevice* device;                   //pointer naar bovenliggende comedidevice
        unsigned int subdevice;                     //number of the subdevice
        unsigned int chan;                   //number of the channel
};

#endif
