#include "corelib/ComediDevice.hpp"

/***********************/
/* con- & destructors  */
/***********************/

ComediDevice::ComediDevice( const char* fn )
        : range( 0 ), aref( 0 ), it( 0 ), filename( fn )
{
    open_device();
}

ComediDevice::~ComediDevice()
{
    close_device();
}

/***********************/
/*   device queries    */
/***********************/

int ComediDevice::get_n_subdevices()
{
    if ( is_device_open() )
        return comedi_get_n_subdevices( it );

    return 0;
}

int ComediDevice::get_version_code()
{
    if ( is_device_open() )
        return comedi_get_version_code( it );

    return -1;
}

char* ComediDevice::get_driver_name()
{
    if ( is_device_open() )
        return comedi_get_driver_name( it );

    return 0;
}

char* ComediDevice::get_board_name()
{
    if ( is_device_open() )
        return comedi_get_board_name( it );

    return 0;
}

/***********************/
/*  subdevice queries  */
/***********************/

int ComediDevice::get_subdevice_type( unsigned int subdevice )
{
    if ( is_device_open() && is_valid_subdevice( subdevice ) )
        return comedi_get_subdevice_type( it, subdevice );

    return -1;
}

int ComediDevice::find_subdevice_by_type( int type, unsigned int startsubdevice )
{
    if ( is_device_open() )
        return comedi_find_subdevice_by_type( it, type, startsubdevice );

    return -1;
}

int ComediDevice::get_subdevice_flags( unsigned int subdevice )
{
    if ( is_device_open() && is_valid_subdevice( subdevice ) )
        return comedi_get_subdevice_flags( it, subdevice );

    return -1;
}

int ComediDevice::get_n_channels( unsigned int subdevice )
{
    if ( is_device_open() && is_valid_subdevice( subdevice ) )
        return comedi_get_n_channels( it, subdevice );

    return -1;
}

/*int ComediDevice::range_is_chan_specific(unsigned int subdevice)
{
        if(is_device_open() && is_valid_subdevice(subdevice) && is_valid_chan(subdevice, chan))
                return comedi_range_is_chan_specific(it, subdevice); //not in kcomedilib
        return -1;
}*/

/*int ComediDevice::maxdata_is_chan_specific(unsigned int subdevice)
{
        if(is_device_open() && is_valid_subdevice(subdevice) && is_valid_chan(subdevice, chan))
                return comedi_maxdata_is_chan_specific(it, subdevice); //not in kcomedilib
        return -1;
}*/

/***********************/
/*   channel queries   */
/***********************/

lsampl_t ComediDevice::get_maxdata( unsigned int subdevice, unsigned int chan )
{
    if ( is_device_open() && is_valid_subdevice( subdevice ) && is_valid_chan( subdevice, chan ) )
        return comedi_get_maxdata( it, subdevice, chan );

    return 0;
}

int ComediDevice::get_n_ranges( unsigned int subdevice, unsigned int chan )
{
    if ( is_device_open() && is_valid_subdevice( subdevice ) && is_valid_chan( subdevice, chan ) )
        return comedi_get_n_ranges( it, subdevice, chan );

    return -1;
}

/*comedi_range* ComediDevice::get_range(unsigned int subdevice, unsigned int chan, int range)
{
        if(is_device_open() && is_valid_subdevice(subdevice) && is_valid_chan(subdevice, chan))
                return comedi_get_range(it, subdevice, chan, range); //not in kcomedilib
        return 0;
}*/

/*int ComediDevice::find_range(unsigned int subdevice, unsigned int chan, unsigned int unit, double min, double max)
{
        if(is_device_open() && is_valid_subdevice(subdevice) && is_valid_chan(subdevice, chan))
                return comedi_find_range(it, subdevice, chan, unit, min, max); //not in kcomedilib
        return -1;
}*/

/***********************/
/*    buffer queries   */
/***********************/

/*int ComediDevice::get_buffer_size(unsigned int subdevice)
{
        if(is_device_open() && is_valid_subdevice(subdevice))
                return comedi_get_buffer_size(it, subdevice); //not in kcomedilib
        return -1;
}*/

/*int ComediDevice::get_max_buffer_size(unsigned int subdevice)
{
        if(is_device_open() && is_valid_subdevice(subdevice))
                return comedi_get_max_buffer_size(it, subdevice); //not in kcomedilib
        return -1;
}*/

/*int ComediDevice::set_buffer_size(unsigned int subdevice, unsigned int len)
{
        if(is_device_open() && is_valid_subdevice(subdevice))
                return comedi_set_buffer_size(it, subdevice, len); //not in kcomedilib
        return -1;
}*/

/************************/
/*   low-level stuff    */
/************************/

/*int ComediDevice::do_insnlist(comedi_insnlist* il)
{
        if(is_device_open())
                return comedi_do_insnlist(it, il); //nt in kcomedilib
        return -1;
}*/

