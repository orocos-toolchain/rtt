/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:43:19 CEST 2002  ComponentOneShot.hpp 

                       ComponentOneShot.hpp -  description
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



#ifndef COMPONENT_OneShot_HPP
#define COMPONENT_OneShot_HPP

#include "os/cpp.hpp"
#include <os/fosi.h>
#include "RunnableInterface.hpp"
#include "Task.hpp"

/**
 * A one shot component 
 * 
 * The ComponentOneShot can only be started once. This might be unsatis-
 * factory for some applications.  We might change that in the future.
 *
 */

namespace ORO_OS
{

    class ComponentOneShot : private Task
    {

        public:
            ComponentOneShot( RunnableInterface* r = 0 );
            virtual ~ComponentOneShot();
            /**
             * Start the Component
             */
            bool start();
            /**
             * This method will be executed once,
             * when the Component is start()'ed
             */
            virtual void step()
            {}

            ;
            /**
             * Set the name of this task
             */
            void taskNameSet( const char* );
            /**
             * Read the name of this task
             */
            char* taskNameGet();

            virtual int run();

        protected:
	    const static int TASKNAME_SIZE = 64;
            char taskName[ TASKNAME_SIZE ];

        private:

            /**
             * When set to 1, the action will start
             */
            int running;

            RunnableInterface* runComp;
    };


}

#endif
