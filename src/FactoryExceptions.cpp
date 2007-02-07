/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  FactoryExceptions.cxx 

                        FactoryExceptions.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 
#include "FactoryExceptions.hpp"
#include <sstream>

namespace RTT
{

    name_not_found_exception::name_not_found_exception( const std::string& n )
          : name(n)
    {
        whatstr = name + " not found in this factory.";
    }

    name_not_found_exception::~name_not_found_exception() throw() {}

    const char* name_not_found_exception::what() const throw() {
        return whatstr.c_str();
    }

    wrong_number_of_args_exception::wrong_number_of_args_exception( int w, int r )
          : wanted( w ), received( r )
      {
        std::stringstream a;
        a << "Wrong number of arguments: Expected ";
        a << w;
        a << ", received ";
        a << r << ".";
        whatstr = a.str();
      }

    wrong_number_of_args_exception::~wrong_number_of_args_exception() throw() {}

    const char* wrong_number_of_args_exception::what() const throw() {
        return whatstr.c_str();
    }

      wrong_types_of_args_exception::wrong_types_of_args_exception( int w, const std::string& expected, const std::string& received )
          : whicharg( w ), expected_(expected), received_(received)
      {
        std::stringstream a;
        a << "In argument "<<w<<": wrong type of argument: Expected type ";
        a << expected_;
        a << ", received type ";
        a << received_ << ".";
        whatstr = a.str();
      }

    wrong_types_of_args_exception::~wrong_types_of_args_exception() throw() {}


    const char* wrong_types_of_args_exception::what() const throw() {
        return whatstr.c_str();
    }

    non_lvalue_args_exception::non_lvalue_args_exception( int w, const std::string& received )
          : whicharg( w ), received_(received)
      {
        std::stringstream a;
        a << "In argument "<<w<<": can not assign value to non-lvalue ";
        a << received_ << ".";
        whatstr = a.str();
      }

    non_lvalue_args_exception::~non_lvalue_args_exception() throw() {}

    const char* non_lvalue_args_exception::what() const throw() {
        return whatstr.c_str();
    }


}
