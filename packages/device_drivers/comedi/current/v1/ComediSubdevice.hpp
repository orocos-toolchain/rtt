/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:44 CEST 2002  ComediSubdevice.hpp 

                        ComediSubdevice.hpp -  description
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
 
 
#ifndef COMEDISUBDEVICE_HPP
#define COMEDISUBDEVICE_HPP

#include "ComediDevice.hpp" 
//#include "ComediChannel.hpp"

class ComediSubdevice
{

    public:
        /*
         * Create a new subdevice, which is part of a device
         *
         *  @param comedidevice : the device containing the requested subdevice
         *  @param subdevicenumber : the number of the subdevice from Comedidevice comdedidevice, which has to be created
         */
        ComediSubdevice( ComediDevice* comedidevice, unsigned int subdevicenumber );

        /*
         * delete this ComediSubdevice
         */
        ~ComediSubdevice();

        //subdevice queries
        int get_subdevice_type();
        int get_subdevice_flags();
        int get_n_channels();
        //        int range_is_chan_specific();
        //        int maxdata_is_chan_specific();

        //channel queries
        lsampl_t get_maxdata( unsigned int chan );
        int get_n_ranges( unsigned int chan );
        //        comedi_range* get_range(unsigned int chan, int range);
        //        int find_range(unsigned int chan, unsigned int unit, double min, double max);

        //buffer queries
        //        int get_buffer_size();
        //        int get_max_buffer_size();
        //        int set_buffer_size(unsigned int len);

        //low-level-stuff

        int lock ()

            ;
        int unlock();

        //synchronous stuff
        int data_read( unsigned int chan, lsampl_t* data );

        //int data_read_n(unsigned int chan, lsampl_t* data, unsigned int n);
        //int data_read_hint(unsigned int chan);
        //int data_read_delayed(unsigned int chan, lsampl_t* data, unsigned int nano_sec);
        int data_write( unsigned int chan, lsampl_t data );

        int dio_config( unsigned int chan, unsigned int dir );

        int dio_read( unsigned int chan, unsigned int* bit );

        int dio_write( unsigned int chan, unsigned int bit );

        int dio_bitfield( unsigned int write_mask, unsigned int* bits );

        //streaming I/O (commands)
        //        int get_cmd_src_mask(comedi_cmd* cmd);
        //        int get_cmd_generic_timed(comedi_cmd* cmd, unsigned int ns);
        int cancel();

        int poll();

        //buffer control
        //        int set_max_buffer_size(unsigned int max_size);
        //        int get_buffer_contents();
        //        int mark_buffer_read(unsigned int bytes);
        //        int get_buffer_offset();

        //extra functions
        int set_rangetype( unsigned int chan, unsigned int rangetype );

        int set_aref( unsigned int chan, unsigned int aref );

        ComediDevice* get_comedidevice();

        // DO NOT USE        ComediChannel* get_comedichannel(unsigned int chan);


        unsigned int get_rangetype( unsigned int chan );

        unsigned int get_aref( unsigned int aref );

    protected:
        ComediDevice* device;                   //pointer naar bovenliggende comedidevice

        unsigned int subdevice;                  //number of the subdevice
};

#endif
