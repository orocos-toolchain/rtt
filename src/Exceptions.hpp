/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  Exceptions.hpp 

                        Exceptions.hpp -  description
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
 
 
#ifndef ORO_EXCEPTIONS_HPP
#define ORO_EXCEPTIONS_HPP

#include "rtt-config.h"

/**
 * @file Exceptions.hpp
 * When Orocos is compiled without exceptions (define ORO_EMBEDDED), the functions
 * which would throw an exception simply return. For void
 * functions, use ORO_THROW, for other functions, use ORO_THROW_OR_RETURN.
 */

#ifndef ORO_EMBEDDED
#define ORO_THROW(x) throw x
#define ORO_THROW_OR_RETURN(x, rv) throw x
#else
#define ORO_THROW(x) return
#define ORO_THROW_OR_RETURN(x, rv) return rv
#endif

#endif
