/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:44 CEST 2002  ComediDevice.hpp 

                        ComediDevice.hpp -  description
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
 
 
#ifndef COMEDIDEVICE_HPP
#define COMEDIDEVICE_HPP

#ifdef __KERNEL__
extern "C"
{
#   include <linux/comedilib.h>
}

#else
#   include <comedilib.h>
#endif

//#include "ComediSubdevice.hpp"
//#include "ComediChannel.hpp"


class ComediDevice
{

    public:
        /* Create a device, given the name of the device
         * 
         * @param fn : the filename of the device (eg /dev/comedi0)
         */
        ComediDevice( const char* fn );

        /*
         * Delete this device
         */
        ~ComediDevice();

        /**********************/
        /*   device queries   */
        /**********************/

        /*
         * Returns the number of subdevices belonging to the Comedi device referenced by this device
         */
        int get_n_subdevices();

        /*
         * Returns the Comedi kernel module version code.
         * A valid Comedi device referenced by the parameter device is necessary to communicate with the kernel module.
         * On error, -1 is returned.
         * The version code is encoded as a bitfield of three 8-bit numbers.
         * For example, 0x00073d is the version code for version 0.7.61.
         */
        int get_version_code();

        /*
         * Returns a pointer to a string containing the name of the driver being used by comedi
         * for the comedi device represented by device.
         * This pointer is valid until the device is closed.
         * This function returns NULL if there is an error.
         */
        char* get_driver_name();

        /*
         * Returns a pointer to a string containing the name of the device.
         * This pointer is valid until the comedi descriptor is closed.
         * This function returns NULL if there is an error.
         */
        char* get_board_name();

        /**********************/
        /*  subdevice queries */
        /**********************/

        /*
         * Returns an integer describing the type of subdevice that belongs to this comedi device
         * and has the index subdevice.
         * The function returns -1 if there is an error.
         *
         *  @param subdevice : the number of the subdevice of which the type has to be retrieved
         */
        int get_subdevice_type( unsigned int subdevice );

        /*
         * Tries to locate a subdevice belonging to comedi device device
         * having type type, starting with the subdevice start_subdevice.
         * If it finds a subdevice with the requested type, it returns its index.
         * If it does not locate the requested subdevice, it returns -1
         * and sets the Comedilib error number to "subdevice not found"
         * If there is an error, the function returns -1 and sets the appropriate error.
         *
         *  @param type : the type of the subdevice to locate
         *  @param startsubdevice : startpoint of the search for a subdevice with type <type>
         */
        int find_subdevice_by_type( int type, unsigned int startsubdevice );

        /*
         * This function returns a bitfield describing the capabilities of the specified subdevice.
         * If there is an error, -1 is returned, and the Comedilib error value is set.
         *
         *  @param subdevice: the number of the subdevice to interrogate
         */
        int get_subdevice_flags( unsigned int subdevice );

        /*
         * The function comedi_get_n_channels() returns the number of channels of the subdevice
         * belonging to this device and having index subdevice.
         * This function returns -1 on error and the Comedilib error value is set.
         *
         *  @param subdevice : the number of the subdevice to interrogate
         */
        int get_n_channels( unsigned int subdevice );

        /*
         * If each channel of the specified subdevice has different range information, this function returns 1.
         * Otherwise, this function returns 0. On error, this function returns -1.
         *
         *  @param subdevice : the number of the subdevice to interrogate
         */ 
        //        int range_is_chan_specific(unsigned int subdevice);

        /*
         * If each channel of the specified subdevice has different maximum sample values, this function returns 1.
         * Otherwise, this function returns 0. On error, this function returns -1.
         *
         *  @param subdevice : the number of the subdevice to interrogate
         */ 
        //        int maxdata_is_chan_specific(unsigned int subdevice);

        /**********************/
        /*  channel queries   */
        /**********************/

        /*
         * Returns the maximum valid data value for channel chan of subdevice subdevice belonging to this device.
         * This function returns 0 on error.
         * 
         *  @param subdevice : the number of the subdevice to interrogate
         *  @param chan : the number of the channel in subdevice subdevice to interrogate
         */
        lsampl_t get_maxdata( unsigned int subdevice, unsigned int chan );

