/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  CommandStopExecution.hpp 

                        CommandStopExecution.hpp -  description
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
 
 
#ifndef ORO_COMMAND_STOP_EXECUTION_HPP
#define ORO_COMMAND_STOP_EXECUTION_HPP

#include <corelib/CommandInterface.hpp>
#include "ProcessorControlInterface.hpp"

namespace ORO_Execution 
{
	using  ORO_CoreLib::CommandInterface;

	/**
     * @brief This command will instruct the Processor to
     * stop execution if it is executed.
     */
	class CommandStopExecution : public CommandInterface
	{
		
		public:
		CommandStopExecution(ProcessorControlInterface * _pci) : pci(_pci)
		{
		}
		
		~CommandStopExecution()
		{}
		
		void execute() 
        {
			pci->stopExecution();
		}
	
		std::string toString() {
			return "CommandStopExecution";
		}
	
        virtual CommandInterface* clone() const
        {
            return new CommandStopExecution( *this );
        }

		private:
			ProcessorControlInterface * pci;
	};

}
#endif
