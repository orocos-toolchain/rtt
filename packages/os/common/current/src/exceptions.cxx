/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  exceptions.cxx 

                        exceptions.cxx -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 

#include <pkgconf/system.h>
#include <pkgconf/os.h>

#include <exception>
#include <boost/throw_exception.hpp>

#ifdef OROPKG_CORELIB_LOGGER
#include <rtt/Logger.hpp>
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