        /*
         * Returns the number of ranges of the channel chan belonging to the subdevice of this comedi device.
         * This function returns -1 on error.
         * 
         *  @param subdevice : the number of the subdevice to interrogate
         *  @param chan : the number of the channel in subdevice subdevice to interrogate
         */
        int get_n_ranges( unsigned int subdevice, unsigned int chan );

        /*
         * Returns a pointer to a comedi_range structure that contains information that can be used to convert
         * sample values to or from physical units.
         * The pointer is valid until this Comedi device is closed.
         * If there is an error, NULL is returned.
         *
         *  @param subdevice : the number of the subdevice to interrogate
         *  @param chan : the number of the channel in subdevice subdevice to interrogate
         *  @param range : the rangetype of the channel. It tells comedi which gain to use when measuring an analog voltage.
         */ 
        //        comedi_range* get_range(unsigned int subdevice, unsigned int chan, int range);

        /*
         * Tries to locate the optimal (smallest) range for the channel chan belonging to a subdevice of this comedi device,
         * that includes both min and max in units.
         * If a matching range is found, the index of the matching range is returned.
         * If no matching range is available, the function returns -1.
         *
         *  @param subdevice : the number of the subdevice to interrogate
         *  @param chan : the number of the channel in subdevice subdevice to interrogate
         *  @param unit : the unit in which the range is defined
         *  @param min : the minimal range value
         *  @param max : the maximal range value
         */ 
        //        int find_range(unsigned int subdevice, unsigned int chan, unsigned int unit, double min, double max);

        /**********************/
        /*   buffer queries   */
        /**********************/

        /*
         * Returns the size (in bytes) of the streaming buffer for the subdevice specified by subdevice.
         * On error, -1 is returned.
         * 
         *  @param subdevice : the number of the subdevice to interrogate
         */ 
        //        int get_buffer_size(unsigned int subdevice);

        /*
         * Returns the maximum allowable size (in bytes) of the streaming buffer for the subdevice specified subdevice.
         * On error, -1 is returned.
         *
         *  @param subdevice : the number of the subdevice to interrogate
         */ 
        //        int get_max_buffer_size(unsigned int subdevice);

        /*
         * Changes the size of the streaming buffer for the subdevice specified by subdevice.
         * The parameter size must be a multiple of the virtual memory page size.
         * The virtual memory page size can be determined using sysconf(_SC_PAGE_SIZE).
         *
         *  @param subdevice : the number of the subdevice
         *  @param len : the new buffer size
         */ 
        //        int set_buffer_size(unsigned int subdevice, unsigned int len);

        /**********************/
        /*   low-level-stuff  */
        /**********************/

        /*
         * Performs multiple Comedi instructions as part of one system call.
         * In addition, Comedi attempts to perform the instructions atomically, that is, on standard Linux kernels,
         * no process preemption should occur during the instructions.
         * However, the process may be preempted before or after the group of instructions.
         * This function can be used to avoid the overhead of multiple system calls,
         * or to ensure that multiple instructions occur without significant delay between them.
         * Preemption may occur if any of the instructions or the data arrays of any of the instructions exist in non-resident
         * or copy-on-write pages.
         * The function returns the number of sucessfully completed instructions.
         * Error information for the unsucessful instruction is not available.
         * If there is an error before the first instruction can be executed, -1 is returned.
         * 
         *  @param il : the list of instructions to be executed
         */ 
        //        int do_insnlist(comedi_insnlist* il);

        /*
         * Performs a single instruction.
         * If sucessful, the function returns the number of samples measured, which may be less than the number of requested samples.
         * Comedi limits the number of requested samples in order to enforce fairness among processes.
         * If there is an error, -1 is returned.
         * 
         *  @param insn : the instruction to perform
         */
        int do_insn( comedi_insn* insn );

