/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:15 CET 2005  Logger.cxx 

                        Logger.cxx -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
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
 
// to retrieve RTAI version, if any.
#define OROBLD_OS_LXRT_INTERNAL 
#include "os/StartStopManager.hpp"
#include "os/MutexLock.hpp"
#include "os/Mutex.hpp"
#include "corelib/TimeService.hpp"

#include "corelib/Logger.hpp"
#include <iomanip>

#ifdef OROSEM_PRINTF_LOGGING
#  include <stdio.h>
#else
#  include <iostream>
#  include <ostream>
#  ifdef OROSEM_FILE_LOGGING
#   include <fstream>
#  endif
#  ifdef OROSEM_REMOTE_LOGGING
#   include <sstream>
#  endif
#endif

#include <stdlib.h>
#include <pkgconf/system.h>
#ifdef OROPKG_OS_LXRT
#include <pkgconf/os_lxrt.h>
#endif

namespace ORO_CoreLib 
{
    using namespace std;

    Logger* Logger::_instance = 0;

    Logger* Logger::Instance() {
        if (_instance == 0) {
            _instance =  new Logger();
        }
        return _instance;
    }

    void Logger::Release() {
      if (_instance) {
        _instance->shutdown();
        delete _instance;
        _instance = 0;
      }
    }

#ifndef OROBLD_DISABLE_LOGGING

    Logger& Logger::log() {
        return *Instance();
    }

    /**
     * This hidden struct stores all data structures required for logging.
     */
    struct Logger::D
    {
        D() :
#ifndef OROSEM_PRINTF_LOGGING
              stdoutput(&std::cerr),
#endif
#ifdef OROSEM_REMOTE_LOGGING
              messagecnt(0),
#endif
#if defined(OROSEM_FILE_LOGGING) && !defined(OROSEM_PRINTF_LOGGING)
              logfile("orocos.log"),
#endif
              inloglevel(Info),
              outloglevel(Warning),
              timestamp(0),
              started(false), showtime(true), allowRT(false), 
              loggermodule("Logger"), moduleptr(loggermodule)
        {
#if defined(OROSEM_FILE_LOGGING) && defined(OROSEM_PRINTF_LOGGING)
            logfile = fopen("orocos.log","w");
#endif
        }

        bool maylog() const {
            if (!started || outloglevel == RealTime && allowRT == false)
                return false;
            return true;
        }

        bool maylogStdOut() const {
            if ( inloglevel <= outloglevel && outloglevel != Never && inloglevel != Never ) 
                return true;
            return false;
        }

        bool maylogFile() const {
            if ( inloglevel <= Info || inloglevel <= outloglevel )
                return true;
            return false;
        }

        /**
         * This function is called when a new message is ready to be
         * written to screen, disk, or stream. 'logline' or 'remotestream'
         * contain a single log message. Time and location is prepended.
         */
        void logit(std::ostream& (*pf)(std::ostream&))
        {
            // only on Logger::nl or Logger::endl, a time+log-line is written.
            ORO_OS::MutexLock lock( inpguard );
            std:: string res = showTime() +" " + showLevel(inloglevel) + showModule() + " ";

            // do not log if not wanted.
            if ( maylogStdOut() ) {
#ifndef OROSEM_PRINTF_LOGGING
                *stdoutput << res << logline.str() << pf;
#else
                printf("%s%s\n", res.c_str(), logline.str().c_str() );
#endif
                logline.str("");   // clear stringstream.
            }

            if ( maylogFile() ) {
#ifdef OROSEM_FILE_LOGGING
#ifndef OROSEM_PRINTF_LOGGING
                logfile << res << fileline.str() << pf;
#else
                fprintf( logfile, "%s%s\n", res.c_str(), fileline.str().c_str() );
#endif
#ifdef OROSEM_REMOTE_LOGGING
                // detect buffer 'overflow'
                if ( messagecnt >= ORONUM_LOGGING_BUFSIZE ) {
                    std::string dummy;
                    remotestream >> dummy; // FIFO principle: read 1 line
                    --messagecnt;
                }
                remotestream << res << fileline.str() << pf;
                ++messagecnt;
#endif
#if defined(OROSEM_FILE_LOGGING) || defined(OROSEM_REMOTE_LOGGING)
                fileline.str("");
#endif
#endif
            }
        }

#ifndef OROSEM_PRINTF_LOGGING
        std::ostream* stdoutput;
#endif
        std::stringstream logline;
#if defined(OROSEM_FILE_LOGGING) || defined(OROSEM_REMOTE_LOGGING)
        std::stringstream fileline;
#endif
#if defined(OROSEM_REMOTE_LOGGING)
        std::stringstream remotestream;
        unsigned int messagecnt;
#endif
#if defined(OROSEM_FILE_LOGGING)
# ifndef OROSEM_PRINTF_LOGGING
        std::ofstream logfile;
# else
        FILE* logfile;
# endif
#endif
        LogLevel inloglevel, outloglevel;

