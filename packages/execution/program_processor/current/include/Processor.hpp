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

#include <list>

namespace ORO_Execution
{

    /**
     * This class represents a controllable execution engine.
     * (see interfaces)
     */
    class Processor
        : public ProcessorInterface
    {

        public:

            /**
             * Constructs a new Processor
             *
             * @post the processor is created and set in init/idle mode
             */
            Processor();

            virtual ~Processor();

//             virtual bool startConfiguration();
// 			virtual bool abort();
// 			virtual bool endConfiguration();
			virtual bool startStepping(const std::string& name);

			virtual bool loadStateContext(const std::string& name, StateContext* sc);
            virtual bool startStateContext(const std::string& name);
            virtual bool stopStateContext(const std::string& name);
            virtual bool resetStateContext(const std::string& name);
            virtual bool deleteStateContext(const std::string& name);

			virtual bool loadProgram(const std::string& name, ProgramInterface* pi) ;
			virtual bool startProgram(const std::string& name);
			virtual bool stopProgram(const std::string& name);
			virtual bool resetProgram(const std::string& name);
			virtual bool deleteProgram(const std::string& name);

			virtual void doStep();
			virtual bool nextStep(const std::string& name);
			virtual bool process(CommandInterface* c);

        struct ProgramInfo
        {
            ProgramInfo(const std::string&_name, ProgramInterface* p)
                : program(p),
                  running(false),
                  stepping(false), name(_name) {}
            ProgramInterface* program;
            bool running;
            bool stepping;
            std::string name;
        };

        struct StateInfo
        {
            StateInfo(const std::string& _name, StateContext* s)
                : state(s),
                  init(false),running(false),final(false),
                  name(_name) {}
            StateContext* state;
            bool init;
            bool running;
            bool final;
            std::string name;
        };

    private:

        //friend class ProcessorState;
        //state setters
        //virtual void changeState(ProcessorState* newState);
        //virtual void resetState();

        /**
         * current state of this processor
         *
         * @invar currentState != 0
         */
        //ProcessorState* currentState;

        typedef std::list<ProgramInfo>::iterator program_iter;
        typedef std::list<StateInfo>::iterator state_iter;
        std::list<ProgramInfo> programs;
        std::list<StateInfo>   states;

        CommandInterface *command;
    };

}

#endif
