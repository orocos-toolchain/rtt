#include "os/StartStopManager.hpp"
#include "os/MutexLock.hpp"
#include "corelib/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <stdlib.h>

namespace ORO_CoreLib 
{
    using namespace std;

    namespace
    {
        int startLogger()
        {
            Logger::Instance()->start();
            return 0;
        }

        void stopLogger()
        {
	  //Logger::log().cleanup();
	  //Logger::Release();
        }

        ORO_OS::InitFunction LogInit( &startLogger );
        ORO_OS::CleanupFunction LogCleanup( &stopLogger );
    }

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
        }
        return _instance;
    }

    Logger& Logger::log() {
        return *Instance();
    }

    void Logger::Release() {
        delete _instance;
        _instance = 0;
    }
        
    void Logger::start() {
        ORO_OS::MutexLock lock( startguard );
        if (started)
            return;
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
        *this<<Logger::nl<<xtramsg<<Logger::nl;
        *this<<"Orocos Logging Activated at level : " << showLevel( outloglevel ) << " ( "<<int(outloglevel)<<" ) "<< Logger::nl;
        *this<<"Reference System Time is : " << timestamp << " ticks ( "<<std::fixed<<Seconds(HeartBeatGenerator::ticks2nsecs(timestamp))/NSECS_IN_SECS<<" seconds )." << Logger::nl;
        *this<<"Logging is relative to this time." <<Logger::nl;
        stdoutput->flush();
    }

    void Logger::cleanup() {
        ORO_OS::MutexLock lock( startguard );
        if (!started)
            return;
        *this<<Logger::Info<<"Orocos Logging Deactivated." << Logger::nl;
        stdoutput->flush();
        started = false;
    }

    std::string Logger::getLogLine() {
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
        this->logit( Logger::nl );
     }

    void Logger::logendl() {
        this->logit( Logger::endl );
     }

    void Logger::setLogLevel( LogLevel ll ) {
        outloglevel = ll;
    }

    Logger::LogLevel Logger::getLogLevel() const {
        return outloglevel ;
    }
}
