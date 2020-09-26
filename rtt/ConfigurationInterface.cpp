/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:08 CET 2004  ConfigurationInterface.cxx

                        ConfigurationInterface.cxx -  description
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



#include "rtt/ConfigurationInterface.hpp"
#include "rtt/internal/mystd.hpp"
#include <functional>
#include <boost/bind.hpp>

namespace RTT {
    using namespace detail;
    using namespace std;
    using namespace boost;

  ConfigurationInterface::ConfigurationInterface()
  {
  }

  ConfigurationInterface::~ConfigurationInterface()
  {
      // we do not claim automatically ownership
      // call clear() manually to delete all contents.
  }

    ConfigurationInterface* ConfigurationInterface::copy( std::map<const DataSourceBase*, DataSourceBase*>& repl, bool inst ) const
    {
        ConfigurationInterface* ar = new ConfigurationInterface();
        for ( map_t::const_iterator i = values.begin(); i != values.end(); ++i ) {
            ar->setValue((*i)->copy( repl, inst ) );
        }
        return ar;
    }


  void ConfigurationInterface::clear()
  {
    for ( map_t::iterator i = values.begin(); i != values.end(); ++i )
      delete *i;
    values.clear();
    bag.clear();
  }

  bool ConfigurationInterface::setValue( AttributeBase* value )
  {
      if ( !value->getDataSource() || value->getName().empty() )
          return false;
      map_t::iterator i = find( values.begin(), values.end(), value );
      if ( i != values.end() ) {
          *i = value;
      } else
          values.push_back( value );
      return true;
  }

    bool ConfigurationInterface::addProperty( PropertyBase& pb ) {
        if ( bag.find( pb.getName() ) )
            return false;
        bag.add( &pb );
        return true;
    }
    void ConfigurationInterface::removeAttribute( const std::string& name ) {
        removeValue(name);
    }

  bool ConfigurationInterface::removeValue( const std::string& name )
  {
    map_t::iterator i = find_if( values.begin(), values.end(), boost::bind(equal_to<std::string>(),name, boost::bind(&AttributeBase::getName, _1)) );
    if ( i != values.end() ) {
        delete (*i);
        values.erase( i );
        return true;
    }
    return false;
  }

  AttributeBase* ConfigurationInterface::getValue( const std::string& name ) const
  {
    map_t::const_iterator i = find_if( values.begin(), values.end(), boost::bind(equal_to<std::string>(),name, boost::bind(&AttributeBase::getName, _1)) );
    if ( i == values.end() ) return 0;
    else return *i;
  }

  bool ConfigurationInterface::hasAttribute( const std::string& name ) const
  {
    map_t::const_iterator i = find_if( values.begin(), values.end(), boost::bind(equal_to<std::string>(),name, boost::bind(&AttributeBase::getName, _1)) );
    return i != values.end();
  }

  bool ConfigurationInterface::hasProperty( const std::string& name ) const
  {
      return (bag.find(name) != 0);
  }

    bool ConfigurationInterface::removeProperty( PropertyBase& p )
    {
        if ( bag.find( p.getName() ) ) {
            bag.remove(&p);
            return true;
        }
        return false;
    }

    std::vector<std::string> ConfigurationInterface::getAttributeNames() const
    {
        std::vector<std::string> ret;
        std::transform( values.begin(), values.end(), back_inserter(ret), boost::bind(&AttributeBase::getName, _1) );
        return ret;
    }

    void ConfigurationInterface::loadValues( AttributeObjects const& new_values) {
        values.insert(values.end(), new_values.begin(), new_values.end());
    }


    PropertyBag* ConfigurationInterface::properties()
    {
        return &bag;
    }
}

bool RTT::ConfigurationInterface::chkPtr(const std::string & where, const std::string & name, const void *ptr)
{
    if ( ptr == 0) {
        log(Error) << "You tried to add a null pointer in '"<< where << "' for the object '" << name << "'. Fix your code !"<< endlog();
        return false;
    }
    return true;
}


