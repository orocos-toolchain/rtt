/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:19:03 CEST 2002  CommandIllegal.hpp 

                        CommandIllegal.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef COMMANDILLEGAL_HPP
#define COMMANDILLEGAL_HPP

#include <corelib/CommandInterface.hpp>

namespace ORO_Execution
{

	using namespace ORO_CoreLib;
	
    /**
     * A Command indicating that an error was encountered
     * somewhere, most likely during the construction of
     * another command.
     */
    class CommandIllegal : public CommandInterface
    {

        public:
            /**
             * Construct a new CommandIllegal with the illegal
             * std::stringcommand as argument
             *
             * @param cmd
             *     command that failed
             */
            CommandIllegal( const std::string& cmd );

            /**
             * Construct a new CommandIllegal with the illegal
             * std::stringcommand as argument and a description
             *
             * @param cmd
             *     command that failed
             * @param descr
             *     description
             */
            CommandIllegal( const std::string& cmd, const std::string& descr );

            virtual void execute();

            /**
             * Preferably, this function is executed right after a
             * CommandIllegal has been created.
             * This function can give feedback about a failed command
             * before the CommandIllegal is actually executed
             */
            virtual void warn();

            /**
             * CommandIllegal -> nothing really happened --> do nothing
             */
            void undo()
            {}

            std::string toString();

        private:
            /**
             * Forbidden
             */
            CommandIllegal();

            std::string illCmd;
            std::string description;
    };

}

#endif //COMMANDILLEGAL_HPP
