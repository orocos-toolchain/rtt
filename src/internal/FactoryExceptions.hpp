/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  FactoryExceptions.hpp

                        FactoryExceptions.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
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

#ifndef FACTORYEXCEPTIONS_HPP
#define FACTORYEXCEPTIONS_HPP

#include <string>
#include <exception>
#include "../rtt-config.h"

/**
 * \file FactoryExceptions.hpp This file contains some structs that can be thrown by both
 * DataSourceFactory's and CommandFactory's.
 */
namespace RTT
{ namespace internal {

    /**
     * Exception thrown when a factory is requested to
     * create an object with an unknown name.
     */
    struct RTT_EXPORT name_not_found_exception
        : public std::exception
  {
      name_not_found_exception( const std::string& n="name" );
      ~name_not_found_exception() throw();
      std::string name;
      std::string whatstr;
      virtual const char* what() const throw();
  };

    /**
     * Exception thrown when a factory is requested to
     * create an object but the wrong number of arguments was given.
     */
  struct RTT_EXPORT wrong_number_of_args_exception
      : public std::exception
  {
      int wanted;
      int received;
      std::string whatstr;
      wrong_number_of_args_exception( int w, int r );
      ~wrong_number_of_args_exception() throw();
      virtual const char* what() const throw();
  };

    /**
     * Exception thrown when a factory is requested to
     * create an object, but a wrong argument type was given.
     */
  struct RTT_EXPORT wrong_types_of_args_exception
      : public std::exception
  {
      // this contains the number of the argument that had the wrong
      // type.  The first argument has number 1.
      int whicharg;
      std::string expected_;
      std::string received_;
      std::string whatstr;
      wrong_types_of_args_exception( int w, const std::string& expected, const std::string& received );
      ~wrong_types_of_args_exception() throw();
      virtual const char* what() const throw();
  };

    /**
     * Exception thrown when a factory is requested to
     * create an object, and one of the arguments needed to
     * be an lvalue (AssignableDataSource) and an rvalue
     * (plain DataSource) was given.
     */
  struct RTT_EXPORT non_lvalue_args_exception
      : public std::exception
  {
      // thrown when a factory expects an AssignableDataSource (lvalue), but only
      // found a DataSource (rvalue).
      int whicharg;
      std::string received_;
      std::string whatstr;
      non_lvalue_args_exception( int w, const std::string& received );
      ~non_lvalue_args_exception() throw();
      virtual const char* what() const throw();
  };
}}

#endif
