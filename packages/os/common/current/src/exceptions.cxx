
#include <pkgconf/system.h>
#include <pkgconf/os.h>

#include <exception>
#include <boost/throw_exception.hpp>

#ifdef OROPKG_CORELIB_LOGGER
#include <corelib/Logger.hpp>
#endif

#ifdef ORO_EMBEDDED

namespace boost
{
    /**
     * We need to implement this function when using boost
     * without exceptions.
     */
    void throw_exception(std::exception const & e) {
#ifdef OROPKG_CORELIB_LOGGER
        Logger::log() << "throw_exception: " << e.what() << Logger::endl;
#endif
    }
}

#endif
