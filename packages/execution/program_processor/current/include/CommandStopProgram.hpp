/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  CommandStopProgram.hpp 

                        CommandStopProgram.hpp -  description
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
 
#ifndef ORO_COMMAND_STOP_PROGRAM_HPP
#define ORO_COMMAND_STOP_PROGRAM_HPP

#include <corelib/CommandInterface.hpp>
#include "ProcessorInterface.hpp"

namespace ORO_Execution
{
	using  ORO_CoreLib::CommandInterface;

	/**
     * @brief This command will instruct the Processor to
     * stop program if it is executed.
     */
	class CommandStopProgram : public CommandInterface
	{
		
		public:
		CommandStopProgram(ProcessorInterface * _pci, const std::string prog_name) : pci(_pci), name(prog_name)
		{
		}
		
		~CommandStopProgram()
		{}
		
		void execute() 
        {
			pci->stopProgram(name);
		}
	
        virtual CommandInterface* clone() const
        {
            return new CommandStopProgram( pci, name );
        }

		private:
			ProcessorInterface * pci;
        std::string name;
	};

}
#endif
