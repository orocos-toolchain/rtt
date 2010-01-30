/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  AttributeRepository.cxx

                        AttributeRepository.cxx -  description
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



#include "AttributeRepository.hpp"
#include "../internal/mystd.hpp"
#include <functional>
#include <boost/bind.hpp>

namespace RTT {
    using namespace detail;
    using namespace std;
    using namespace boost;

  AttributeRepository::AttributeRepository()
      :bag(0)
  {
  }

  AttributeRepository::~AttributeRepository()
  {
      delete bag;
      // we do not claim automatically ownership
      // call clear() manually to delete all contents.
  }

    AttributeRepository* AttributeRepository::copy( std::map<const DataSourceBase*, DataSourceBase*>& repl, bool inst ) const
    {
        AttributeRepository* ar = new AttributeRepository();
        for ( map_t::const_iterator i = values.begin(); i != values.end(); ++i ) {
            ar->setValue((*i)->copy( repl, inst ) );
        }
        return ar;
    }


  void AttributeRepository::clear()
  {
    for ( map_t::iterator i = values.begin(); i != values.end(); ++i )
      delete *i;
    values.clear();
    delete bag;
    bag = 0;
  }

  bool AttributeRepository::setValue( AttributeBase* value )
  {
    map_t::iterator i = find( values.begin(), values.end(), value );
    if ( i != values.end() )
        return false;
    values.push_back( value );
    return true;
  }

    bool AttributeRepository::addProperty( PropertyBase* pb ) {
        if ( bag && bag->find( pb->getName() ) )
            return false;
        if ( bag == 0 )
            bag = new PropertyBag();
        bag->add( pb );
        return true;
    }
    void AttributeRepository::removeAttribute( const std::string& name ) {
        removeValue(name);
    }

  bool AttributeRepository::removeValue( const std::string& name )
  {
    map_t::iterator i = find_if( values.begin(), values.end(), bind(equal_to<std::string>(),name, bind(&AttributeBase::getName, _1)) );
    if ( i != values.end() ) {
        delete (*i);
        values.erase( i );
        return true;
    }
    return false;
  }

  AttributeBase* AttributeRepository::getValue( const std::string& name ) const
  {
    map_t::const_iterator i = find_if( values.begin(), values.end(), bind(equal_to<std::string>(),name, bind(&AttributeBase::getName, _1)) );
    if ( i == values.end() ) return 0;
    else return *i;
  }

  bool AttributeRepository::hasAttribute( const std::string& name ) const
  {
    map_t::const_iterator i = find_if( values.begin(), values.end(), bind(equal_to<std::string>(),name, bind(&AttributeBase::getName, _1)) );
    return i != values.end();
  }

  bool AttributeRepository::hasProperty( const std::string& name ) const
  {
      return (bag && bag->find(name) != 0);
  }

    bool AttributeRepository::removeProperty( PropertyBase* p )
    {
        if ( bag && bag->find( p->getName() ) ) {
            bag->remove(p);
            removeValue( p->getName() );
            return true;
        }
        return false;
    }

    std::vector<std::string> AttributeRepository::names() const { return this->getAttributes(); }

    std::vector<std::string> AttributeRepository::getAttributes() const
    {
        std::vector<std::string> ret;
        std::transform( values.begin(), values.end(), back_inserter(ret), bind(&AttributeBase::getName, _1) );
        return ret;
    }

    PropertyBag* AttributeRepository::properties() const
    {
        if ( bag == 0 )
            bag = new PropertyBag();
        return bag;
    }
}