int ComediDevice::do_insn( comedi_insn* insn )
{
    if ( is_device_open() )
        return comedi_do_insn( it, insn );

    return -1;
}

int ComediDevice::lock ( unsigned int subdevice )
{
    if ( is_device_open() && is_valid_subdevice( subdevice ) )
        return comedi_lock( it, subdevice );

    return -1;
}

int ComediDevice::unlock( unsigned int subdevice )
{
    if ( is_device_open() && is_valid_subdevice( subdevice ) )
        return comedi_unlock( it, subdevice );

    return -1;
}

/**************************/
/*   synchronous stuff    */
/**************************/

int ComediDevice::data_read( unsigned int subdevice, unsigned int chan, lsampl_t* data )
{
    if ( is_device_open() && is_valid_subdevice( subdevice ) && is_valid_chan( subdevice, chan ) )
        return comedi_data_read( it, subdevice, chan, get_rangetype( subdevice, chan ), get_aref( subdevice, chan ), data );

    return -1;
}

/*int ComediDevice::data_read_n(unsigned int subdevice, unsigned int chan, lsampl_t* data, unsigned int n)
{
        return comedi_data_read_n(it, subdevice, chan, get_rangetype(subdevice, chan), get_aref(subdevice, chan), data, n);
}
 
int ComediDevice::data_read_hint(unsigned int subdevice, unsigned int chan)
{
        return comedi_data_read_hint(it, subdevice, chan, get_rangetype(subdevice, chan), get_aref(subdevice, chan));
}
 
int ComediDevice::data_read_delayed(unsigned int subdevice, unsigned int chan, lsampl_t* data, unsigned int nano_sec)
{
        return comedi_data_read_delayed(it, subdevice, chan, get_rangetype(subdevice, chan), get_aref(subdevice, chan), data, nano_sec);
}*/

int ComediDevice::data_write( unsigned int subdevice, unsigned int chan, lsampl_t data )
{
    if ( is_device_open() && is_valid_subdevice( subdevice ) && is_valid_chan( subdevice, chan ) )
        return comedi_data_write( it, subdevice, chan, get_rangetype( subdevice, chan ), get_aref( subdevice, chan ), data );

    return -1;
}

int ComediDevice::dio_config( unsigned int subdevice, unsigned int chan, unsigned int dir )
{
    if ( is_device_open() && is_valid_subdevice( subdevice ) && is_valid_chan( subdevice, chan ) )
        return comedi_dio_config( it, subdevice, chan, dir );

    return -1;
}

int ComediDevice::dio_read( unsigned int subdevice, unsigned int chan, unsigned int* bit )
{
    if ( is_device_open() && is_valid_subdevice( subdevice ) && is_valid_chan( subdevice, chan ) )
        return comedi_dio_read( it, subdevice, chan, bit );

    return -1;
}

int ComediDevice::dio_write( unsigned int subdevice, unsigned int chan, unsigned int bit )
{
    if ( is_device_open() && is_valid_subdevice( subdevice ) && is_valid_chan( subdevice, chan ) )
        return comedi_dio_write( it, subdevice, chan, bit );

    return -1;
}

int ComediDevice::dio_bitfield( unsigned int subdevice, unsigned int write_mask, unsigned int* bits )
{
    if ( is_device_open() && is_valid_subdevice( subdevice ) )
        return comedi_dio_bitfield( it, subdevice, write_mask, bits );

    return -1;
}

/****************************/
/* streaming I/O (commands) */
/****************************/

/*int ComediDevice::get_cmd_src_mask(unsigned int subdevice, comedi_cmd* cmd)
{
        if(is_device_open() && is_valid_subdevice(subdevice))
                return comedi_get_cmd_src_mask(it, subdevice, cmd);
        return -1;
}*/

/*int ComediDevice::get_cmd_generic_timed(unsigned int subdevice, comedi_cmd* cmd, unsigned int ns)
{
        if(is_device_open() && is_valid_subdevice(subdevice))
                return comedi_get_cmd_generic_timed(it, subdevice, cmd, ns);
        return -1;
}*/

int ComediDevice::cancel( unsigned int subdevice )
{
    if ( is_device_open() && is_valid_subdevice( subdevice ) )
        return comedi_cancel( it, subdevice );

    return -1;
}

int ComediDevice::command( comedi_cmd* cmd )
{
    if ( is_device_open() )
        return comedi_command( it, cmd );

    return -1;
}

int ComediDevice::command_test( comedi_cmd* cmd )
{
    if ( is_device_open() )
        return comedi_command_test( it, cmd );

    return -1;
}

int ComediDevice::poll( unsigned int subdevice )
{
    if ( is_device_open() && is_valid_subdevice( subdevice ) )
        return comedi_poll( it, subdevice );

    return -1;
}