        TimeService::ticks timestamp;

        Logger::LogLevel intToLogLevel(int ll) {
            switch (ll)
                {
                case -1:
                case 0:
                    return Never;
                case 1:
                    return Fatal;
                case 2:
                    return Critical;
                case 3:
                    return Error;
                case 4:
                    return Warning;
                case 5:
                    return Info;
                case 6:
                    return Debug;
                }
            return Debug; // > 6
        }


        std::string showTime() const
        {
            std::stringstream time;
            if ( showtime )
                time <<fixed<< showpoint << setprecision(3) << TimeService::Instance()->secondsSince(timestamp);
            return time.str();
        }

        /**
         * Convert a loglevel to a string representation.
         */
        std::string showLevel( LogLevel ll) const {
            std::string prefix;
            switch (ll)
                {
                case Fatal:
                    prefix="[ FATAL  ]";
                    break;
                case Critical:
                    prefix="[CRITICAL]";
                    break;
                case Error:
                    prefix="[ ERROR  ]";
                    break;
                case Warning:
                    prefix="[ Warning]";
                    break;
                case Info:
                    prefix="[ Info   ]";
                    break;
                case Debug:
                    prefix="[ Debug  ]";
                    break;
                case RealTime:
                    prefix="[RealTime]";
                    break;
                case Never:
                    break;
                }
            return prefix;
        }



        std::string showModule() const
        {
            return "["+std::string(moduleptr)+"]";
        }

        bool started;

        bool showtime;

        bool allowRT;

        const char* loggermodule;
        const char* moduleptr;

        ORO_OS::Mutex inpguard;
        ORO_OS::Mutex startguard;
    };

    Logger::Logger()
        :d ( new Logger::D() )
    {
      this->startup();
    }

    Logger::~Logger()
    {
        delete d;
    }


    void Logger::allowRealTime() {
        *this << Logger::Warning << "Enabling Real-Time Logging !" <<Logger::endl;
        d->allowRT = true;
    }
    void Logger::disallowRealTime() {
        *this << Logger::Warning << "Disabling Real-Time Logging !" <<Logger::endl;
        d->allowRT = false;
    }

    std::ostream&
    Logger::nl(std::ostream& __os)
    {
#ifndef OROSEM_PRINTF_LOGGING
        return __os.put(__os.widen('\n'));
#else
        return __os;
#endif
    }

    std::ostream&
    Logger::endl(std::ostream& __os)
    {
#ifndef OROSEM_PRINTF_LOGGING
        return flush(__os.put(__os.widen('\n')));
#else
        return __os;
#endif
    }

    std::ostream&
    Logger::flush(std::ostream& __os)
    {
#ifndef OROSEM_PRINTF_LOGGING
        return __os.flush();
#else
        return __os;
#endif
    }


    Logger::In::In(const char* modname)
        : oldmod( Logger::log().getLogModule() )
    {
        Logger::log().in(modname);
    }

    Logger::In::~In() 
    {
        Logger::log().in( oldmod );
    }

    Logger& Logger::in(const char* modname) 
    {
        d->moduleptr = modname;
        return *this;
    }

    Logger& Logger::out()
    {
        d->moduleptr = d->loggermodule;
        return *this;
    }

    const char* Logger::getLogModule() const {
        return d->moduleptr;
    }


#define ORO_xstr(s) ORO_str(s)
#define ORO_str(s) #s        