        /*
         * Reserves a subdevice for use by the current process.
         * While the lock is held, no other process is allowed to read, write, or configure that subdevice,
         * although other processes can read information about the subdevice.
         * The lock is released when unlock() is called, or the device is closed.
         * If sucessful, 0 is returned. If there is an error, -1 is returned.
         *
         *  @param subdevice : the number of the subdevice to lock
         */

        int lock ( unsigned int subdevice )

            ;

        /*
         * Releases a subdevice lock acquired by lock().
         * If sucessful, 0 is returned, otherwise -1.
         *
         *  @param subdevice : the subdevice to unlock
         */
        int unlock( unsigned int subdevice );

        /**********************/
        /*  synchronous stuff */
        /**********************/

        /*
         * Read a single sample on the channel specified by the subdevice subdevice, and the channel channel.
         * For the A/D conversion (if appropriate), the device is configured to use range specification range
         * and (if appropriate) analog reference type aref.
         * Analog reference types that are not supported by the device are silently ignored.
         * The function reads one data value from the specified channel and places the data value in the location pointed to by
         * data.
         * On success, the function returns 0.
         * If there is an error, -1 is returned.
         * Data values returned by this function are unsigned integers less than or equal to the maximum sample value of the channel,
         * which can be determined using the function get_maxdata().
         *  
         * The values aref and range must be set (if appropriate) with the functions set_aref() and set_rangetype().
         * (see the function on how to use it)
         * 
         *  @param subdevice : the number of the subdevice to read on
         *  @param chan : the number of the channel of subdevice subdevice to read on
         *  @param data : struct to store the read values
         */
        int data_read( unsigned int subdevice, unsigned int chan, lsampl_t* data );

        /*
         * Read n samples on the channel specified by the subdevice subdevice, and the channel channel.
         * For the A/D conversion (if appropriate), the device is configured to use range specification range
         * and (if appropriate) analog reference type aref.
         * Analog reference types that are not supported by the device are silently ignored.
         * The function reads one data value from the specified channel and places the data value in the location pointed to by
         * data.
         * On sucess, the function returns 0.
         * If there is an error, -1 is returned.
         * Data values returned by this function are unsigned integers less than or equal to the maximum sample value of the channel,
         * which can be determined using the function get_maxdata().
         *  
         * The value aref must be set (if appropriate) with the function set_aref().
         * (see the function on how to use it)
         * 
         *  @param subdevice : the number of the subdevice to read on
         *  @param chan : the number of the channel of subdevice subdevice to read on
         *  @param data : struct to store the read values
         *  @param n : the amount of samples to be read
         */ 
        //int data_read_n(unsigned int subdevice, unsigned int chan, lsampl_t* data, unsigned int n);

        /*
         * Read 0 samples on the channel specified by the subdevice subdevice, and the channel channel.
         * For the A/D conversion (if appropriate), the device is configured to use range specification range
         * and (if appropriate) analog reference type aref.
         * Analog reference types that are not supported by the device are silently ignored.
         * The function reads one data value from the specified channel and places the data value in the location pointed to by
         * data.
         * On sucess, the function returns 0.
         * If there is an error, -1 is returned.
         *  
         * The value aref must be set (if appropriate) with the function set_aref().
         * (see the function on how to use it)
         * 
         *  @param subdevice : the number of the subdevice to read on
         *  @param chan : the number of the channel of subdevice subdevice to read on
         */ 
        //int data_read_hint(unsigned int subdevice, unsigned int chan);

        /*
         * Read a single sample on the channel specified by the subdevice subdevice, and the channel channel,
         * after a delay period nano_Sec.
         * For the A/D conversion (if appropriate), the device is configured to use range specification range
         * and (if appropriate) analog reference type aref.
         * Analog reference types that are not supported by the device are silently ignored.
         * The function reads one data value from the specified channel and places the data value in the location pointed to by
         * data.
         * On sucess, the function returns 0.
         * If there is an error, -1 is returned.
         * Data values returned by this function are unsigned integers less than or equal to the maximum sample value of the channel,
         * which can be determined using the function get_maxdata().
         *  
         * The value aref must be set (if appropriate) with the function set_aref().
         * (see the function on how to use it)
         * 
         *  @param subdevice : the number of the subdevice to read on
         *  @param chan : the number of the channel of subdevice subdevice to read on
         *  @param data : struct to store the read values
         *  @param nano_sec : the delay to read a sample
         */ 
        //int data_read_delayed(unsigned int subdevice, unsigned int chan, lsampl_t* data, unsigned int nano_sec);

