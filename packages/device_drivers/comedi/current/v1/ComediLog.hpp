/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:44 CEST 2002  ComediLog.hpp 

                        ComediLog.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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
 
#ifndef COMEDILOG_HPP
#define COMEDILOG_HPP

#ifdef __KERNEL__
extern "C"
{
#   include <linux/comedilib.h>
}

#else
#   include <comedilib.h>
#endif

class ComediLog
{

    public:
        /*
         * This function affects the output of debugging and error messages from Comedilib.
         * By increasing the loglevel, additional debugging information will be printed.
         * Error and debugging messages are printed to the stream stderr.
         * The default loglevel can be set by using the environment variable COMEDI_LOGLEVEL. The default loglevel is 1. 
         * In order to conserve resources, some debugging information is disabled by default when Comedilib is compiled.
         * The meaning of the loglevels is as follows:
         * COMEDI_LOGLEVEL=0 Comedilib prints nothing.
         * COMEDI_LOGLEVEL=1 (default) Comedilib prints error messages when there is a self-consistency error (i.e., an internal bug.)
         * COMEDI_LOGLEVEL=2 Comedilib prints an error message when an invalid parameter is passed.
         * COMEDI_LOGLEVEL=3 Comedilib prints an error message whenever an error is generated in the Comedilib library
         * or in the C library, when called by Comedilib.
         * COMEDI_LOGLEVEL=4 Comedilib prints a lot of junk. 
         * This function returns the previous loglevel.
         *
         *  @param loglevel : the new loglevel
         */
        int loglevel( int loglevel );

        /*
         * When a Comedilib function fails, it usually returns -1 or NULL, depending on the return type.
         * An internal library variable stores an error number, which can be retrieved with errno().
         * This error number can be converted to a human-readable form by the functions perror() and strerror().
         * These functions are intended to mimic the behavior of the standard C library functions perror(), strerror(), and errno.
         * In particular, Comedilib functions sometimes return an error that is generated inside the C library;
         * the comedi error message in this case is the same as the C library.
         * The function perror() prints an error message to stderr.
         * The error message consists of the argument string, a colon, a space, a description of the error condition, and a new line.
         *    
         *  @param s :
         */
        void perror( const char* s );

        /*
         * When a Comedilib function fails, it usually returns -1 or NULL, depending on the return type.
         * An internal library variable stores an error number, which can be retrieved with errno().
         * This error number can be converted to a human-readable form by the functions perror() and strerror().
         * These functions are intended to mimic the behavior of the standard C library functions perror(), strerror(), and errno.
         * In particular, Comedilib functions sometimes return an error that is generated inside the C library;
         * the comedi error message in this case is the same as the C library.
         * The function strerror() returns a pointer to a character string describing the Comedilib error errnum.
         * The persistence of the returned pointer is undefined, and should not be trusted after the next Comedilib call.
         * An unrecognized error number will return a pointer to the string "undefined error", or similar.
         *
         *  @param errnum : the error ID to convert to a string
         */
        char* strerror( int errnum );

        /*
         * When a Comedilib function fails, it usually returns -1 or NULL, depending on the return type.
         * An internal library variable stores an error number, which can be retrieved with errno().
         * This error number can be converted to a human-readable form by the functions perror() and strerror().
         * These functions are intended to mimic the behavior of the standard C library functions perror(), strerror(), and errno.
         * In particular, Comedilib functions sometimes return an error that is generated inside the C library;
         * the comedi error message in this case is the same as the C library.
         * The function errno() returns an integer describing the most recent comedilib error.
         * This integer may be used as the errnum parameter for strerror().
         * Note that errno() is deliberately different than the variable errno.
         * This is to overcome difficulties in making errno thread-safe.
         */ 
        //        int errno();

        /*
         * The function fileno returns the integer descriptor for the device dev.
         * This descriptor can then be used as the file descriptor parameter of read(), write(), etc.
         * This function is intended to mimic the standard C library function fileno().
         * If dev is an invalid comedi_t pointer, the function returns -1 and sets the appropriate Comedilib error value.
         *
         *  @param it : pointer to a ComediDevice
         */
        int fileno( comedi_t* it );
};

#endif
