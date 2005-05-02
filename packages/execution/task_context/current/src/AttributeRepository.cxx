/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  AttributeRepository.cxx 

                        AttributeRepository.cxx -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 

#include "execution/AttributeRepository.hpp"
#include "execution/TaskAttribute.hpp"
#include "execution/DataSource.hpp"
#include "execution/mystd.hpp"

namespace ORO_Execution
{

  AttributeRepository::AttributeRepository()
  {
  }

  AttributeRepository::~AttributeRepository()
  {
      // we do not claim automatically ownership
      // call clear() manually to delete all contents.
  }

    AttributeRepository* AttributeRepository::copy( std::map<const DataSourceBase*, DataSourceBase*>& repl ) const
    {
        AttributeRepository* ar = new AttributeRepository();
        for ( map_t::const_iterator i = values.begin(); i != values.end(); ++i )
            ar->setValue(i->first, i->second->copy( repl ) );
        return ar;
    }


  void AttributeRepository::clear()
  {
    for ( map_t::iterator i = values.begin(); i != values.end(); ++i )
      delete i->second;
    values.clear();
  }

  bool AttributeRepository::setValue( const std::string& name,
                                      TaskAttributeBase* value )
  {
    std::auto_ptr<TaskAttributeBase> v( value );
    map_t::iterator i = values.find( name );
    if ( i != values.end() )
        return false;
    values[name] = v.release();
    return true;
  }

  void AttributeRepository::removeValue( const std::string& name )
  {
    map_t::iterator i = values.find( name );
    if ( i != values.end() )
      delete i->second;
    values.erase( name );
  }

  TaskAttributeBase* AttributeRepository::getValue( const std::string& name )
  {
    map_t::iterator i = values.find( name );
    if ( i == values.end() ) return 0;
    else return i->second;
  }

  bool AttributeRepository::isDefined( const std::string& name ) const
  {
    return values.find( name ) != values.end();
  }

    std::vector<std::string> AttributeRepository::attributes() const
    {
        return mystd::keys( values );
    }
}
