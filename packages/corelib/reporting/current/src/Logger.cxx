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
 
 
#include "os/StartStopManager.hpp"
#include "os/MutexLock.hpp"
#include "corelib/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <stdlib.h>

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
          started(false), showtime(true)
    {
      this->start();
    }

    Logger* Logger::Instance() {
        if (_instance == 0) {
            _instance =  new Logger();
	    _instance->start();
        }
        return _instance;
    }

    Logger& Logger::log() {
        return *Instance();
    }

    void Logger::Release() {
      if (_instance) {
        _instance->cleanup();
        delete _instance;
        _instance = 0;
      }
    }
        
    void Logger::start() {
        if (started)
            return;
#ifndef OROBLD_DISABLE_LOGGING
        ORO_OS::MutexLock lock( startguard );
        started = true;
        std::string xtramsg = "No ORO_LOGLEVEL environment variable set.";
        *this<<Logger::Info; // default log to Info
        if ( getenv( "ORO_LOGLEVEL" ) != 0 ) {
            std::stringstream conv;
            conv.str( std::string( getenv( "ORO_LOGLEVEL" ) ) );
            int res=4; // default log level is 4.
            conv >> res;
            if ( conv.fail() ) {
                xtramsg = std::string( "Failed to extract loglevel from evironment variable ORO_LOGLEVEL.")
                    + " It contained the string '"+conv.str()+"', while it should contain a positive integer value.";
                *this<<Logger::Error;
            }
            else {
                outloglevel = intToLogLevel(res);
                xtramsg = "Successfully extracted environment variable ORO_LOGLEVEL";
            }
        }
            
        timestamp = HeartBeatGenerator::Instance()->ticksGet();
        *this<<xtramsg<<Logger::nl;
        *this<<"Orocos Logging Activated at level : " << showLevel( outloglevel ) << " ( "<<int(outloglevel)<<" ) "<< Logger::nl;
        *this<<"Reference System Time is : " << timestamp << " ticks ( "<<std::fixed<<Seconds(HeartBeatGenerator::ticks2nsecs(timestamp))/NSECS_IN_SECS<<" seconds )." << Logger::nl;
        *this<<"Logging is relative to this time." <<Logger::nl;
        stdoutput->flush();
#endif
    }

    void Logger::cleanup() {
        if (!started)
            return;
        ORO_OS::MutexLock lock( startguard );
        *this<<Logger::Info<<"Orocos Logging Deactivated." << Logger::nl;
        stdoutput->flush();
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
            time <<fixed<< showpoint << setprecision(3) << HeartBeatGenerator::Instance()->secondsSince(timestamp);
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
            case Never:
            case RealTime:
                break;
            }
        return prefix;
    }

    Logger& Logger::operator<<(LogLevel ll) {
        inloglevel = ll;
        return *this;
    }

    Logger& Logger::operator<<(std::ostream& (*pf)(std::ostream&))
    {
        if (!started)
            return *this;
        if ( pf == Logger::endl )
            this->logendl();
        else if ( pf == Logger::nl )
            this->lognl();
        else if ( pf == Logger::flush )
            this->logflush();
        else {
            ORO_OS::MutexLock lock( inpguard );
            input << pf; // normal std operator in stream.
            filedata << pf;
        }
        return *this;
    }

    void Logger::logflush() {
        if (!started)
            return;
        {
            ORO_OS::MutexLock lock( inpguard );
            if ( input.str().empty() ){
                // if nothing to log, just flush all ostreams.
                outputstream << Logger::flush;
                *stdoutput   << Logger::flush;
                logfile      << Logger::flush;
                return;
            }
        }
        
        this->logit(Logger::flush);
     }

  // private :
    void Logger::logit(std::ostream& (*pf)(std::ostream&)) {
        ORO_OS::MutexLock lock( inpguard );
        std:: string res = showTime() +" "+ showLevel(inloglevel) +" ";

        // do not log if not wanted.
        if ( inloglevel <= outloglevel && outloglevel != Never && inloglevel != Never && started == true) {
            *stdoutput << res << input.str() << pf;
            input.str("");   // clear stingstream.
        }

        if ( inloglevel <= Info || inloglevel <= outloglevel) {
            logfile << res << filedata.str() << pf;
            outputstream << res << filedata.str() << pf;
            filedata.str("");
        }
    }

    void Logger::lognl() {
        if (!started)
            return;
        this->logit( Logger::nl );
     }

    void Logger::logendl() {
        if (!started)
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
