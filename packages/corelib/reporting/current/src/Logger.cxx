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
#include "corelib/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <pkgconf/system.h>
#ifdef OROPKG_OS_LXRT
#include <pkgconf/os_lxrt.h>
#endif

namespace ORO_CoreLib 
{
    using namespace std;

    Logger* Logger::_instance = 0;

    Logger::Logger()
        : stdoutput(&std::cerr),
          logfile("orocos.log"),
          inloglevel(Info),
          outloglevel(Warning),
          timestamp(0),
          started(false), showtime(true), allowRT(false), 
          loggermodule("Logger"), moduleptr(loggermodule)
    {
      this->startup();
    }

    Logger* Logger::Instance() {
        if (_instance == 0) {
            _instance =  new Logger();
        }
        return _instance;
    }

    Logger& Logger::log() {
        return *Instance();
    }

    void Logger::Release() {
      if (_instance) {
        _instance->shutdown();
        delete _instance;
        _instance = 0;
      }
    }

    void Logger::allowRealTime() {
        *this << Logger::Warning << "Enabling Real-Time Logging !" <<Logger::endl;
        allowRT = true;
    }
    void Logger::disallowRealTime() {
        *this << Logger::Warning << "Disabling Real-Time Logging !" <<Logger::endl;
        allowRT = false;
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
        moduleptr = modname;
        return *this;
    }

    Logger& Logger::out()
    {
        moduleptr = loggermodule;
        return *this;
    }

    const char* Logger::getLogModule() const {
        return moduleptr;
    }


#define ORO_xstr(s) ORO_str(s)
#define ORO_str(s) #s        

    void Logger::startup() {
        if (started)
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
                outloglevel = intToLogLevel(wantedlevel);
                xtramsg = "Successfully extracted environment variable ORO_LOGLEVEL";
            }
        }

        // Completely disable logging on negative values.
        if ( wantedlevel < 0 )
            return;
        started = true;
            
        timestamp = TimeService::Instance()->getTicks();
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
        *this<<"Orocos Logging Activated at level : " << showLevel( outloglevel ) << " ( "<<int(outloglevel)<<" ) "<< Logger::nl;
        *this<<"Reference System Time is : " << timestamp << " ticks ( "<<std::fixed<<Seconds(TimeService::ticks2nsecs(timestamp))/NSECS_IN_SECS<<" seconds )." << Logger::nl;
        *this<<"Logging is relative to this time." <<Logger::endl;
#endif
    }

    void Logger::shutdown() {
        if (!started)
            return;
        *this<<Logger::Info<<"Orocos Logging Deactivated." << Logger::endl;
        this->logflush();
        started = false;
    }

    std::string Logger::getLogLine() {
        if (!started)
            return "";
        std::string line;
        {
            ORO_OS::MutexLock lock( inpguard );
            getline( outputstream, line );
            if ( !outputstream )
                outputstream.clear();
        }
            //            *this<<Logger::Debug<<"Bad getLogLine." << Logger::nl;
            
        return line;
    }

    void Logger::setStdStream( std::ostream& stdos ) {
        stdoutput = &stdos;
    }

    Logger::LogLevel Logger::intToLogLevel(int ll) {
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

        
    std::string Logger::showTime() const
    {
        std::stringstream time;
        if ( showtime )
            time <<fixed<< showpoint << setprecision(3) << TimeService::Instance()->secondsSince(timestamp);
        return time.str();
    }

    std::string Logger::showLevel( LogLevel ll) const {
        std::string prefix;
        switch (ll)
            {
            case Fatal:
                prefix="[FATAL]";
                break;
            case Critical:
                prefix="[CRITICAL]";
                break;
            case Error:
                prefix="[ERROR]";
                break;
            case Warning:
                prefix="[Warning]";
                break;
            case Info:
                prefix="[Info]";
                break;
            case Debug:
                prefix="[Debug]";
                break;
            case RealTime:
                prefix="[Real-Time]";
                break;
            case Never:
                break;
            }
        return prefix;
    }

    std::string Logger::showModule() const {
        return "["+std::string(moduleptr)+"]";
    }

    Logger& Logger::operator<<( const char* t ) {
#ifndef OROBLD_DISABLE_LOGGING
        if ( !maylog() )
            return *this;
        
        ORO_OS::MutexLock lock( inpguard );
        if ( maylogStdOut() )
            input << t;

        // log Info or better to log file, even if not started.
        if ( maylogFile() )
            filedata << t;
#endif
        return *this;
    }

    Logger& Logger::operator<<( const std::string& t ) {
        return this->operator<<( t.c_str() );
    }

    Logger& Logger::operator<<(LogLevel ll) {
        if ( !maylog() )
            return *this;
        inloglevel = ll;
        return *this;
    }

    bool Logger::maylog() const {
        if (!started || outloglevel == RealTime && allowRT == false)
            return false;
        return true;
    }

    bool Logger::maylogStdOut() const {
        if ( inloglevel <= outloglevel && outloglevel != Never && inloglevel != Never ) 
            return true;
        return false;
    }

    bool Logger::maylogFile() const {
        if ( inloglevel <= Info || inloglevel <= outloglevel )
            return true;
        return false;
    }

    Logger& Logger::operator<<(std::ostream& (*pf)(std::ostream&))
    {
        if ( !maylog() )
            return *this;
        if ( pf == Logger::endl )
            this->logendl();
        else if ( pf == Logger::nl )
            this->lognl();
        else if ( pf == Logger::flush )
            this->logflush();
        else {
            ORO_OS::MutexLock lock( inpguard );
            if ( maylogStdOut() )
                input << pf; // normal std operator in stream.
            if ( maylogFile() )
                filedata << pf;
        }
        return *this;
    }

    void Logger::logflush() {
        if (!maylog())
            return;
        {
            // just flush all buffers, do not produce a new logline
            ORO_OS::MutexLock lock( inpguard );
            if ( maylogStdOut() ) {
                stdoutput->flush();
                outputstream.flush();
            }
            if ( maylogFile() )
                logfile.flush();
        }
     }

  // private :
    void Logger::logit(std::ostream& (*pf)(std::ostream&)) {
        // only on Logger::nl or Logger::endl, a time+log-line is written.
        ORO_OS::MutexLock lock( inpguard );
        std:: string res = showTime() +" " + showLevel(inloglevel) + showModule() + " ";

        // do not log if not wanted.
        if ( maylogStdOut() ) {
            *stdoutput << res << input.str() << pf;
            input.str("");   // clear stingstream.
        }

        if ( maylogFile() ) {
            logfile << res << filedata.str() << pf;
            outputstream << res << filedata.str() << pf;
            filedata.str("");
        }
    }

    void Logger::lognl() {
        if (!maylog())
            return;
        this->logit( Logger::nl );
     }

    void Logger::logendl() {
        if (!maylog())
            return;
        this->logit( Logger::endl );
     }

    void Logger::setLogLevel( LogLevel ll ) {
        outloglevel = ll;
    }

    Logger::LogLevel Logger::getLogLevel() const {
        return outloglevel ;
    }

}
