/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  FactoryExceptions.hpp 

                        FactoryExceptions.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
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
 
#ifndef FACTORYEXCEPTIONS_HPP
#define FACTORYEXCEPTIONS_HPP

#include <string>

/**
 * \file This files contains some structs that can be thrown by both
 * DataSourceFactory's and CommandFactory's.
 */
namespace ORO_Execution
{
  struct name_not_found_exception
  {
  };

  struct wrong_number_of_args_exception
  {
    int wanted;
    int received;
    wrong_number_of_args_exception( int w, int r )
      : wanted( w ), received( r )
      {
      };
  };

  struct wrong_types_of_args_exception
  {
      // this contains the number of the argument that had the wrong
      // type.  The first argument has number 1.
      int whicharg;
      std::string expected_;
      std::string received_;
      wrong_types_of_args_exception( int w, const std::string& expected, const std::string& received )
          : whicharg( w ), expected_(expected), received_(received)
      {
      };
  };
}

#endif