        /*
         * Writes a single sample on the channel that is specified by the subdevice subdevice, and the channel channel.
         * If appropriate, the device is configured to use range specification range and analog reference type aref.
         * Analog reference types that are not supported by the device are silently ignored.
         * The function writes the data value specified by the parameter data to the specified channel.
         * On sucess, data_write() returns 0.
         * If there is an error, -1 is returned.
         *
         * The values aref and range must be set (if appropriate) with the functions set_aref() and set_rangetype().
         * (see the function on how to use it)
         * 
         *  @param subdevice : the subdevice to write on
         *  @param chan : the channel of subdevice subdevice to write on
         *  @param data : the data sample to write
         */
        int data_write( unsigned int subdevice, unsigned int chan, lsampl_t data );

        /*
         * Configures individual channels in a digital I/O subdevice to be either input or output,
         * depending on the value of parameter direction.
         * Valid directions are COMEDI_INPUT or COMEDI_OUTPUT.
         * Depending on the capabilities of the hardware device, multiple channels may be grouped together to determine direction.
         * In this case, a single call to dio_config() for any channel in the group will affect the entire group.
         * If sucessful, 0 is returned, otherwise -1.
         * 
         *  @param subdevice : the subdevice to configure
         *  @param chan : the channel of subdevice subdevice to configure
         *  @param dir : the new direction type of the channel
         */
        int dio_config( unsigned int subdevice, unsigned int chan, unsigned int dir );

        /*
         * Reads the channel channel belonging to the subdevice subdevice.
         * The data value that is read is stored in the location pointed to by bit.
         * This function is equivalent to data_read(device,subdevice,channel,0,0,bit).
         * This function does not require a digital subdevice or a subdevice with a maximum data value of 1 to work properly.
         * On sucess, the function returns 0.
         * If there is an error, -1 is returned.
         * Data values returned by this function are unsigned integers less than or equal to the maximum sample value of the channel,
         * which can be determined using the function get_maxdata().
         * 
         *  @param subdevice : the subdevice to read on
         *  @param chan : the channel of subdevice subdevice to read on
         *  @param bit : the location to store the read data
         */
        int dio_read( unsigned int subdevice, unsigned int chan, unsigned int* bit );

        /*
         * Writes the value bit to the channel channel belonging to the subdevice subdevic. 
         * This function is equivalent to data_write(device,subdevice,channel,0,0,bit).
         * This function does not require a digital subdevice or a subdevice
         * with a maximum data value of 1 to work properly.
         * On sucess, the function returns 0.
         * If there is an error, -1 is returned.
         * 
         *  @param subdevice : the subdevice to write to
         *  @param chan : the channel of subdevice subdevice to write to
         *  @param bit : the data to write to the channel
         */
        int dio_write( unsigned int subdevice, unsigned int chan, unsigned int bit );

        /*
         * Allows multiple channels to be read simultaneously from a digital input or digital I/O device.
         * The parameter write_mask and the value pointed to by bits are interpreted as bit fields,
         * with the least significant bit representing channel 0.
         * For each bit in write_mask that is set to 1, the corresponding bit in *bits is written to the digital output channel.
         * After writing all the output channels, each channel is read, and the result placed in the approprate bits in *bits.
         * The result of reading an output channel is undefined.
         * It is not possible to access channels greater than 31 using this function.
         *
         *  @param subdevice : the subdevice to read on
         *  @param write_mask : mask to select the channels to read
         *  @param bits : the location to store the read data
         */
        int dio_bitfield( unsigned int subdevice, unsigned int write_mask, unsigned int* bits );

        /****************************/
        /* streaming I/O (commands) */
        /****************************/

