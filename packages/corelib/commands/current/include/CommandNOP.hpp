/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:19:04 CEST 2002  CommandNOP.hpp 

                        CommandNOP.hpp -  description
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
 
#ifndef COMMANDNOP_HPP
#define COMMANDNOP_HPP

#include "CommandInterface.hpp"

namespace ORO_CoreLib
{
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
