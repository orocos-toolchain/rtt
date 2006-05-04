/***************************************************************************
  tag: Peter Soetens  Thu May 6 17:09:06 CEST 2004  GlobalDataSourceFactory.hpp

                        GlobalDataSourceFactory.hpp -  description
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

#ifndef GLOBALMEMBERFACTORY_HPP
#define GLOBALMEMBERFACTORY_HPP

#include <string>
#include <vector>
#include <map>
#include "FactoryExceptions.hpp"
#include "ArgumentDescription.hpp"
#include "MemberFactoryComposite.hpp"
#include "MethodC.hpp"

namespace ORO_Execution
{

  /**
   * @brief A browsable factory for task member functions.
   *
   * It is the DataSource equivalent of GlobalCommandFactory.  You may
   * be interested to check the documentation of that class for more
   * info. A task registers and unregister its factories under a
   * certain name, and the user can ask for the factory registered
   * under a certain name.
   * @ingroup globalFactory
   */
  class GlobalMemberFactory
  {
      typedef std::map<std::string, const MemberFactoryInterface*> map_t;
  public:
      GlobalMemberFactory();

      ~GlobalMemberFactory();

      /**
       * Create an invocable member function.
       * Use this function as in:
       @verbatim
       double d = 1.234, c = 0;
       MethodC cc = create("this", "methodName").arg( 3 ).arg(d).arg(5.0).ret(c);
       cc.execute();
       assert( c != 0 )
       @verbatim
       * Also variables or reference to variables may be given
       * within arg().
       * @see MethodC
       * @throw name_not_found_exception
       * @throw wrong_number_of_args_exception
       * @throw wrong_types_of_args_exception
       */
      MethodC create(const std::string& object,
                     const std::string& command) const;
                        
      /**
       * Inspect if a given object has a given member function.
       */
      bool hasMember(const std::string& objectname,
                     const std::string& fun ) const;

      /**
       * Register a factory under a certain name.  This does 
       * transfer ownership.
       */
      void registerObject( const std::string& name,
                           const MemberFactoryInterface* fact );

      /**
       * Remove a factory with a certain name.  This does 
       * destroy the factory.
       */
      void unregisterObject( const std::string& name );

      /**
       * @brief Get a factory registered by a certain name.
       *
       * @return The requested factory, or 0, indicating no factory
       * has been registered under that name.
       */
      const MemberFactoryInterface* getObjectFactory( const std::string& name ) const;

      /**
       * Get a list of all registered objects.
       */
      std::vector<std::string> getObjectList() const;

  private:
      map_t mdata;
  };
}

#endif