        /*
         * The command capabilities of the subdevice indicated by the parameter subdevice are probed,
         * and the results placed in the command structure pointed to by the parameter command.
         * The trigger source elements of the command structure are set to the logical OR value of possible trigger sources.
         * Other elements in the structure are undefined.
         * If sucessful, 0 is returned, otherwise -1.
         *
         *  @param subdevice : the subdevice to interrogate
         *  @param cmd : the struct to store the command capabilities
         */ 
        //        int get_cmd_src_mask(unsigned int subdevice, comedi_cmd* cmd);

        /*
         * The command capabilities of the subdevice indicated by the parameter subdevice are probed,
         * and the results placed in the command structure pointed to by the parameter command.
         * The command structure pointed to by the parameter command is modified to be a valid command
         * that can be used as a parameter to command().
         * The command measures samples at a rate that corresponds to the period period_ns.
         * The rate is adjusted to a rate that the device can handle.
         * If sucessful, 0 is returned, otherwise -1.
         * 
         *  @param subdevice : the subdevice to interrogate
         *  @param cmd : the struct to store the command capabilities
         *  @param ns : rate of measurement of the samples
         */ 
        //        int get_cmd_generic_timed(unsigned int subdevice, comedi_cmd* cmd, unsigned int ns);

        /*
         * Stop a Comedi command previously started by command() that is still in progress on the subdevice subdevice.
         * This may not return the subdevice to a ready state, since there may be samples in the buffer that need to be read.
         * If sucessful, 0 is returned, otherwise -1.
         * 
         *  @param subdevice : the subdevice which runs the command that has to be canceled
         */
        int cancel( unsigned int subdevice );

        /*
         * Start streaming input or output.
         * The command structure pointed to by the parameter command specifies the acquisition.
         * For input subdevices, sample values are read using the function read().
         * For output subdevices, sample vlaues are written using the function write().
         * If sucessful, 0 is returned, otherwise -1.
         * 
         *  @param cmd : acquisition specification
         */
        int command( comedi_cmd* cmd );

        /*
         * Test the command structure pointed to by the parameter command and returns an integer
         * describing the testing stages that were sucessfully passed.
         * In addition, if elements of the command structure are invalid, they may be modified.
         * Source elements are modified to remove invalid source triggers.
         * Argument elements are adjusted or rounded to the nearest valid value.
         *
         *  @param cmd : acquisition specification
         */
        int command_test( comedi_cmd* cmd );

        /*
         * Used on a subdevice that has a Comedi command in progress in order to update the streaming buffer.
         * If supported by the driver, all available samples are copied to the streaming buffer.
         * These samples may be pending in DMA buffers or device FIFOs.
         * If sucessful, the number of additional bytes available is returned.
         * If there is an error, -1 is returned.
         *
         *  @param subdevice : the subdevice to poll
         */
        int poll( unsigned int subdevice );

        /**********************/
        /*   buffer control   */
        /**********************/

        /*
         * Change the maximum allowable size (in bytes) of the streaming buffer for the subdevice subdevice.
         * Changing the maximum buffer size requires appropriate privileges.
         * If sucessful, the old buffer size is returned.
         * On error, -1 is returned.
         * 
         *  @param subdevice : the subdevice that gets a new maximum buffer size
         *  @param max_size : the new buffer size for the subdevice
         */ 
        //        int set_max_buffer_size(unsigned int subdevice, unsigned int max_size);

        /*
         * Used on a subdevice that has a Comedi command in progress.
         * The number of bytes that are available in the streaming buffer is returned.
         * If there is an error, -1 is returned.
         *
         *  @param subdevice : subdevice to read the buffer from
         */ 
        //        int get_buffer_contents(unsigned int subdevice);

        /*
         * Used on a subdevice that has a Comedi command in progress.
         * This function can be used to indicate that the next num_bytes bytes in the buffer are no longer needed and may be discarded.
         * If there is an error, -1 is returned.
         *
         *  @param subdevice : the subdevice to address
         *  @param bytes : the amount of bytes in the buffer that are no longer needed
         */ 
        //        int mark_buffer_read(unsigned int subdevice, unsigned int bytes);

