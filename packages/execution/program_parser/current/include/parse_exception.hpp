/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:23 CEST 2004  parse_exception.hpp 

                        parse_exception.hpp -  description
                           -------------------
    begin                : Thu July 15 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens at mech.kuleuven.ac.be
 
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
#ifndef PARSE_EXCEPTION_HPP
#define PARSE_EXCEPTION_HPP

#include <string>

#pragma interface

namespace ORO_Execution
{
  /**
   * @brief This is the exception class that is thrown by all of the parsers
   * to indicate a parse error.  It holds a string describing what
   * went wrong.
   */
  class parse_exception
  {
    const std::string mwhat;
  public:
    parse_exception( const std::string& what )
      : mwhat( what ) {}
    const char* what() const throw () { return mwhat.c_str(); }
  };

}

#endif
