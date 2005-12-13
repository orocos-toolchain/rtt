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
#include "CommandProcessor.hpp"
#include "CommandC.hpp"
#include <map>

namespace ORO_Execution 
{
    class ExecutionEngine;

    /**
     * @brief A browsable interface for command factories.
     *
     * A task registers its factories under a certain name, and then
     * the user can come and ask us for the factory by a certain name.
     * For example, to invoke a call to "object.command()", get the
     * CommandFactory under the name object, and then ask that
     * CommandFactory for the command.
     * @ingroup globalFactory
     */
    class GlobalCommandFactory
    {
      typedef std::map<std::string, CommandFactoryInterface*> map_t;
      typedef map_t::iterator iter_t;
      typedef map_t::const_iterator const_iter_t;
        ExecutionEngine* eproc;
    public:
        /**
         * Create a command factory which produces commands which must
         * be executed by \a cp.
         */
      GlobalCommandFactory(ExecutionEngine* ee);
      ~GlobalCommandFactory();

      /**
       *  @brief Check whether the factory registered with a given object,
       *  contains a given command.
       *
       * @param objectname a registered object
       * @param	com a command
       */
      bool hasCommand(const std::string& objectname,const std::string& com) const;

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

        /**
         * Get a list of all registered factories.
         */
        std::vector<std::string> getObjectList() const;

        /**
         * Get the command processor which processes the
         * commands created by this factory.
         */
        CommandProcessor* getCommandProcessor() const;

        /**
         * Create a command.
         * Use this function as in:
         @verbatim
         CommandC cc = create("this", "doSomething").arg( 3 ).arg(d).arg(5.0);
         cc.execute();
         // lateron ...
         if ( cc.evaluate() ) {
            // ...
         }
         @verbatim
         * Also variables or reference to variables may be given
         * within arg().
         * @see CommandC
         * @throw name_not_found_exception
         * @throw wrong_number_of_args_exception
         * @throw wrong_types_of_args_exception
         */
        CommandC create(const std::string& object,
                        const std::string& command) const {
            return CommandC(this, object, command, false);
        }
                        
    private:
      /**
       * our data..
       */
      map_t mdata;
    };
}

#endif
