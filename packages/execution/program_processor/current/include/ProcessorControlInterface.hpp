/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:19:04 CEST 2002  ProcessorControlInterface.hpp 

                        ProcessorControlInterface.hpp -  description
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
 
 
#ifndef PROCESSORCONTROLINTERFACE_HPP
#define PROCESSORCONTROLINTERFACE_HPP

namespace ORO_Execution
{

    /**
     * This abstract class provides functions that control 
     * its configuration state.
     */
    class ProcessorControlInterface
    {

        public:
        
            virtual ~ProcessorControlInterface()
            {}

            /**
             * Start execution mode, only if not yet started 
             * and data has been loaded. 
             */
            virtual void startExecution() = 0;

            /**
             * End execution mode, only if already started.
             */
            virtual void stopExecution() = 0;
            
            /**
             * Start configuration mode.
             */
            virtual void startConfiguration() = 0;
                        
            /**
             * Discard the data currently loaded.
             * If that data was being executed, execution stopped.
             */
            virtual void deleteProgram() = 0;
            
			/**
			 * Retreat to init mode (data and configuration were discarded).
			 */
			virtual void abort() = 0;
			
			/**
			 * End configuration mode and start loading mode, only
			 * if a configuration has been loaded.
			 */
			virtual void endConfiguration() = 0; 
			
			
    };

}

#endif
