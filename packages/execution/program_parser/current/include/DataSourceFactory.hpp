/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  DataSourceFactory.hpp 

                        DataSourceFactory.hpp -  description
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
 
#ifndef DATASOURCEFACTORY_HPP
#define DATASOURCEFACTORY_HPP

#include <string>
#include <vector>
#include <map>
#include "parser-types.hpp"
#include "FactoryExceptions.hpp"
#include "ArgumentDescription.hpp"

namespace ORO_Execution
{
  /**
   * @brief This is an interface for a factory generating DataSource's..
   *
   * This class is a close relative of CommandFactoryInterface, it
   * does basically the same thing, but for DataSource's..
   *
   * It is used by the parser to gain access to the properties that
   * the components export...  There is a generic implementation using
   * template techniques in TemplateDataSourceFactory, instead of
   * writing your own factory, you are very much encouraged to use
   * that one..
   *
   * Some of these methods throw exceptions that you can find in
   * FactoryExceptions.hpp
   */
  class DataSourceFactoryInterface
  {
  public:
    virtual ~DataSourceFactoryInterface();

    /**
     * Returns a list of all available names..
     * throws nothing.
     */
    virtual std::vector<std::string> dataNames() const = 0;

    /**
     * Returns true if data is available by the name s.  Equivalent to
     * checking whether s is in dataNames(), but probably faster..
     * throws nothing.
     */
    virtual bool hasData( const std::string& s ) const = 0;

      /**
       * @brief Return the description of a given Data Source.
       * @param source The DataSource to get the description from.
       */
      virtual std::string getDescription( const std::string& source ) const = 0;

      /**
       * @brief Return the list of arguments of a certain method.
       */
      virtual std::vector< ArgumentDescription > getArgumentList( const std::string& method ) const = 0;

    /**
     * Users of this class need to be able to find out what types of
     * arguments are needed for a certain datum.  This method returns
     * a PropertyBag containing Property's of the types of the
     * arguments..  The user can then fill up this PropertyBag, and
     * construct a command with it via the create method below.
     *
     * Note that the Properties in the property bag have been
     * constructed using new, and the caller gains their ownership..
     * You should explicitly call deleteProperties on the bag after
     * you're done with it..  Store the PropertyBag in a
     * PropertyBagOwner if you're affraid you'll forget it..
     *
     * TODO: fix this requirement somehow ?
     *
     * throws name_not_found_exception
     */
    virtual PropertyBag
    getArgumentSpec( const std::string& method ) const = 0;

    /**
     * construct the DataSource identified by the name name,
     * and the arguments args...  The PropertyBag should contain
     * Properties of the types returned by getArgumentSpec( name ), in
     * the same order.  This method does not delete the Properties, as
     * stated before, if you got them from getArgumentSpec, you're
     * responsible for that yourself..
     *
     * throws name_not_found_exception,
     * wrong_number_of_args_exception, wrong_types_of_args_exception
     */
    virtual DataSourceBase* create(
      const std::string& name,
      const PropertyBag& args ) const = 0;

    /**
     * We also support passing DataSources as the arguments.  In this
     * case, the factory should *not* read out the DataSource's, but
     * it should return a DataSource that stores the DataSources, and
     * reads them out each time in its get() method.  The created
     * DataSource should of course keep a reference to the
     * DataSource's it holds.  These are complicated requirements, you
     * are encouraged to look at the TemplateDataSourceFactory to do
     * the work for you..
     *
     * throws name_not_found_exception,
     * wrong_number_of_args_exception, wrong_types_of_args_exception
     */
    virtual DataSourceBase* create(
      const std::string& name,
      const std::vector<DataSourceBase*>& args ) const = 0;
  };

  /**
   * This is a sort of registry for DataSourceFactory's.  It is the
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
