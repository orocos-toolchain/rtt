/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:19:04 CEST 2002  Processor.hpp

                        Processor.hpp -  description
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


#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include "ProcessorInterface.hpp"
#include "ProcessorControlInterface.hpp"
#include "ProcessorState.hpp"
#include "ProcessorStateInit.hpp"


namespace ORO_Execution
{

    /**
     * This class represents a controllable execution engine.
     * (see interfaces)
     */
    class Processor: public ProcessorInterface, public ProcessorControlInterface
    {

        public:

            /**
             * Constructs a new Processor
             *
             * @post the processor is created and set in init/idle mode
             */
            Processor();

            virtual ~Processor();

			//see ProcessorControlInterface
            virtual void startConfiguration();
			virtual void abort();
			virtual void endConfiguration();
			virtual void deleteProgram();
			virtual void startExecution();
			virtual void stopExecution();

			//see ProcessorInterface
			virtual void loadSystemContext(SystemContext* sc);
			virtual void loadProgram(ProgramInterface* pi) ;
			virtual void resetProgram() ;
			virtual void doStep();


        private:

        	friend class ProcessorState;
        	//state setters
        	virtual void changeState(ProcessorState* newState);
            virtual void resetState();

            /**
             * current state of this processor
             *
             * @invar currentState != 0
             */
            ProcessorState* currentState;

    };

}

#endif
