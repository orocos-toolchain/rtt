#include "corelib/ComediSubdevice.hpp"

/***********************/
/* con- & destructors  */
/***********************/

ComediSubdevice::ComediSubdevice( ComediDevice* comedidevice, unsigned int subdevicenumber )
{
    device = comedidevice;
    subdevice = subdevicenumber;
}

ComediSubdevice::~ComediSubdevice()
{
    device = 0;
    //??????????????????
}

/***********************/
/*  subdevice queries  */
/***********************/

int ComediSubdevice::get_subdevice_type()
{
    return device->get_subdevice_type( subdevice );
}

int ComediSubdevice::get_subdevice_flags()
{
    return device->get_subdevice_flags( subdevice );
}

int ComediSubdevice::get_n_channels()
{
    return device->get_n_channels( subdevice );
}

/*int ComediSubdevice::range_is_chan_specific()
{
        return device->range_is_chan_specific(subdevice);
}
 
int ComediSubdevice::maxdata_is_chan_specific()
{
        return device->maxdata_is_chan_specific(subdevice);
}*/

/***********************/
/*   channel queries   */
/***********************/

lsampl_t ComediSubdevice::get_maxdata( unsigned int chan )
{
    return device->get_maxdata( subdevice, chan );
}

int ComediSubdevice::get_n_ranges( unsigned int chan )
{
    return device->get_n_ranges( subdevice, chan );
}

/*comedi_range* ComediSubdevice::get_range(unsigned int chan, int range)
{
        return device->get_range(subdevice, chan, range);
}
 
int ComediSubdevice::find_range(unsigned int chan, unsigned int unit, double min, double max)
{
        return device->find_range(subdevice, chan, unit, min, max);
}*/

/***********************/
/*    buffer queries   */
/***********************/

/*int ComediSubdevice::get_buffer_size()
{
        return device->get_buffer_size(subdevice);
}
 
int ComediSubdevice::get_max_buffer_size()
{
        return device->get_max_buffer_size(subdevice);
}
 
int ComediSubdevice::set_buffer_size(unsigned int len)
{
        return device->set_buffer_size(subdevice, len);
}*/

/**************************/
/*   synchronous stuff    */
/**************************/

int ComediSubdevice::data_read( unsigned int chan, lsampl_t* data )
{
    return device->data_read( subdevice, chan, data );
}

/*int ComediSubdevice::data_read_n(unsigned int chan, lsampl_t* data, unsigned int n)
{
        return device->data_read_n(subdevice, chan, data, n);
}*/

/*int ComediSubdevice::data_read_hint(unsigned int chan)
{
        return device->data_read_hint(subdevice, chan); 
}*/

/*int ComediSubdevice::data_read_delayed(unsigned int chan, lsampl_t* data, unsigned int nano_sec)
{
        return device->data_read_delayed(subdevice, chan, data, nano_sec); 
}*/

int ComediSubdevice::data_write( unsigned int chan, lsampl_t data )
{
    return device->data_write( subdevice, chan, data );
}

int ComediSubdevice::dio_config( unsigned int chan, unsigned int dir )
{
    return device->dio_config( subdevice, chan, dir );
}

int ComediSubdevice::dio_read( unsigned int chan, unsigned int* bit )
{
    return device->dio_read( subdevice, chan, bit );
}

int ComediSubdevice::dio_write( unsigned int chan, unsigned int bit )
{
    return device->dio_write( subdevice, chan, bit );
}

int ComediSubdevice::dio_bitfield( unsigned int write_mask, unsigned int* bits )
{
    return device->dio_bitfield( subdevice, write_mask, bits );
}

/****************************/
/* streaming I/O (commands) */
/****************************/

/*int ComediSubdevice::get_cmd_src_mask(comedi_cmd* cmd)
{
        return device->get_cmd_src_mask(subdevice, cmd);
}
 
int ComediSubdevice::get_cmd_generic_timed(comedi_cmd* cmd, unsigned int ns)
{
        return device->get_cmd_generic_timed(subdevice, cmd, ns);
}*/

int ComediSubdevice::cancel()
{
    return device->cancel( subdevice );
}

int ComediSubdevice::poll()
{
    return device->poll( subdevice );
}

/***************************/
/*      buffer control     */
/***************************/

/*int ComediSubdevice::set_max_buffer_size(unsigned int max_size)
{
        return device->set_max_buffer_size(subdevice, max_size);
}
 
int ComediSubdevice::get_buffer_contents()
{
        return device->get_buffer_contents(subdevice);
}
 
int ComediSubdevice::mark_buffer_read(unsigned int bytes)
{
        return device->mark_buffer_read(subdevice, bytes);
}
 
int ComediSubdevice::get_buffer_offset()
{
        return device->get_buffer_offset(subdevice);
}*/

/**********************/
/*   extra functions  */
/**********************/

int ComediSubdevice::set_rangetype( unsigned int chan, unsigned int rangetype )
{
    return device->set_rangetype( subdevice, chan, rangetype );
}

int ComediSubdevice::set_aref( unsigned int chan, unsigned int aref )
{
    return device->set_aref( subdevice, chan, aref );
}

unsigned int ComediSubdevice::get_rangetype( unsigned int chan )
{
    return device->get_rangetype( subdevice, chan );
}

unsigned int ComediSubdevice::get_aref( unsigned int chan )
{
    return device->get_aref( subdevice, chan );
}

ComediDevice* ComediSubdevice::get_comedidevice()
{
    return device;
}

/** DO NOT USE
ComediChannel* ComediSubdevice::get_comedichannel(unsigned int chan)
{
        return device->get_comedichannel(subdevice, chan);
}
*/


