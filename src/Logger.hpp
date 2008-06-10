/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:16 CET 2005  Logger.hpp 

                        Logger.hpp -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 
#ifndef ORO_CORELIB_lOGGER_HPP
#define ORO_CORELIB_lOGGER_HPP

#include "rtt-config.h"
#ifndef OROBLD_DISABLE_LOGGING
#include <ostream>
#include <sstream>
#else
#include <iosfwd>
#endif
#include <string>
#ifndef OROSEM_PRINTF_LOGGING
#include <iostream>            // for std::cerr
#endif

namespace RTT 
{
    /**
     * A simple logging class to debug/ analyse what is 
     * going on in the Orocos system. You MUST NOT use this
     * logger in a HARD realtime task or thread.
     *
     * You can disable all logging at compile time by
     * defining \a OROBLD_DISABLE_LOGGING (not advised for normal usage).
     * This class can log to a console, and/or to a file and/or to an 
     * internal buffer which may be emptied by another class. This
     * is decided upon compile time and can not be changed during runtime.
     * Both printf/iostream are supported.
     *
     * Example Usage : 
     * @verbatim
     Logger::log() << Logger::Error << "An error Occured !" << Logger::endl;
     Logger::log() << Logger::Debug << "All debug info ..." << Logger::endl;
     * @endverbatim
     *
     * When the application is started, set the displayed loglevel with
     * setLogLevel() with a LogLevel parameter. The default is Warning. Set the desired log streams
     * ( a file or std output ) with logToStream() and setStdStream(). Additionally,
     * an \a orocos.log  which is always logs
     * at log level 'Info'.
     * 
     * If you set an environment variable \a ORO_LOGLEVEL=0..6, this value will be used
     * to determine the output level until overriden by the application (if so).
     * The \a ORO_LOGLEVEL has the same effect on the 'orocos.log' file, but can not lower it below "Info".
     *
     * @warning
     * Use Logger::RealTime to log from real-time threads. As long as the output LogLevel
     * is 6 or lower, these messages will not appear and do no harm to real-time performance.
     * You need to call @verbatim Logger::log().allowRealTime(); @endverbatim once in your program
     * to confirm this choice. AGAIN: THIS WILL BREAK REAL-TIME PERFORMANCE.
     * @ingroup CoreLib
     */
    class Logger 
    {
        class D;
        D* d;
    public:
        /**
         * Enumerate all log-levels from absolute silence to
         * everything.
         * @warning If you enable 'RealTime' logging, this may break realtime performance. Use With Care and NOT
         * on production systems.
         * @see allowRealTime()
         */
        enum LogLevel { Never = 0, Fatal, Critical, Error, Warning, Info, Debug, RealTime };

        /**
         * Allow messages of the LogLevel 'RealTime' to appear on the console.
         */
        void allowRealTime();

        /**
         * Disallow messages of the LogLevel 'RealTime' to appear on the console.
         */
        void disallowRealTime();

        /**
         * Toggles the flag if the logger may log to the
         * standard output stream.
         */
        void mayLogStdOut(bool tf);

        /**
         * Toggles the flag if the logger may log to the
         * file stream.
         */
        void mayLogFile(bool tf);

        /**
         * Notify the Logger in which 'module' the message occured. This returns an object
         * whose scope (i.e. {...} ) is indicative for the boundaries of the module.
         * This is reset to the previous module name (default is 'Logger') after the in object is destroyed. Practical
         * usage must thus have the form:
         * @verbatim
         {
             Logger::In("Mymodule");
             Logger::log() << Logger::Warning << "My warning message"<<Logger::nl;
             Logger::log() << "A second message, still in MyModule"<<Logger::nl;
         }
         Logger::log() << Logger::Info << "A message in module 'Logger'..."<<Logger::endl;
         @endverbatim
        */
        struct In {
            In(const std::string& module);
            ~In();
			std::string oldmod;
        };

        /**
         * Inform the Logger of the entry of a module.
         * @see In. Use Logger::In(\a modname) for management.
         */
        Logger& in(const std::string& modname);

        /**
         * The counterpart of in().
         * @see In. Use Logger::In(\a modname) for management.
         */
        Logger& out(const std::string& modname);

        /**
         * Get the name of the current Log generating Module.
         */
		std::string getLogModule() const;

        /**
         * Function signature of the functions that influence the
         * log stream.
         */
        typedef std::ostream& (*LogFunction)(std::ostream&);

        /**
         * Insert a newline '\n' in the ostream. (Why is this not in the standard ?)
         */
        static std::ostream& nl(std::ostream& __os);

        /*
         * Insert a newline '\n' in the ostream and flush. (Copy of the standard)
         */
        static std::ostream& endl(std::ostream& __os);

