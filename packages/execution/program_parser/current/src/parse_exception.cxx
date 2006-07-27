/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:06 CEST 2004  parse_exception.cxx

                        parse_exception.cxx -  description
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

#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "rtt/parse_exception.hpp"

#include <sstream>

namespace RTT
{
    using namespace detail;
  // implemented here to avoid having to include sstream in the header
  const std::string parse_exception_wrong_number_of_arguments::what() const
  {
    std::ostringstream stream;
    stream << "Wrong number of arguments in call of function \""
           << mcomponentname << "." << mmethodname
           << "\": expected " << mexpectednumber
           << ", received " << mreceivednumber << ".";
    return stream.str();
  }

  // implemented here to avoid having to include sstream in the header
  const std::string parse_exception_wrong_type_of_argument::what() const
  {
    std::ostringstream stream;
    stream << "Wrong type of argument provided for argument number "
           << margnumber << " in call of function \""
           << mcomponentname << "." << mmethodname
           << "\". Expected type "<<mexpected<<", got type "<< mreceived <<".";
    return stream.str();
  }

  file_parse_exception::~file_parse_exception()
  {
    delete mpe;
  }

  file_parse_exception::file_parse_exception( const file_parse_exception& rhs )
    : mpe( rhs.mpe->copy() ), mfile( rhs.mfile ), mline( rhs.mline ),
      mcolumn( rhs.mcolumn )
  {
  }

  const std::string file_parse_exception::what() const
  {
    std::ostringstream stream;
    stream << "Parse error at line "
           << mline << ": " << mpe->what();
    return stream.str();
  }

    parse_exception_no_such_constructor::parse_exception_no_such_constructor(const std::string& tname, 
                                                                             std::vector<DataSourceBase::shared_ptr> args)
        : margsig( "No such constructor found: " + tname + "(")
    {
        
        for(std::vector<DataSourceBase::shared_ptr>::iterator it = args.begin();
            it != args.end();
            ) {
            margsig += (*it)->getType();
            if ( ++it != args.end() )
                margsig += ", ";
        }
        margsig += ")";
    }
}
