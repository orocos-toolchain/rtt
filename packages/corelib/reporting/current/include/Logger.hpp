#ifndef ORO_CORELIB_lOGGER_HPP
#define ORO_CORELIB_lOGGER_HPP

#include <ostream>
#include <fstream>
#include <sstream>
#include "HeartBeatGenerator.hpp"
#include "os/Mutex.hpp"

namespace ORO_CoreLib 
{
    /**
     * A simple logging class to debug/ analyse what is 
     * going on in the Orocos system. You MUST NOT use this
     * logger in a realtime task or thread.
     *
     * You can disable all logging at compile time by
     * defining \a OROBLD_DISABLE_LOGGING (not advised for normal usage).
     *
     * Example Usage : 
     * @verbatim
     Logger::log() << Logger::Error << "An error Occured !" << Logger::endl;
     Logger::log() << Logger::Debug << "All debug info ..." << Logger::endl;
     * @endverbatim
     *
     * When the application is started, set the displayed loglevel with
     * setLogLevel() with a LogLevel parameter. The default is Info. Set the desired log streams
     * ( a file or std output ) with logToStream() and setStdStream(). Additionally,
     * an \a orocos.log  which is always logs
     * at log level 'Info'.
     * 
     * If you set an environment variable \a ORO_LOGLEVEL=0..6, this value will be used
     * to determine the output level until overriden by the application (if so).
     * The \a ORO_LOGLEVEL has no effect on the 'orocos.log' file.
     */
    class Logger 
    {
    public:
        /**
         * Enumerate all log-levels from absolute silence to
         * everything.
         * @warning If you enable 'RealTime' logging, this may break realtime performance. Use With Care and NOT
         * on production systems.
         */
        enum LogLevel { Never = 0, Fatal, Critical, Error, Warning, Info, Debug, RealTime };

        /**
         * Insert a newline '\n' in the ostream. (Why is this not in the standard ?)
         */
        static std::ostream&
        nl(std::ostream& __os)
        { return __os.put(__os.widen('\n')); }

        /*
         * Insert a newline '\n' in the ostream and flush. (Copy of the standard)
         */
        static std::ostream&
        endl(std::ostream& __os)
        { return flush(__os.put(__os.widen('\n'))); }

        static std::ostream&
        flush(std::ostream& __os)
        { return __os.flush(); }

        /**
         * Get the singleton logger.
         */
        static Logger* Instance();

        /**
         * Delete the singleton logger.
         */
        static void Release();
        
        /**
         * As Instance(), but more userfriendly.
         */
        static Logger& log();

        /**
         * Print a 'welcome' string in Info  and reset log timestamp.
         */
        void start();

        /**
         * Print a 'goodbye' string in Info and Flush all streams.
         */
        void cleanup();

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
         * Send data into this logger. All data with lower priority than
         * the current loglevel will be discarded. If any loglevel (thus in or out)
         * is set to \a Never, it will never be displayed. You must flush() or
         * end with std::endl to get the log's output in your file or display.
         */
        template< class T>
        Logger& operator<<( T t ) {
#ifndef OROBLD_DISABLE_LOGGING
            if ( inloglevel <= outloglevel && outloglevel != Never && inloglevel != Never && started == true) {
                input << t;
            }
            // log Info or better to log file, even if not started.
            if ( inloglevel <= Info )
                filedata << t;
#endif
            return *this;
        }

        /**
         * Set the loglevel of the incomming messages.
         */
        Logger& operator<<(LogLevel ll);

        /**
         * Catch the std::endl and other stream manipulators.
         */
        Logger& operator<<(std::ostream& (*pf)(std::ostream&));

        /**
         * Set the loglevel of the outgoing (streamed) messages.
         * All messages with this level or lower will be displayed.
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
        void logit(std::ostream& (*pf)(std::ostream&));

        std::stringstream input;
        std::stringstream filedata;
        std::stringstream outputstream;
        std::ostream* stdoutput;
        std::ofstream logfile;
        LogLevel inloglevel, outloglevel;

        HeartBeatGenerator::ticks timestamp;

        Logger();

        static Logger* _instance;

        LogLevel intToLogLevel(int ll);

        std::string showTime() const;

        /**
         * Convert a loglevel to a string representation.
         */
        std::string showLevel( LogLevel ll) const;

        bool started;

        bool showtime;

        ORO_OS::Mutex inpguard;
        ORO_OS::Mutex startguard;
    };
}
#endif
