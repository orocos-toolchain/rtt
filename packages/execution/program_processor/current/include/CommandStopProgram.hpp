/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  CommandStopProgram.hpp 

                        CommandStopProgram.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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
	
		std::string toString() {
			return "CommandStopProgram";
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
