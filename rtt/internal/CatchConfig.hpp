/***************************************************************************
  tag: Matthias Goldhoorn  Fri Oct 18 10:00:05 CEST 2013  Config.hpp

                        CatchConfig.hpp -  description
                           -------------------
    begin                : Fri Oct 18 10:00:05 CEST 2013
    copyright            : (C) 2013 Matthias Goldhoorn
    email                : matthias@goldhoorn.eu

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

#ifndef ORO_CATCHCONFIG_HPP
#define ORO_CATCHCONFIG_HPP

#include <stdlib.h>

/**
 * @file CatchConfig.hpp
 */

/**
 * @brief Contains static global configuration variables and cached entries
 *
 * The Real-Time Toolkit is documented in <a href="../../orocos-manual.html">
 * The Orocos Real-Time Toolkit Online Manual</a>
 */


#if !defined(ORO_EMBEDDED)
# define TRY(C) if(::RTT::internal::catch_exception()){C} else try{C}
# define CATCH(T,C) catch(T){C}
# define CATCH_ALL(C) catch(...){C}
#else //Exceptions are not availible at all
# define TRY(C) C
# define CATCH(T,C)
# define CATCH_ALL(C)
#endif

namespace RTT{
    namespace internal {
        extern int m_catch_exception;
        inline bool catch_exception(){
            if(m_catch_exception == -1){
                if(getenv("RTT_IGNORE_EXCEPTION")){
                    m_catch_exception = 1;
                }else{
                    m_catch_exception = 0;
                }
            }
            return (bool)m_catch_exception;
        }
    }
}

#endif
