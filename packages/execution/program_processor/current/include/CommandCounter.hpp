/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  CommandCounter.hpp 

                        CommandCounter.hpp -  description
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
 
 
#ifndef COMMANDCOUNTER_HPP
#define COMMANDCOUNTER_HPP

#include <corelib/CommandInterface.hpp>

namespace ORO_Execution
{

	using  ORO_CoreLib::CommandInterface;
	
    /**
     * This command increments a counter when executed. 
     * (Currently solely used by jilske for testing purposes.)
     */
    class CommandCounter
        : public CommandInterface
    {

        public:
        
            CommandCounter();

            virtual ~CommandCounter();

            virtual void execute();

            virtual std::string toString();
            
            /**
             * set number of times executed to zero
             */
            virtual void reset();
            
            /**
             * return the number of times this command has been 
             * executed since the last reset
             */
            virtual int getCounter();
        
        virtual CommandInterface* clone() const
        {
            return new CommandCounter( *this );
        }

        private:
        	int counter;
    };

}

#endif

