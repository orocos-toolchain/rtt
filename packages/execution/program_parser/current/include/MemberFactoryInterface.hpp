/***************************************************************************
  tag: Peter Soetens  Thu May 6 17:09:06 CEST 2004  MemberFactoryInterface.hpp 

                        MemberFactoryInterface.hpp -  description
                           -------------------
    begin                : Thu May 06 2004
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

#ifndef MEMBERFACTORYINTERFACE
#error "This is an internal headerfile" 
#endif
 
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
  class MEMBERFACTORYINTERFACE
  {
  public:
    virtual ~MEMBERFACTORYINTERFACE();

    /**
     * Returns a list of all available names..
     * throws nothing.
     */
    virtual std::vector<std::string> getNames() const = 0;

    /**
     * Returns true if data is available by the name s.  Equivalent to
     * checking whether s is in dataNames(), but probably faster..
     * throws nothing.
     */
    virtual bool hasName( const std::string& s ) const = 0;

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

}
