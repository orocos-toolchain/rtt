/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:19:04 CEST 2002  CommandNOP.hpp 

                        CommandNOP.hpp -  description
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
 
 
#ifndef COMMANDNOP_HPP
#define COMMANDNOP_HPP

#include "CommandInterface.hpp"

namespace ORO_CoreLib
{

	using namespace ORO_CoreLib;
	
    /**
     * The empty command.
     *
     * This command does nothing when executed.
     */
    class CommandNOP : public CommandInterface
    {
    public:
        /**
         * Create an empty command.
         */
        CommandNOP() {}

        virtual ~CommandNOP() {}

        virtual void execute();

        virtual std::string toString();

        virtual CommandInterface* clone() const;
    };

}

#endif
