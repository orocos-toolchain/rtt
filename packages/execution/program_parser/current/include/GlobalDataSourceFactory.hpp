/***************************************************************************
  tag: Peter Soetens  Thu May 6 17:09:06 CEST 2004  GlobalDataSourceFactory.hpp 

                        GlobalDataSourceFactory.hpp -  description
                           -------------------
    begin                : Thu May 06 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 

#ifndef GLOBALDATASOURCEFACTORY_HPP
#define GLOBALDATASOURCEFACTORY_HPP

#include <string>
#include <vector>
#include <map>
#include "parser-common.hpp"
#include "FactoryExceptions.hpp"
#include "ArgumentDescription.hpp"
#include "DataSourceFactoryInterface.hpp"

namespace ORO_Execution
{

  /**
   * @brief This is a sort of registry for DataSourceFactory's.
   *
   * It is the
   * DataSource equivalent of GlobalCommandFactory.  You may be
   * interested to check the documentation of that class for more
   * info.. Components register and unregister their factories under a
   * certain name, and the parser can ask for the factory registered
   * under a certain name..
   */
  class GlobalDataSourceFactory
  {
    typedef std::map<std::string, const DataSourceFactoryInterface*> map_t;
  public:
    GlobalDataSourceFactory()
      {
      };

  bool hasData(
    const std::string& objectname,
    const std::string& source )
  {
    const DataSourceFactoryInterface* o = factory( objectname );
    if ( o ) return o->hasData( source );
    else return false;
  };

    /**
     * Register a factory under a certain name..  This does not
     * transfer ownership, the caller is responsible for making sure
     * the factory is deleted at the appropriate time ( of course, not
     * while the factory is registered with us.. )
     */
    void registerObject( const std::string& name,
                         const DataSourceFactoryInterface* fact )
      {
        mdata[name] = fact;
      };

    void unregisterObject( const std::string& name )
      {
        mdata.erase( name );
      };

    /**
     * @brief Ask for the factory registered by a certain name..
     *
     * @return the requested factory, or 0, indicating no factory
     * has been registered under that name..
     */
    const DataSourceFactoryInterface* factory( const std::string& name ) const
      {
        map_t::const_iterator i = mdata.find( name );
        if ( i == mdata.end() ) return 0;
        else return i->second;
      };

      std::vector<std::string> getObjectList() const
      {
          std::vector<std::string> mlist;
          for (map_t::const_iterator r = mdata.begin(); r != mdata.end(); ++r )
              mlist.push_back( r->first );
          return mlist;
      }
  private:
    map_t mdata;
  };
}

#endif
