/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:33:20 CEST 2002  EventTopHalfInterface.hpp 

                       EventTopHalfInterface.hpp -  description
                          -------------------
   begin                : Mon June 10 2002
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


#ifndef EVENT_TOP_HALF_INTERFACE
#define EVENT_TOP_HALF_INTERFACE

struct pt_regs;

namespace ORO_OS
{

    /**
     * This class provides functionality for an Interrupt Service Routine
     * (ISR) similar to the one in the Linux Kernel.
     * @invar handleInterrupt may only be called when an actual interrupt occured
     */
    class ISRInterface
    {

        public:
            virtual ~ISRInterface()
            {}

            /**
             * Handle a system interrupt
             * @param irq    
             *       | The number of the occuring interrupt
             * @param dev_id 
             *       | Cast this to EventInterrupt to know thou boss (ugly, no ?)
             *         So that you can request completion
             * @param regs   
             *       | Processor status information
             * @pre   The system interrupt <irq> occured 
             */

            virtual void handleInterrupt( int irq, void *dev_id, struct pt_regs* regs ) = 0;
    };

} // namespace ORO_OS

#endif