        /*
         * Used on a subdevice that has a Comedi command in progress.
         * This function returns the offset of the read pointer in the streaming buffer.
         * This offset is only useful for memory mapped buffers.
         * If there is an error, -1 is returned.
         * 
         *  @param subdevice : the number of the subdevice to interrogate
         */ 
        //        int get_buffer_offset(unsigned int subdevice);

        /**********************/
        /*  extra functions   */
        /**********************/

        /*
         * Set the analog reference of a channel.
         * Possible values are: (see comedi.h)
         *   AREF_GROUND
         *   AREF_COMMON
         *   AREF_DIFF
         *   AREF_OTHER
         * if there is an error, -1 is returned.
         * 
         *  @param subdevice : the number of the subdevice to query
         *  @param chan : the channel of subdevice subdevice to assign the reference aref to
         *  @param aref : the new analog reference of the channel
         */
        int set_aref( unsigned int subdevice, unsigned int chan, unsigned int aref );

        /*
         * Set the range of a channel.
         * if there is an error, -1 is returned.
         * 
         *  @param subdevice : the number of the subdevice to query
         *  @param chan : the channel of subdevice subdevice to assign the reference aref to
         *  @param range : the range of the channel
         */
        int set_rangetype( unsigned int subdevice, unsigned int chan, unsigned int range );


        /*
         * return a specific subdevice of this device.
         *
         *  @param subdevice: the number of the subdevice to be returned
         */ 
        //ComediSubdevice* get_comedisubdevice(unsigned int subdevice);

        /*
         * return a specific channel of the subdevice subdevice of this device.
         *
         *  @param subdevice : the number of the subdevice to be addressed
         *  @param chan : the number of the channel to be returned
         */ 
        //ComediChannel* get_comedichannel(unsigned int subdevice, unsigned int chan);

        /*
         * get the rangetype of a channel of subdevice subdevice.
         *
         *  @param subdevice : the subdevice to query
         *  @param chan : the channel of subdevice subdevice to query
         */
        unsigned int get_rangetype( unsigned int subdevice, unsigned int chan );

        /*
         * get the analog reference of a channel of subdevice subdevice.
         *
         *  @param subdevice : the subdevice to query
         *  @param chan : the channel of subdevice subdevice to query
         */
        unsigned int get_aref( unsigned int subdevice, unsigned int chan );

        /*
         * open a comedidevice referenced by the local variable <filename>
         */
        void open_device();

        /*
         * close a comedidevice referenced by the local variable <filename>
         */
        void close_device();

        /*
         * check is the ComediDevice contains an open comedi device.
         * returns 1 when true, else 0;
         */
        int is_device_open();

    protected:

        /*
         * create and initialize the range and analog reference structure.
         * (internal use by ComediDevice())
         */
        void init_range_aref();

        /*
         * check if a subdevicenumber is valid, present.
         * returns -1 if not valid.
         * 
         *  @param subdevice : the number of a subdevice to check
         */
        int is_valid_subdevice( unsigned int subdevice );

        /*
         * check if a channel of a subdevice is valid, present.
         * returns -1 if not valid.
         * The parameter subdevice must be a valid subdevicenumber.
         * 
         *  @param subdevice : the subdevice to query
         *  @param chan : the number of the channel to check
         */
        int is_valid_chan( unsigned int subdevice, unsigned int chan );

        /*
         * retrieve the maximum amount of channels in all subdevices of this comedi device.
         */
        int get_max_channels();

        /*
         * Structures to store the range type and analog reference of the channels in this device.
         * These values can be set by set_range_type() and set_aref().
         *
         * Note: range is stored in the subdevice_struct in the variable range_type, but to get this variable,
         * a DEPRECATED function should be used.
         * So now this value can be stored in the following variable.
         */
        unsigned int* range;

        unsigned int* aref;

        /*
         * struct containing information about the comedi device.
         */
        comedi_t* it;

        /*
         * the filename of the comedi device (eg "/dev/comedi0").
         */
        const char* filename;
};

#endif
