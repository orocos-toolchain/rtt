/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  main.cxx 

                        main.cxx -  description
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

/**
 * @file main.cxx
 * This file contains the start-up code of the framework
 * and will call in turn the user's ORO_main() function.
 * Only compile the main() function if the OS calls it.
 * Otherwise, your port (ie target) must compile a file
 * which calls the user's ORO_main function.
 * (See os.cdl)
 */
#include "pkgconf/os.h"
#ifdef ORO_OS_HAVE_MAIN

#include <os/startstop.h>
#include <os/main.h>
#include <iostream>
// extern package config headers.
#include "pkgconf/system.h"
#ifdef OROPKG_CORELIB
#include "pkgconf/corelib.h"
#endif

#ifdef OROPKG_CORELIB_REPORTING
#include "corelib/Logger.hpp"
using ORO_CoreLib::Logger;
#endif

#ifdef OROPKG_DEVICE_INTERFACE
# include "pkgconf/device_interface.h"
# include <boost/scoped_ptr.hpp>
# ifdef OROPKG_OS_THREAD_SCOPE
#  include "device_interface/DigitalOutInterface.hpp"
   using namespace ORO_DeviceInterface;
#  ifdef ORODAT_DEVICE_DRIVERS_THREAD_SCOPE_INCLUDE
#   include ORODAT_DEVICE_DRIVERS_THREAD_SCOPE_INCLUDE
    using namespace ORO_DeviceDriver;
#  endif
# endif
#endif

#ifdef OROSEM_OS_LOCK_MEMORY
#include <sys/mman.h>
#endif

#ifdef OROPKG_CORBA
#include <tao/Exception.h>
#include <ace/String_Base.h>
#endif

using namespace std;

const char* catchflag = "--nocatch";

int main(int argc, char** argv)
{
    int res = -1;

    __os_init(argc, argv);

    {
        string location( argv[0] );
        location += "::main()";
        Logger::In in( location.c_str() );

    bool dotry = true;
    // look for --nocatch flag :
    for( int i=1; i < argc; ++i)
        if ( strncmp(catchflag, argv[i], strlen(catchflag) ) == 0 )
            dotry = false;

#ifdef OROPKG_OS_THREAD_SCOPE
        unsigned int bit = 0;

        boost::scoped_ptr<DigitalOutInterface> pp;
        DigitalOutInterface* d = 0;
        try {
            // this is the device users can use across all threads to control the
            // scope's output.
            if ( DigitalOutInterface::nameserver.getObject("ThreadScope") )
                d = DigitalOutInterface::nameserver.getObject("ThreadScope");
            else
# ifdef OROCLS_DEVICE_DRIVERS_THREAD_SCOPE_DRIVER
                {
                    pp.reset( new OROCLS_DEVICE_DRIVERS_THREAD_SCOPE_DRIVER("ThreadScope") );
                    d = pp.get();
                }
            
# else
                Logger::log() << Logger::Error<< "Failed to find 'ThreadScope' object in DigitalOutInterface::nameserver." << Logger::endl;
# endif
        } catch( ... )
            {
#ifdef OROPKG_CORELIB_REPORTING
                Logger::log() << Logger::Error<< "Failed to create ThreadScope." << Logger::endl;
#endif
            }
        if ( d ) {
#ifdef OROPKG_CORELIB_REPORTING
            Logger::log() << Logger::Info << "ThreadScope : main thread toggles bit "<< bit << Logger::endl;
#endif
            d->switchOn( bit );
        }
#endif
#ifdef OROPKG_CORELIB_REPORTING
    Logger::log() << Logger::Debug << "ORO_main starting..." << Logger::endl;
#endif

    // locking of all memory for this process
#ifdef OROSEM_OS_LOCK_MEMORY
        int locktype = MCL_CURRENT;
#ifdef OROSEM_OS_LOCK_MEMORY_FUTURE
        locktype |= MCL_FUTURE;
#endif
        mlockall(locktype);
#endif
        
    if ( dotry ) {
        try {
            res = ORO_main(argc, argv);
#ifdef OROPKG_CORBA
        } catch( CORBA::Exception &e )
            {
#ifdef OROPKG_CORELIB_REPORTING
                Logger::log() <<Logger::Error << "ORO_main : CORBA exception raised!" << Logger::nl;
                Logger::log() << e._info().c_str() << Logger::endl;
#endif
#endif
        } catch( ... )
            {
                cerr <<endl<< " Orocos has detected an uncaught C++ exception"<<endl;
                cerr << " in the ORO_main() function."<<endl;
                cerr << " You might have called a function which throws"<<endl;
                cerr << " without a try {} catch {} block."<< endl << endl;
                cerr << "To Debug this situation, issue the following command:"<<endl<<endl;
                cerr << "   valgrind --num-callers=16 "<<argv[0]<<" " << catchflag << endl;
                cerr << "Which will show where the exception occured."<<endl;
                cerr << " ( Change num-callers for more/less detail."<<endl;
                cerr << "   Also, compiling orocos and your program with"<<endl;
                cerr << "   -g adds more usefull information. )"<<endl<<endl;
            }
    } else {
        res = ORO_main(argc, argv);
    }
#ifdef OROPKG_OS_THREAD_SCOPE
    if (d)
        d->switchOff(bit);
#endif
#ifdef OROPKG_CORELIB_REPORTING
    Logger::log() << Logger::Debug << "ORO_main done." << Logger::endl;
#endif
    }
    __os_exit();

    return res;
}

#endif // ORO_OS_HAVE_MAIN
