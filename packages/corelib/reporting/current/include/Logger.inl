#ifndef ORO_CORELIB_LOGGER_INL
#define ORO_CORELIB_LOGGER_INL

/**
 * @file Logger.inl 
 * Provides empty inlines when no logging is used, which the
 * compiler can optimize out.
 */



namespace ORO_CoreLib
{

    template< class T>
    Logger& Logger::operator<<( T t ) {
#ifndef OROBLD_DISABLE_LOGGING
        if ( !mayLog() )
            return *this;
        std::stringstream tmp;
        tmp << t;
        return this->operator<<( tmp.str() );
#endif
        return *this;
    }

#ifdef OROBLD_DISABLE_LOGGING

    // instance will be actually null
    inline Logger& Logger::log() {
        return *_instance;
    }

    inline Logger& Logger::log(LogLevel) {
        return *_instance;
    }

    inline bool Logger::mayLog() const {
        return false;
    }

    inline void Logger::allowRealTime() {
    }

    inline void Logger::disallowRealTime() {
    }

    inline std::ostream&
    Logger::nl(std::ostream& __os)
    {
        return __os;
    }

    inline std::ostream&
    Logger::endl(std::ostream& __os)
    {
        return __os;
    }

    inline std::ostream&
    Logger::flush(std::ostream& __os)
    {
        return __os;
    }


    inline Logger::In::In(const char*)
    {
    }

    inline Logger::In::~In() 
    {
    }

    inline Logger& Logger::in(const char*) 
    {
        return *this;
    }

    inline Logger& Logger::out()
    {
        return *this;
    }

    inline const char* Logger::getLogModule() const {
        return "";
    }

    inline void Logger::startup() {
    }

    inline void Logger::shutdown() {
    }

    inline std::string Logger::getLogLine() {
        return "";
    }

    inline void Logger::setStdStream( std::ostream& ) {
    }

    inline Logger& Logger::operator<<( const char* ) {
        return *this;
    }

    inline Logger& Logger::operator<<( const std::string& ) {
        return *this;
    }

    inline Logger& Logger::operator<<(LogLevel) {
        return *this;
    }

    inline Logger& Logger::operator<<(std::ostream& (*pf)(std::ostream&))
    {
        return *this;
    }

    inline void Logger::logflush() {
     }

    inline void Logger::lognl() {
     }

    inline void Logger::logendl() {
     }

    inline void Logger::setLogLevel( LogLevel ) {
    }

    inline Logger::LogLevel Logger::getLogLevel() const {
        return Never;
    }
#endif

}

#endif
