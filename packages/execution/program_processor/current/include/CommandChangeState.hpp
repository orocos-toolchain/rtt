/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  CommandChangeState.hpp 

                        CommandChangeState.hpp -  description
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
 
#include "CommandInterface.hpp"
#include "SystemContext.hpp"
#include "SystemStateInterface.hpp"
#include <exception>

namespace ORO_Execution
{
    /**
     * @brief A command that changes the State of the System.
     */
	class CommandChangeState
        : public CommandInterface
	{
		
		public:
		CommandChangeState(SystemContext* _sc, SystemStateInterface * _sti)
            : sc(_sc), sti(_sti) {
		}
		
		~CommandChangeState()
		{}
		
		void execute() {
			sc->requestState(sti);
		}
		
		std::string toString() {
			return "CommandChangeState";
		}
	
        virtual CommandInterface* clone() const
        {
            // shalow copy, we do not own the pointers
            return new CommandChangeState(*this);
        }

		private:
			SystemContext * sc;
			SystemStateInterface * sti;

	};

}