    void Logger::startup() {
        if (d->started)
            return;
#ifndef OROBLD_DISABLE_LOGGING
        std::string xtramsg = "No ORO_LOGLEVEL environment variable set.";
        *this << Logger::Info; // default log to Info

        int wantedlevel=4; // default log level is 4.

        if ( getenv( "ORO_LOGLEVEL" ) != 0 ) {
            std::stringstream conv;
            conv.str( std::string( getenv( "ORO_LOGLEVEL" ) ) );
            conv >> wantedlevel;
            if ( conv.fail() ) {
                xtramsg = std::string( "Failed to extract loglevel from environment variable ORO_LOGLEVEL.")
                    + " It contained the string '"+conv.str()+"', while it should contain an integer value.";
                *this<<Logger::Error;
            }
            else {
                d->outloglevel = d->intToLogLevel(wantedlevel);
                xtramsg = "Successfully extracted environment variable ORO_LOGLEVEL";
            }
        }

        // Completely disable logging on negative values.
        if ( wantedlevel < 0 )
            return;
        d->started = true;
            
        d->timestamp = TimeService::Instance()->getTicks();
        *this<<xtramsg<<Logger::nl;
        *this<< " OROCOS version '" ORO_xstr(OROPKG_CORELIB) "'";
#ifdef __GNUC__
        *this << " compiled with GCC " ORO_xstr(__GNUC__) "." ORO_xstr(__GNUC_MINOR__) "." ORO_xstr(__GNUC_PATCHLEVEL__) ".";
#endif
#ifdef OROPKG_OS_LXRT
        *this<<" Running in LXRT, RTAI version '" ORO_xstr(ORO_RTAI_CONFIG_VERSION) "'."<< Logger::nl;
#endif
#ifdef OROPKG_OS_GNULINUX
        *this<<" Running in GNU/Linux."<< Logger::nl;
#endif
        *this<<"Orocos Logging Activated at level : " << d->showLevel( d->outloglevel ) << " ( "<<int(d->outloglevel)<<" ) "<< Logger::nl;
        *this<<"Reference System Time is : " << d->timestamp << " ticks ( "<<std::fixed<<Seconds(TimeService::ticks2nsecs(d->timestamp))/NSECS_IN_SECS<<" seconds )." << Logger::nl;
        *this<<"Logging is relative to this time." <<Logger::endl;
#endif
    }

    void Logger::shutdown() {
        if (!d->started)
            return;
        *this<<Logger::Info<<"Orocos Logging Deactivated." << Logger::endl;
        this->logflush();
        d->started = false;
    }

    std::string Logger::getLogLine() {
#ifdef OROSEM_REMOTE_LOGGING
        if (!d->started)
            return "";
        std::string line;
        {
            ORO_OS::MutexLock lock( d->inpguard );
            getline( d->remotestream, line );
            if ( !d->remotestream )
                d->remotestream.clear();
        }
        if ( !line.empty() )
            --d->messagecnt;
        return line;
#else
        return "";
#endif
    }

    void Logger::setStdStream( std::ostream& stdos ) {
#ifndef OROSEM_PRINTF_LOGGING
        d->stdoutput = &stdos;
#endif
    }

    Logger& Logger::operator<<( const char* t ) {
        if ( !d->maylog() )
            return *this;
        
        ORO_OS::MutexLock lock( d->inpguard );
        if ( d->maylogStdOut() )
            d->logline << t;

#if defined(OROSEM_FILE_LOGGING) || defined(OROSEM_REMOTE_LOGGING)
        // log Info or better to log file, even if not started.
        if ( d->maylogFile() )
            d->fileline << t;
#endif
        return *this;
    }

    Logger& Logger::operator<<( const std::string& t ) {
        return this->operator<<( t.c_str() );
    }

    Logger& Logger::operator<<(LogLevel ll) {
        if ( !d->maylog() )
            return *this;
        d->inloglevel = ll;
        return *this;
    }

    Logger& Logger::operator<<(std::ostream& (*pf)(std::ostream&))
    {
        if ( !d->maylog() )
            return *this;
        if ( pf == Logger::endl )
            this->logendl();
        else if ( pf == Logger::nl )
            this->lognl();
        else if ( pf == Logger::flush )
            this->logflush();
        else {
            ORO_OS::MutexLock lock( d->inpguard );
            if ( d->maylogStdOut() )
                d->logline << pf; // normal std operator in stream.
#if defined(OROSEM_FILE_LOGGING) || defined(OROSEM_REMOTE_LOGGING)
            if ( d->maylogFile() )
                d->fileline << pf;
#endif
        }
        return *this;
    }

    void Logger::logflush() {
        if (!d->maylog())
            return;
        {
            // just flush all buffers, do not produce a new logline
            ORO_OS::MutexLock lock( d->inpguard );
            if ( d->maylogStdOut() ) {
#ifndef OROSEM_PRINTF_LOGGING
                d->stdoutput->flush();
#endif
#if defined(OROSEM_REMOTE_LOGGING)
                d->remotestream.flush();
#endif
            }
#if defined(OROSEM_FILE_LOGGING)
            if ( d->maylogFile() ) {
#ifndef OROSEM_PRINTF_LOGGING
                d->logfile.flush();
#endif
            }
#endif
        }
     }

    void Logger::lognl() {
        if (!d->maylog())
            return;
        d->logit( Logger::nl );
     }

    void Logger::logendl() {
        if (!d->maylog())
            return;
        d->logit( Logger::endl );
     }

    void Logger::setLogLevel( LogLevel ll ) {
        d->outloglevel = ll;
    }

    Logger::LogLevel Logger::getLogLevel() const {
        return d->outloglevel ;
    }


#else // OROBLD_DISABLE_LOGGING

    Logger::Logger()
        : d(0)
    {
    }

#endif
}
