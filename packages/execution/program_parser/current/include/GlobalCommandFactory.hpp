/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  GlobalCommandFactory.hpp 

                        GlobalCommandFactory.hpp -  description
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
 
#ifndef GLOBALCOMMANDFACTORY_HPP
#define GLOBALCOMMANDFACTORY_HPP

#include "CommandFactoryInterface.hpp"
#include <map>

namespace ORO_Execution
	{

    /**
     * @brief This class is sort of a registry for command factories..
     *
     * Components can register their factory with us under a certain
     * name, and then the parser can come and ask us for the factory
     * by a certain name..  For example, if the parser sees a call to
     * "object.method()", then it will ask us for the CommandFactory
     * under the name object, and then ask that CommandFactory for the
     * command method.  You can think of a CommandFactory as the
     * virtual table of the object, and this class would then be
     * something like a namespace linking names to the objects.
     */
    class GlobalCommandFactory
    {
      typedef std::map<std::string, CommandFactoryInterface*> map_t;
      typedef map_t::iterator iter_t;
      typedef map_t::const_iterator const_iter_t;
    public:
      GlobalCommandFactory();

      /**
       *  @brief Check whether the factory registered with a given object,
       *  contains a given command.
       *
       * @param objectname a registered object
       * @param	com a command
       */
      bool hasCommand(const std::string& objectname,const std::string& com);

      /*
       * registers a object together with a factory.
       *
       * @param	name	name of the object
       * @param	factory	description of the factory
       *
       */
      void registerObject( const std::string& objectname,
                           CommandFactoryInterface* cfi);

      /**
       * unregisters the object by the name objectname...
       */
      void unregisterObject( const std::string& objectname );

      /*
       * search for an object and return its factory
       * @param objectname name of the object
       * @return the requested factory, or 0, indicating no factory
       * has been registered under that name..
       */
      const CommandFactoryInterface* getObjectFactory(
        const std::string& objectname ) const;

        std::vector<std::string> getObjectList() const;
    private:
      /**
       * our data..
       */
      map_t mdata;
    };
}

#endif