        /**
         * Flush the output stream.
         */
        static std::ostream& flush(std::ostream& __os);

        /**
         * Get the singleton logger.
         * \post If the singleton did not already exist then it is created
         * and associated with the given ostream. If the singleton already
         * existed then no change occurs (and the singleton remains associated
         * with its existing ostream).
         */
        static Logger* Instance(std::ostream& str=std::cerr);

        /**
         * Delete the singleton logger.
         */
        static void Release();
        
        /**
         * As Instance(), but more userfriendly.
         */
        static Logger& log();

        /**
         * As log(), but also specify the LogLevel of the next message.
         */
        static Logger& log(LogLevel ll);

        /**
         * Print a 'welcome' string in Info  and reset log timestamp.
         */
        void startup();

        /**
         * Print a 'goodbye' string in Info, Flush all streams and stop Logging.
         */
        void shutdown();

        /**
         * This method gets all messages upto level Info
         * and can be freely read by the user, removing the line
         * from an internal buffer of Logger.
         */
        std::string getLogLine();

        /**
         * Set the standard output stream. (default is cerr).
         */
        void setStdStream( std::ostream& stdos  );
        
        /**
         * Send (user defined) data into this logger. All data with lower priority than
         * the current loglevel will be discarded. If any loglevel (thus in or out)
         * is set to \a Never, it will never be displayed. You must flush() or
         * end with std::endl to get the log's output in your file or display.
         */
        template< class T>
        Logger& operator<<( T t );

        /**
         * Set the loglevel of the incomming messages.
         */
        Logger& operator<<(LogLevel ll);

        /**
         * Log a string. This is equivalent to the templated 
         * operator<<, but reduces code size since it is compiled
         * only once.
         */
        Logger& operator<<(const std::string&);

        /**
         * Log a text message. This is equivalent to the templated 
         * operator<<, but reduces code size since it is compiled
         * only once.
         */
        Logger& operator<<(const char*);

        /**
         * Catch the std::endl and other stream manipulators.
         */
        Logger& operator<<(std::ostream& (*pf)(std::ostream&));

        /**
         * Set the loglevel of the outgoing (streamed) messages.
         * All messages with this level or higher importance will be displayed.
         * For example, setting to \a Logger::Debug will print everyting,
         * setting to \a Logger::Critical will only print critical or fatal
         * errors.
         */
        void setLogLevel( LogLevel ll );

        /**
         * Return the current output loglevel.
         */
        LogLevel getLogLevel() const;

        /**
         * Flush log buffers. May log nothing if empty.
         */
        void logflush();

        /**
         * Add endl and flush buffers. Will always log
         * at least one line.
         */
        void logendl();
        
        /**
         * Add newline without flushing buffers. 
         * If you need to log a lot of lines, this is advised with
         * a flush or endl at the end.
         */
        void lognl();

    private:
        /**
         * Returns true if the next message will be logged.
         * Returns false if the LogLevel is RealTime and 
         * allowRealTime() was not called or if the logger
         * was not started.
         */
        bool mayLog() const;

        Logger(std::ostream& str=std::cerr);
        ~Logger();

        static Logger* _instance;
    };

    /**
     * Enumerate all log-levels from absolute silence to
     * everything.
     * @warning If you enable 'RealTime' logging, this may break realtime performance. Use With Care and NOT
     * on production systems.
     * @see Logger::allowRealTime()
     */
    enum LoggerLevel { Never = 0, Fatal, Critical, Error, Warning, Info, Debug, RealTime };

    /**
     * Free function in order to access the Logger instance.
     */
    static inline Logger& log() { return Logger::log(); }

    /**
     * Free function in order to access the Logger instance and set the
     * LoggerLevel of next message.
     */
    static inline Logger& log(LoggerLevel ll) { return Logger::log(Logger::LogLevel(ll)); }

    /** 
     * Function to tell the logger that the log message ended.
     * Usage: log() << "Message" << endlog();
     */
    static inline Logger::LogFunction endlog() {return Logger::endl; }

    /** 
     * Function to tell the logger that the log message ended and 
     * specify the LoggerLevel of that message
     * Usage: log() << "Error Message" << endlog(Error);
     */
    static inline Logger::LogFunction endlog(LoggerLevel ll) { log(ll); return Logger::endl; }

    /** 
     * Function to tell the logger that a newline may be inserted in the log message.
     * Usage: log() << "Message on line 1" << nlog() << "Message on line 2" << endlog();
     */
    static inline Logger::LogFunction nlog() {return Logger::nl; }

    /** 
     * Function to tell the logger that the logs may be flushed.
     * Usage: log() << "Message on line 1" << flushlog();
     */
    static inline Logger::LogFunction flushlog() {return Logger::flush; }
}

#include "Logger.inl"

#endif
