/***************************************************************************
  tag: Peter Soetens  Wed Jul 14 17:58:21 CET 2004  CommandProgramEndToken.hpp

                        CommandProgramEndToken.hpp -  description
                           -------------------
    begin                : Wed July 14 2004
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

#ifndef ORO_COMMAND_PROGRAM_END_TOKEN_HPP
#define ORO_COMMAND_PROGRAM_END_TOKEN_HPP

#include <corelib/CommandInterface.hpp>

namespace ORO_Execution
{
    using  ORO_CoreLib::CommandInterface;

    /**
     * @brief This is a dummy Command which serves to signal the
     * ProgramGraph that the end of a program is reached.
     */
    class CommandProgramEndToken : public CommandInterface
    {
    public:
        void execute()
            {
            }

        virtual CommandInterface* clone() const
            {
                return new CommandProgramEndToken;
            }
    };
}
#endif
