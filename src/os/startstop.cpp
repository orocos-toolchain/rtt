/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  startstop.cxx 

                        startstop.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
/**
 * @file startstop.cxx
 *
 * This file defines the __os_init and __os_exit functions
 * which call manually crafted initialisation functions.
 * The target calls these functions at startup time.
 */

#include "../rtt-config.h"
#include <os/startstop.h>
#include "os/MainThread.hpp"
#include "os/StartStopManager.hpp"

#ifdef OROPKG_OS_THREAD_SCOPE
# include <boost/scoped_ptr.hpp>
# include "dev/DigitalOutInterface.hpp"
  using namespace RTT;
#endif

#ifdef OS_HAVE_IOSTREAM
#include <iostream>
using namespace std;
#else
#include <cstdio>
#endif
    
#include "Logger.hpp"
#include "TimeService.hpp"

using namespace RTT;
static OS::StartStopManager* initM;

extern "C"
int __os_init(int argc, char** argv )
{
#ifdef OS_HAVE_MANUAL_CRT
    DO_GLOBAL_CTORS();
#endif

    OS::MainThread::Instance();
    Logger::log() << Logger::Debug << "MainThread started." << Logger::endl;

    Logger::log() << Logger::Debug << "Starting StartStopManager." << Logger::endl;
    initM = OS::StartStopManager::Instance();
    int ret = initM->start();

#ifdef OROPKG_OS_THREAD_SCOPE
        unsigned int bit = 0;

        boost::scoped_ptr<DigitalOutInterface> pp;
        DigitalOutInterface* d = 0;
        // this is the device users can use across all threads to control the
        // scope's output.
        if ( DigitalOutInterface::nameserver.getObject("ThreadScope") )
            d = DigitalOutInterface::nameserver.getObject("ThreadScope");
        else
            Logger::log() << Logger::Error<< "Failed to find 'ThreadScope' object in DigitalOutInterface::nameserver." << Logger::endl;
        if ( d ) {
            Logger::log() << Logger::Info << "ThreadScope : main thread toggles bit "<< bit << Logger::endl;
            d->switchOn( bit );
        }
#endif
        return ret;
}

extern "C"
void __os_printFailure()
{
#ifdef OS_HAVE_IOSTREAM
                        cerr <<endl<< " Orocos has detected an uncaught C++ exception"<<endl;
                        cerr << " in the ORO_main() function."<<endl;
                        cerr << " You might have called a function which throws"<<endl;
                        cerr << " without a try {} catch {} block."<< endl << endl;
                        cerr << "To Debug this situation, issue the following command:"<<endl<<endl;
                        cerr << "   valgrind --num-callers=16 <program> [options] --nocatch" << endl;
                        cerr << "Which will show where the exception occured."<<endl;
                        cerr << " ( Change num-callers for more/less detail."<<endl;
                        cerr << "   Also, compiling orocos and your program with"<<endl;
                        cerr << "   -g adds more usefull information. )"<<endl<<endl;
#else
                        printf("Orocos intercepted uncaught C++ exception\n");
#endif

}

const char* oro_catchflag = "--nocatch";

extern "C"
int __os_checkException(int& argc, char** argv)
{
    bool dotry = true;
    // look for --nocatch flag :
    for( int i=1; i < argc; ++i)
        if ( strncmp(oro_catchflag, argv[i], strlen(oro_catchflag) ) == 0 ) {
            // if --no-catch was given last, remove it from the argc.
            if ( i == argc-1)
                --argc;
            dotry = false;
        }
    
    return dotry;
}

extern "C"
void __os_exit(void)
{
#ifdef OROPKG_OS_THREAD_SCOPE
        if (d)
            d->switchOff(bit);
#endif

    Logger::log() << Logger::Debug << "Stopping StartStopManager." << Logger::endl;
    initM->stop();
    OS::StartStopManager::Release();

    // This should be the (one but) last message to be logged :
    Logger::log() << Logger::Debug << "Stopping MainThread." << Logger::endl;

    // Stop logging
    Logger::Release();

    // Stop TimeService if present.
    TimeService::Release();

    // Stop Main Thread
    OS::MainThread::Release();

#ifdef OS_HAVE_MANUAL_CRT
    DO_GLOBAL_DTORS();
#endif
}