/***************************/
/*      buffer control     */
/***************************/

/*int ComediDevice::set_max_buffer_size(unsigned int subdevice, unsigned int max_size)
{
        if(is_device_open() && is_valid_subdevice(subdevice))
                return comedi_set_max_buffer_size(it, subdevice, max_size);
        return -1;
}
 
int ComediDevice::get_buffer_contents(unsigned int subdevice)
{
        if(is_device_open() && is_valid_subdevice(subdevice))
                return comedi_get_buffer_contents(it, subdevice);
        return -1; 
}
 
int ComediDevice::mark_buffer_read(unsigned int subdevice, unsigned int bytes)
{
        if(is_device_open() && is_valid_subdevice(subdevice))
                return comedi_mark_buffer_read(it, subdevice, bytes);
        return -1;
}
 
int ComediDevice::get_buffer_offset(unsigned int subdevice)
{
        if(is_device_open() && is_valid_subdevice(subdevice))
                return comedi_get_buffer_offset(it, subdevice);
        return -1;
}*/

/**************************/
/*     extra functions    */
/**************************/

int ComediDevice::set_rangetype( unsigned int subdevice, unsigned int chan, unsigned int rangetype )
{
    if ( !is_valid_subdevice( subdevice ) )
    {
        return -1;
    }

    if ( !is_valid_chan( subdevice, chan ) )
    {
        return -1;
    }

    range[ subdevice * get_max_channels() + chan ] = rangetype;
    return 0;
}

int ComediDevice::set_aref( unsigned int subdevice, unsigned int chan, unsigned int a_ref )
{
    if ( !is_valid_subdevice( subdevice ) )
    {
        return -1;
    }

    if ( !is_valid_chan( subdevice, chan ) )
    {
        return -1;
    }

    aref[ subdevice * get_max_channels() + chan ] = a_ref;
    return 0;
}

unsigned int ComediDevice::get_rangetype( unsigned int subdevice, unsigned int chan )
{
    if ( is_device_open() )
        return range[ subdevice * get_max_channels() + chan ];

    return 0;
}

unsigned int ComediDevice::get_aref( unsigned int subdevice, unsigned int chan )
{
    if ( is_device_open() )
        return aref[ subdevice * get_max_channels() + chan ];

    return 0;
}

/** DO NOT USE **
ComediSubdevice* ComediDevice::get_comedisubdevice(unsigned int subdevice)
{
        if(is_device_open() && is_valid_subdevice(subdevice))
                return new ComediSubdevice(this, subdevice);
        return 0;
}
 
ComediChannel* ComediDevice::get_comedichannel(unsigned int subdevice, unsigned int chan)
{
        if(is_device_open() && is_valid_subdevice(subdevice) && is_valid_chan(subdevice, chan))
                return new ComediChannel(this, subdevice, chan);
        return 0;
}*/


int ComediDevice::get_max_channels()
{
    if ( is_device_open() )
    {
        int n_sd = get_n_subdevices();
        int max_n_channels = 0;

        for ( unsigned int i = 0;i < ( unsigned int ) n_sd;i++ )
        {
            int ch = get_n_channels( i );

            if ( ch > max_n_channels )
            {
                max_n_channels = ch;
            }
        }

        return max_n_channels;
    }

    return 0;
}

void ComediDevice::init_range_aref()
{
    if ( is_device_open() )
    {
        int n_sd = get_n_subdevices();
        int max_n_channels = get_max_channels();

        range = new unsigned int[ n_sd * max_n_channels ];
        aref = new unsigned int[ n_sd * max_n_channels ];

        //initialising (in case someone forgets to set range and aref for a channel in a subdevice)

        for ( int cnt = 0; cnt < n_sd*max_n_channels; cnt++ )
        {
            aref[ cnt ] = 0;
            range[ cnt ] = 0;
        }
    }
}

int ComediDevice::is_valid_subdevice( unsigned int subdevice )
{
    if ( is_device_open() )
        return ( subdevice < ( unsigned int ) get_n_subdevices() );

    return 0;
}

int ComediDevice::is_valid_chan( unsigned int subdevice, unsigned int chan )
{
    if ( is_device_open() && is_valid_subdevice( subdevice ) )
        return ( chan < ( unsigned int ) get_n_channels( subdevice ) );

    return 0;
}

void ComediDevice::open_device()
{
    it = comedi_open( filename );

    if ( is_device_open() )
        init_range_aref();
}

void ComediDevice::close_device()
{
    if ( is_device_open() )
    {
        int err = comedi_close( it );

        if ( err == 0 )  //successful comedi_close
        {
            delete[] range;
            delete[] aref;
            it = 0;
        }
    }
}

int ComediDevice::is_device_open()
{
    return ( it != 0 );
}
