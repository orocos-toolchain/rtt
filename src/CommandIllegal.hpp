/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:19:03 CEST 2002  CommandIllegal.hpp 

                        CommandIllegal.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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
 
#ifndef COMMANDILLEGAL_HPP
#define COMMANDILLEGAL_HPP

#include "CommandInterface.hpp"
#include <string>

namespace RTT
{

	
	
    /**
     * @brief A Command indicating that an error was encountered
     * somewhere, most likely during the construction of
     * another command.
     */
    class CommandIllegal
        : public CommandInterface
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

        void readArguments() {}

        virtual bool execute();

        /**
         * Preferably, this function is executed right after a
         * CommandIllegal has been created.
         * This function can give feedback about a failed command
         * before the CommandIllegal is actually executed
         */
        virtual void warn();

        virtual CommandInterface* clone() const
        {
            return new CommandIllegal( *this );
        }

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
