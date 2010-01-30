/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  Attribute.cxx

                        Attribute.cxx -  description
                           -------------------
    begin                : Tue December 21 2004
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


#include "Attribute.hpp"
#include "rtt-fwd.hpp"

namespace RTT
{
    using namespace detail;

  AttributeBase::AttributeBase()
  {
  }

  AttributeBase::AttributeBase(const std::string& name)
      : mname(name)
  {
  }

  AttributeBase::~AttributeBase()
  {
  }

  const std::string& AttributeBase::getName() const
  {
      return mname;
  }

  void AttributeBase::setName(const std::string& n)
  {
      mname = n;
  }

  Alias::Alias(const std::string& name, DataSourceBase::shared_ptr d )
      : base::AttributeBase(name),
        data( d )
  {
  }

  base::DataSourceBase::shared_ptr Alias::getDataSource() const
  {
      return data;
  }

  Alias* Alias::clone() const
  {
      return new Alias( mname, data.get() );
  }
  Alias* Alias::copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replacements, bool )
  {
      // instantiate does not apply.
      return new Alias( mname, data->copy( replacements ) );
  }


}
